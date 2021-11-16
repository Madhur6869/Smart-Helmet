#include <TinyGPS++.h> 
#include <ESP8266WiFi.h>                                                    
#include <SoftwareSerial.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <WiFiClientSecure.h>

const char* ssid = "xxxxxx";//Wifi credentials
const char* password = "xxxxx";

//define the parameters for the IFTTT
#define HOSTIFTTT "maker.ifttt.com"
#define EVENTO "accident"
#define IFTTTKEY "xxxxxxxxx"
#include "AnotherIFTTTWebhook.h" //For Webhooks IFTTT

//Adafruit MQTT
#define MQTT_SERV "io.adafruit.com"
#define MQTT_PORT 1883
#define MQTT_NAME "Madhur01"
#define MQTT_PASS "aio_VYIo92aidsxEVuASFObbDuvksPho"
Adafruit_MQTT_Client mqtt(&client, MQTT_SERV, MQTT_PORT, MQTT_NAME, MQTT_PASS);
Adafruit_MQTT_Publish resetbutton = Adafruit_MQTT_Publish(&mqtt, MQTT_NAME "/feeds/Helmet"); // Publishing to Status
Adafruit_MQTT_Subscribe Status = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/feeds/Helmet"); //Subscription to check the status 

//WiFiClient client;  Already defined in imported file


void MQTT_connect();//Initialize function for MQTT Adafruit

float latitude , longitude, la,lo; //Variables for Gps module

void setup() {
  Serial.begin(115200);
  delay(2000);
  WiFi.begin(ssid, password); //Connect to WiFi
  Serial.println("");
  Serial.print("Waiting to connect to WiFi... ");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  delay(1000);
// Setup MQTT subscription for status feed.
  mqtt.subscribe(&Status);
   MQTT_connect();
//Check input from Arduino
  pinMode(D6,INPUT);
 }


void loop() {
  //Check for free fall
 start: if(digitalRead(D6)==1){//Input from Arduino
     MQTT_connect();
     Serial.println("Delay start");
     delay(10000); //To consider false alarm, wait for the user to set the status to OFF
     Serial.println("Delay end");
      Adafruit_MQTT_Subscribe *subscription;
     while ((subscription = mqtt.readSubscription(200))) //For subscription
      { Serial.println("Inside loop");
         if (subscription == &Status) 
         {          
         Serial.print("STATUS:");
         Serial.println((char *)Status.lastread); //Value is OFF: 
           if (!strcmp((char*)Status.lastread, "OFF"))
           {
//               Serial.println("False alarm detected Turning system back on");
                 resetbutton.publish("ON");
                 goto start;//Start function again
           }               
         }       
       }
          //when real fall is detected
          Serial.println("Fall detected");                  
                gps.get_position(&la,&lo);//GPS Input
 
                //converting degreemin to Decimal degree
                float minut=la.tofloat();
                minut=minut/60;
                float degree= la.tofloat();
                latitude=degree+minut;
                float minut=lo.tofloat();
                minut=minut/60;
                float degree= lo.tofloat();
                longitude=degree+minut;
                
                //Gmaps link
                String link;
                  link="http://maps.google.com/?q=12.9717+79.1594";
//                link="http://maps.google.com/?q=" +String(latitude) + '+' + String(longitude);
                 char linkcharBuf[50];
                 link.toCharArray(linkcharBuf, 50);
                //Triggering IFTTT
                send_webhook(EVENTO,IFTTTKEY,linkcharBuf,"","");                

                while(1){//Infinite loop after Emergency request is sent
                Serial.println("Accident has happened. Emergeny request sent");
                delay(20000);
                }
  }
  
}
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
