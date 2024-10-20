#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <WiFi.h>
#include <HTTPClient.h>

const int LED_WIFI_OFF = 2;
const long WIFI_TIMEOUT_MS = 2000;
const long WIFI_RECOVER_TIME_MS = 2000;

// instances tasks
TaskHandle_t th_wf = NULL;
TaskHandle_t rl_wf = NULL;

void xTaskKeepWiFiAlive(void *p);
void xTaskRule(void *p);

void setup() {
    Serial.begin(115200);

    pinMode(2, OUTPUT);
    pinMode(15, OUTPUT);
  
    xTaskCreate(xTaskKeepWiFiAlive,"wifi_Debug",(configMINIMAL_STACK_SIZE * 3), NULL, 2, &th_wf);
    xTaskCreate(xTaskRule,         "rule_Debug",(configMINIMAL_STACK_SIZE * 5) ,NULL, 1, &rl_wf);
}

void loop() { }

void xTaskRule(void *p) {
  while(1)  {
    digitalWrite(15, HIGH);
    vTaskDelay(3000 / portTICK_PERIOD_MS);  
     
     if(WiFi.status() == WL_CONNECTED) {
        HTTPClient http;   
        http.begin("http://192.168.43.119:3000/");  
        http.addHeader("Content-Type", "text/plain"); 
        
        int httpResponseCode = http.GET();   
        
        if(httpResponseCode > 199 && httpResponseCode < 300){
          digitalWrite(15, LOW);
          String response = http.getString();   
          Serial.println(httpResponseCode); 
          Serial.println(response);
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
        digitalWrite(LED_WIFI_OFF, HIGH);
        continue;
     }
     
     WiFi.mode(WIFI_STA);
     WiFi.disconnect();
     WiFi.begin("CLL2", "qwe123456");
     
     unsigned long startAttemptTime = millis();

     while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT_MS){
       digitalWrite(LED_WIFI_OFF, LOW);
     }

     if(WiFi.status() != WL_CONNECTED) {        
	continue;
      }
   }
}