#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <iostream>
#include <string>
#include <codecvt>

const int SEN_LDR = 33;
const int LED_WIFI_ON = 2;
const int LED_POLES = 15;

const int LDR_READ = 34;

const long WIFI_TIMEOUT_MS = 2000;
const long WIFI_RECOVER_TIME_MS = 2000;

// instances tasks
TaskHandle_t th_wf = NULL;
TaskHandle_t rl_wf = NULL;

void xTaskKeepWiFiAlive(void *p);
void xTaskRule(void *p);

void setup() {
    Serial.begin(115200);
    
    analogReadResolution(SEN_LDR);

    pinMode(LED_WIFI_ON, OUTPUT);
    pinMode(LED_POLES, OUTPUT);
    pinMode(LDR_READ, INPUT);

    xTaskCreate(xTaskKeepWiFiAlive,"wifi_Debug",(configMINIMAL_STACK_SIZE * 3), NULL, 2, &th_wf);
    xTaskCreate(xTaskRule,         "rule_Debug",(configMINIMAL_STACK_SIZE * 5) ,NULL, 1, &rl_wf);
}

void loop() { }

String getLDR() {
  int analogValue = analogRead(SEN_LDR);
  int a = (analogValue / 4095.0) * 100;
  String postData = "{\"SL\":"+ String(a)+"}";
  return postData;
}

// Implementations
void xTaskRule(void *p) {
  while(1)  {
    vTaskDelay(3000 / portTICK_PERIOD_MS);  

     if(WiFi.status() == WL_CONNECTED) {

        HTTPClient http;   
        http.begin("http://192.168.43.119:3000/");  
        http.addHeader("Content-Type", "application/json"); 

        String body = getLDR();
        Serial.print("Preparando Response: ");
        Serial.println(body);
        int httpResponseCode = http.POST(body);   

        if(httpResponseCode > 199 && httpResponseCode < 300){
          String response = http.getString();
          char power = (int) response[0];

          
          Serial.print("Power : ");
          Serial.print(power);
          Serial.println("");
          Serial.print("response : ");
          Serial.print(response);
          

          if((int) response[1] == 0) {}
          if((int) response[2] == 0) {}

          digitalWrite(LED_POLES, power);
        }else{
          Serial.print("Error on sending POST: ");
          Serial.println(httpResponseCode);
        }
        http.end();
        vTaskDelay(3000 / portTICK_PERIOD_MS); 
      } else {
        Serial.println("Error in WiFi connection");  
      }
   }
}

void xTaskKeepWiFiAlive(void *p) {
   while(1) {
     
     if (WiFi.status() == WL_CONNECTED) {
        digitalWrite(LED_WIFI_ON, HIGH);
        continue;
     }
     
     WiFi.mode(WIFI_STA);
     WiFi.disconnect();
     WiFi.begin("CLL2", "qwe123456");
     
     unsigned long startAttemptTime = millis();

     while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT_MS){
       digitalWrite(LED_WIFI_ON, LOW);
     }

     if(WiFi.status() != WL_CONNECTED) {    
        digitalWrite(LED_WIFI_ON, LOW);    
			  continue;
      }
   }
}
