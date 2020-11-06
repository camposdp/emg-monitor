#include <Arduino.h>
#include "WiFi.h"
#include <ArduinoJson.h>
#include <math.h>

#define BUFF_SIZE 64
//#define WIN_SIZE 32
#define Ts 1000

const char* ssid = "SSID";
const char* password = "PW";
const char * host = "0.0.0.0";

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



void setup() {
  
  Serial.begin(115200);
  Serial.println("Serial Iniciada!");
  
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
 
  

  pinMode (LED_BUILTIN, OUTPUT);
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, Ts, true);
  timerAlarmEnable(timer);
  Serial.println("Timer configurado!");

}

void loop() {

 
  if (flagAcq>0){

  leituraEMG();
  montarBuffer();

  portENTER_CRITICAL(&timerMux);
  flagAcq = 0;
  portEXIT_CRITICAL(&timerMux);
  }
    

  if (temBufferPronto > 0) {
    t0 = micros();
    
    clientConnect();   
    temBufferPronto--;   
    //Serial.println("Enviando dados");
    client.print(prepararDado());
    command = client.readStringUntil('\n');
    command.trim();        //kill whitespace
    ledFlag();
    client.stop();
    
    t1 = micros();
    Serial.println(t1-t0);


  }
}
  






