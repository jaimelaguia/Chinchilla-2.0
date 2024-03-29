#include <HTTPClient.h>
#include "ArduinoJson.h"


bool samples_refeshed = false;
bool stop_refresh_lcd = false;

float sun_power = 0.0;
float grid_power = 0.0;
float battery_power = 0.0;
float house_power = 0.0;

int charge_state = 0;
float charge_power = 0.0;
float charge_energy = 0.0;
int charge_time = 0;
int paused = 0;

bool locked = false;
bool timer_on = true;

int max_intensity = 32;
int min_intensity = 6;
int intensity = 19;

String common_name = "";
bool connected_trydan = false;

HTTPClient http;

void manageSamples( void * parameter ){
  while(1){   
    while(stop_refresh_lcd)delay(20);
    delay(2000);
    
    String serverPath = "http://" + String(TRYDAN_IP) + ":8080/RealTimeData.txt";
    
    // Your Domain name with URL path or IP address with path
    http.begin(serverPath.c_str());
  
    // Send HTTP GET request
    int httpResponseCode = http.GET();
    
    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      
      JsonDocument data_received; 
      DeserializationError error_json  = deserializeJson(data_received, payload.c_str());
    
      if(!error_json)
      { 
        common_name =  "Conectado a " + data_received["ID"].as<String>();
        locked =  data_received["Locked"];
        timer_on = data_received["Timer"];
        
        sun_power =  data_received["FVPower"];
        battery_power =  data_received["BatteryPower"];;
        house_power = data_received["HousePower"];
        grid_power = house_power - sun_power - battery_power;
        
        charge_state = data_received["ChargeState"];
        charge_power = data_received["ChargePower"];
        charge_energy = data_received["ChargeEnergy"];
        charge_time = data_received["ChargeTime"];
        paused = data_received["Paused"];

        max_intensity =  data_received["MaxIntensity"];
        min_intensity =  data_received["MinIntensity"];
        intensity =  data_received["Intensity"];

        connected_trydan =  true;
        samples_refeshed = true;
      }else{
        connected_trydan =  false;
      }

      Serial.println(payload);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
    
    delay(REFRESH_SAMPLES);
  }
  
  vTaskDelete( NULL );
}
