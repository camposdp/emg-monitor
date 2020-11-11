#include <Arduino.h>
#include "WiFi.h"
#include <ArduinoJson.h>
#include <math.h>

#define BUFF_SIZE 64
//#define WIN_SIZE 32
#define Ts 1000


float startVelocity = 10.80;



const char* ssid = "Daniel 1206 2.4 GHz";
const char* password = "BDQ-6J26";
const char * host = "192.168.1.10";
//const char* ssid = "ESP32";
//const char* password = "emg123";
WiFiServer wifiServer(80);

char c = 'c';
volatile int temBufferPronto;
volatile int flagAcq = 0;
int bufferIndex = 0;
const uint16_t port = 15200;
int contador_de_timeout = 0;
const int analogIn = 4;
int  MValue=0;
int TempC;
float Voltage;
float x = 0;
int bip = 1;
WiFiClient client;
String command;
unsigned long t0, t1, t2;
int LED_BUILTIN = 2;
int led_sts = 1;
float buf1[BUFF_SIZE];
float buf2[BUFF_SIZE];
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
const size_t CAPACITY = JSON_ARRAY_SIZE(BUFF_SIZE);

String prepararDado(){

    // allocate the memory for the document
    StaticJsonDocument<CAPACITY> doc;

    // create an empty array
    JsonArray msg = doc.to<JsonArray>();
    for (int i=0; i< BUFF_SIZE; i++)
        msg.add(buf1[i]);

    // serialize the array and sed the result to Serial
    String json;
    serializeJson(msg, json);
    //Serial.println("Data ready to send...");
    return json;
}


void flipLED(){
    if (led_sts == 1){
      led_sts = 0;
    }
    else{
      led_sts = 1;
    }
    digitalWrite(LED_BUILTIN, led_sts);
}

void ledFlag(){
    //Serial.print(command);
    //Serial.print("!");
    if (command.equals("a")){
      digitalWrite(LED_BUILTIN, 1);
    } 
    else{
      digitalWrite(LED_BUILTIN, 0);
    }

}

void montarBuffer(){
  
  buf1[bufferIndex] = Voltage;
  bufferIndex++;

  if (bufferIndex > BUFF_SIZE-1){

    temBufferPronto++;
    bufferIndex = 0;
    //Serial.println("Buffer Ready");
  }
}


void doThings(){


    

  if (temBufferPronto > 0) {
    //t0 = micros();
      
    temBufferPronto--;   
    //Serial.println("Enviando dados");
    
    client.print(prepararDado());

    while (client.available()>0) {
    Serial.println("bop..."); 
    command = client.readStringUntil('\n');
    command.trim();        //kill whitespace
    ledFlag();
    }
    
    
    //t1 = micros();
    //Serial.println(t1-t0);


  }

}


void leituraEMG(){

  MValue = analogRead(34);
  //MValue = hpFilter.input(analogRead(34));
  Voltage = ((((3.3 / 4096.0) *(MValue))-1.6));

}

 
void IRAM_ATTR onTimer() {

  portENTER_CRITICAL_ISR(&timerMux);
   flagAcq = 1;
  portEXIT_CRITICAL_ISR(&timerMux);
  
}
/*
void clientConnect(){
    //Serial.print("Conectando ao servidor... ");
    if (!client.connect(host, port)) {
        Serial.println("Falha na conexão");
        delay(5000);
        return;
    }else{
        Serial.println("Conectado");
        delay(1000);
    }
}
*/




void setup() {
  
  Serial.begin(115200);
  Serial.println("Serial Iniciada!");
  
  /*
  WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Conectando ao Wifi");
    while (WiFi.status() != WL_CONNECTED && contador_de_timeout<=20) {
        delay(500);
        Serial.print("...");
        contador_de_timeout += 1;
     }
    
    if(contador_de_timeout<=19){
    Serial.print(" WiFi conectado com IP: ");
    Serial.println(WiFi.localIP());
    contador_de_timeout = 0;
    delay(100);
    } else{
        Serial.print(" Falha na conexão com WiFi: ");
        contador_de_timeout = 0;
        delay(100);
    }
 */

  delay(1000);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());
 
  wifiServer.begin();
  
  pinMode (LED_BUILTIN, OUTPUT);
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, Ts, true);
  timerAlarmEnable(timer);
  Serial.println("Timer configurado!");

}

void loop() {


WiFiClient client = wifiServer.available();
 
if (client) {
  Serial.println("bip...");
  bip = 1;
  while (client.connected()) {

        

    while (temBufferPronto < 1) {
      if (flagAcq>0){

      leituraEMG();
      montarBuffer();

      portENTER_CRITICAL(&timerMux);
      flagAcq = 0;
      portEXIT_CRITICAL(&timerMux);
      }
    }

    temBufferPronto--;   
    //Serial.println("Enviando dados");
  
    client.print(prepararDado());
    
    //while (client.available()>0) {
    //Serial.println("bop..."); 
    command = client.readStringUntil('\n');
    command.trim();        //kill whitespace
    ledFlag();
    //}


    
    //while (client.available()>0) {
      //}
     
    //client.write(c);  
    //if (flagAcq>0){





    //}  

    
    //delay(1);
    
  }

  client.stop();
  Serial.println("Client disconnected");

}
}
  






