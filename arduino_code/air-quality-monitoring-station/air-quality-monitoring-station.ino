//This code write for microcontroller that use ESP32
//Please make sure these libraries are in folder of arduino. #\Documents\Arduino\libraries 
#include "PMS.h"
PMS pms(Serial);
PMS::DATA data;

#include <WiFi.h> //ESP32 libraries
#include <WiFiUdp.h> 

#include <NTPClient.h> //NTP libraries

#include <Wire.h>

#include <IOXhop_FirebaseESP32.h> //Firebase libraries

#define FIREBASE_HOST "YOUR_FIREBASE_HOST_URL.firebaseio.com" //please paste in this and not include https://
#define FIREBASE_AUTH "YOUR_FIREBASE_DATABASE_SECRETS"



// Replace with your network credentials
const char* ssid     = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;


void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));
  pinMode(2, OUTPUT); // Declare the LED as an output
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  
  // Initialize Serial Monitor
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address
  
  Serial.println("WiFi connected."); //If wifi connected print connected
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

// Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(25200);  //GMT +7 = 25200
}
void loop() {
  delay(1000);
  // read data from PM detector sensor
//  if (pms.readUntil(data))
//  {
//    Serial.print("PM 1.0 (ug/m3): ");
//    Serial.println(data.PM_AE_UG_1_0);
//
//    Serial.print("PM 2.5 (ug/m3): ");
//    Serial.println(data.PM_AE_UG_2_5);
//
//    Serial.print("PM 10.0 (ug/m3): ");
//    Serial.println(data.PM_AE_UG_10_0);
//  }
//  else
//  {
//    Serial.println("No data.");
//  }
  
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedDate();
  Serial.println(formattedDate);

  // Extract date
  int splitGap = formattedDate.indexOf(" ");
  dayStamp = formattedDate.substring(0, splitGap);
  Serial.print("Date: ");
  Serial.println(dayStamp);
  
  //Extract time
  timeStamp = formattedDate.substring(splitGap+1, formattedDate.length());
  Serial.print("Time: ");
  Serial.println(timeStamp);

  // RANDOM NUMBER FOR DUMMY DATA  
  // print a random number from 80 to 109
  int randNumber1 = random(80, 110);
  Serial.println(randNumber1);
  // print a random number from 60 to 89
  int randNumber25 = random(60, 90);
  Serial.println(randNumber25);
  // print a random number from 30 to 49
  int randNumber100 = random(30, 50);
  Serial.println(randNumber100);

  //Form data packet before send to firebase
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  
//  root["time"] = formattedDate;
//  root["PM10"] = data.PM_AE_UG_1_0;
//  root["PM25"] = data.PM_AE_UG_2_5;
//  root["PM100"] = data.PM_AE_UG_10_0;

    root["time"] = formattedDate;
    root["PM10"] = randNumber1;
    root["PM25"] = randNumber25;
    root["PM100"] = randNumber100;

  Firebase.push("Sensor", root); //In this case child name in firebase is Sensor
  digitalWrite(2, HIGH); // Turn the LED Blue on meaning push success
  delay(1000);
  
  // handle error
  if (Firebase.failed()) {
      Serial.print("pushing to firebase failed:");
      Serial.println(Firebase.error());  
      return;
  }
  Serial.print("pushed to firebase!");
  digitalWrite(2, LOW);

  // if use deep sleep, the microcontroller will re-connect wifi every time that wakeup
//  esp_sleep_enable_timer_wakeup(30000000);
//  esp_deep_sleep_start();
   delay(10000);
}
