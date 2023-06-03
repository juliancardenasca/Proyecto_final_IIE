
#include <WiFi.h>
#include "secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros
#include "DHT.h"

char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

//seleccionar pines de los sensores y actuadores
const int DH11 = 15;
int MQ4pin = 36;
const int LED = 3;
const int Cooler = 19;

// Initialize our values
float tempDH11 = 0;
int humDH11 =0;
int MQvalue = 0;

//instanciar el sensor
DHT dht(DH11,DHT11);
String myStatus = "";

void setup() {
  Serial.begin(115200);  //Initialize serial
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  //pinMode(LED, LOW);
  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  dht.begin();
  pinMode(LED,OUTPUT);
  pinMode(Cooler,OUTPUT);

}

void loop() {

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }

    // change the values
  tempDH11 = dht.readTemperature();
  humDH11= dht.readHumidity();
  MQvalue = analogRead(MQ4pin);
  

  // set the fields with the values
  ThingSpeak.setField(3, tempDH11);
  ThingSpeak.setField(2, MQvalue);
  ThingSpeak.setField(1, humDH11);
  
  // set the status
  ThingSpeak.setStatus(myStatus);


  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("update successful.");
    Serial.println(tempDH11);
    Serial.println(MQvalue);
    Serial.println(humDH11);

  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  if(MQvalue > 1800 ){
    digitalWrite(LED,HIGH);
    digitalWrite(Cooler,HIGH);
    //delay(2000);
  }
  else{
    digitalWrite(LED,LOW);
    digitalWrite(Cooler,LOW);
  }
  if(tempDH11 > 35){
    digitalWrite(Cooler,HIGH);
  }
  else if(MQvalue < 1800){
    digitalWrite(Cooler,LOW);
  }

  delay(20000); // Wait 20 seconds to update the channel again
}