/*


#include  <WiFi.h>
#include  <EEPROM.h>
//#include  <ESP32CAN.h>
//#include  <CAN_config.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <math.h>
//#include <Filters.h>

#define BUFF_SIZE 128
//#define WIN_SIZE 32
#define fs 1000

volatile int temBufferPronto;
volatile int flagAcq = 0;
int bufferIndex = 0;
//int winIndex = 0;

//float filterFrequency = 5.0  
// create a one pole (RC) lowpass filter
//FilterOnePole hpFilter( HIGHPASS, filterFrequency );   

const char ssid[] = "teste";
const char password[] = "1234567890";
const int WiFi_Channel = 1;    //1~13
const int SSID_Hidden = 0;  
const int maxConnection = 4;  //1~4

uint16_t time_ST = 0;
uint8_t flag_Time_App = 0;

uint8_t tx_Velocity[30];
uint8_t tx_Bat_Level[30];
uint8_t tx_Error[30];
uint8_t tx_General_Odo[30];
uint8_t tx_Trip_Odo[30];
uint8_t tx_Blink_Alert[30];
uint8_t tx_Light[30];

uint8_t _receivedMessage[30];
uint8_t receivedMessage[30];
uint8_t user[10];
uint8_t key[10];





uint8_t readEEPROM = 0;

uint8_t end_key = 0;

WiFiServer server(23);

//const char* ssid = "Daniel 1206 2.4 GHz";
//const char* password = "BDQ-6J26";
//const char * host = "192.168.1.10";
//const uint16_t port = 15200;

int contador_de_timeout = 0;
const int analogIn = 4;
int  MValue=0;
int TempC;
float Voltage;
//float RMS;
//float sumSquare = 0;
float x = 0;
//float w = 1;
//int leituras = 1;
//WiFiClient client;
String command;



unsigned long t0, t1, t2;

//int totalInterruptCounter;
 
int LED_BUILTIN = 2;
int led_sts = 1;
float buf1[BUFF_SIZE];
float buf2[BUFF_SIZE];

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;


const size_t CAPACITY = JSON_ARRAY_SIZE(BUFF_SIZE);

String prepararDado(){

    // allocate the memory for the document
    StaticJsonDocument<CAPACITY> doc;

    // create an empty array
    JsonArray msg = doc.to<JsonArray>();
    for (int i=0; i< BUFF_SIZE; i++)
        msg.add(buf1[i]);

    // serialize the array and sed the result to Serial
    String json;
    serializeJson(msg, json);
    //Serial.println("Dado Pronto para envio...");
    return json;
}

void flipLED(){
    if (led_sts == 1){
      led_sts = 0;
    }
    else{
      led_sts = 1;
    }
    digitalWrite(LED_BUILTIN, led_sts);
}

void ledFlag(){
    Serial.print(command);
    //const char * string1 = command.c_str();
    Serial.print("!");
    if (command.equals("a")){
      digitalWrite(LED_BUILTIN, 1);
    } 
    else{
      digitalWrite(LED_BUILTIN, 0);
    }

}

void montarBuffer(){
  
  buf1[bufferIndex] = Voltage;
  bufferIndex++;

  if (bufferIndex > BUFF_SIZE-1){

    temBufferPronto++;
    bufferIndex = 0;

    //Serial.println("Buffer Pronto");
    //x=1;

  }


}

void leituraEMG(){
  

  MValue = analogRead(34);
  //MValue = hpFilter.input(analogRead(34));
  Voltage = ((((3.3 / 4096.0) *(MValue))-1.6));

}


 
void IRAM_ATTR onTimer() {


  
  portENTER_CRITICAL_ISR(&timerMux);
   flagAcq = 1;
  portEXIT_CRITICAL_ISR(&timerMux);
  
  
}





void setup() {
  

  Serial.begin(9600);
  Serial.println("Serial Iniciada!");
	//WiFi.mode(WIFI_AP);
	WiFi.softAP(ssid, password, WiFi_Channel, SSID_Hidden, maxConnection);
	IPAddress IP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(IP);
	server.begin();

 
  

  pinMode (LED_BUILTIN, OUTPUT);
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, fs, true);
  timerAlarmEnable(timer);
  Serial.println("Timer configurado!");

}

void loop() {

  WiFiClient client = server.available();
  
	//receivedCAN_Message();

	if(client)
	{
    Serial.println("Oi");
		while(client.connected())
		{
			while(client.available()>0)
			{

 
        if (flagAcq>0){

        leituraEMG();
        montarBuffer();

        portENTER_CRITICAL(&timerMux);
        flagAcq = 0;
        portEXIT_CRITICAL(&timerMux);
        }
          

        if (temBufferPronto > 0) {
          //t0 = micros();
          

          temBufferPronto--;   
          
          client.print(prepararDado());
          command = client.readStringUntil('\n');
          command.trim();        //kill whitespace
          //Serial.print(command);
          ledFlag();
          //flipLED();
          client.stop();
          
          
          //t1 = micros();
          //Serial.println(t1-t0);

          //totalInterruptCounter++;


        }

      }
    }
  }
}



*/