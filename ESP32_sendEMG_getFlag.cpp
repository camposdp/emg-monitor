#include <Arduino.h>
#include "WiFi.h"
#include <ArduinoJson.h>
#include <math.h>


// Replace with your network credentials
const char* ssid     = "ESP32-Access-Point";
const char* password = "123456789";

// Set web server port number to 80
WiFiServer server(80);

#define BUFF_SIZE 64
#define Ts 1000



char a = 'a';
char s = 's';
char d = 'd';
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
String command_th;
String command_val;
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
    command_th = command[0];
    command_val = command.substring(1);
    //Serial.print(command_th);
    //Serial.print("?");
    //Serial.print(command_val);
    //Serial.print("!");
    Serial.print(command_val);
    if (command_th.equals("a")){
      digitalWrite(LED_BUILTIN, 1);
      //Serial.print(command_th);
      Serial2.print(a);
    } 
    else{
      digitalWrite(LED_BUILTIN, 0);
    }

    if (command_val.equals("s")){
      Serial2.print(s);
    }

    if (command_val.equals("d")){
      Serial2.print(d);
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
      
    temBufferPronto--;   
    //Serial.println("Sending data...");
    client.print(prepararDado());

    while (client.available()>0) {
    //Serial.println("bop..."); 
    command = client.readStringUntil('\n');
    command.trim();        //kill whitespace
    ledFlag();
    }
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

  //Serial.begin(115200);
  Serial.begin(9600);
  Serial2.begin(9600);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  server.begin();


  
  pinMode (LED_BUILTIN, OUTPUT);
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, Ts, true);
  timerAlarmEnable(timer);
  Serial.println("Timer configurado!");

}

void loop() {
WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
     // if (client.available()) {             // if there's bytes to read from the client,
        

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
  
    client.print(prepararDado());
    

    command = client.readStringUntil('\n');
    command.trim();        //kill whitespace
    ledFlag();

  }

  client.stop();
  Serial.println("Client disconnected");

}
}
  










