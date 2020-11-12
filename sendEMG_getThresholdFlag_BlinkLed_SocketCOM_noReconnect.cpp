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
  Voltage = ((((3.3 / 4096.0) *(MValue))-1.6));

}

 
void IRAM_ATTR onTimer() {

  portENTER_CRITICAL_ISR(&timerMux);
   flagAcq = 1;
  portEXIT_CRITICAL_ISR(&timerMux);
  
}




void setup() {
  
  Serial.begin(115200);
  Serial.println("Serial Iniciada!");
  
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


   
    
  }

  client.stop();
  Serial.println("Client disconnected");

}
}
  

