#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <WiFi.h>
#include <iostream>

#define WIFI_NETWORK "--- your WiFi network name ---"
#define WIFI_PASSWORD "--- your WiFi password ---"

const long WIFI_TIMEOUT_MS = 10000; // 20 second WiFi connection timeout
const long WIFI_RECOVER_TIME_MS = 10000; // Wait 30 seconds after a failed connection attempt

// instances tasks
TaskHandle_t th_wf = NULL;

void xTaskKeepWiFiAlive(void *p);

void setup() {
    
    pinMode(13, OUTPUT);

    xTaskCreate(
      xTaskKeepWiFiAlive,
      "wifi_Debug",
      (configMINIMAL_STACK_SIZE * 5),
      NULL,
      1,
      &th_wf
    );

}

void loop() {
  // put your main code here, to run repeatedly:
}

// Implementations
void xTaskKeepWiFiAlive(void *p) {
   for(;;) {
     
     if (WiFi.status() == WL_CONNECTED)
     {
        digitalWrite(13, LOW);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        continue;
     }
     
     Serial.println("[WIFI] Connecting");
     WiFi.mode(WIFI_STA);
     WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);
     
     unsigned long startAttemptTime = millis();

     while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT_MS){}
     
     if(WiFi.status() != WL_CONNECTED){
        Serial.println("[WIFI] FAILED");

        digitalWrite(13, HIGH);
        vTaskDelay(WIFI_RECOVER_TIME_MS / portTICK_PERIOD_MS);
			  continue;
      }

      Serial.println("[WIFI] Connected: " + WiFi.localIP());
      digitalWrite(13, LOW);
   }
}