/**
 * Main
  * Este código es propiedad exclusiva de la comunidad de amigos de V2C y está destinado estrictamente para pasar un rato divertido programando

       .---.              .:-==-:.          .-==+===-:             
        #@@#           .+%@@@@@@@@#-     :+%@@@@@@@@@%*-          
        .%@@+         .#@@*-:..:=%@@=  .*@@@*=     -*%@@*         
         :@@@-       =+.-:       =@@# .%@@*.   º   º  .==          
          -@@@:     :@@%.       .#@@+ *@@*                
           +@@%.   .%@@-       :#@@#..@@@:   0   0   0                  <3
            *@@#   #@@+      .+@@%+   %@@-           
            .%@@+ *@@*     :*@@@+.    +@@#.    0   0       
             :%@@*@@#.  .=#@@%+.       *@@%           -#%=         
              -@@@@%: .+%@@@%*+++++++++.-#@@@#      %@@%+         
               =@@@-  :@@@@@@@@@@@@@@@@%+.:+#@@@@@@@%*-. 
                                  
*/
#include "FS.h"
#include <LittleFS.h>

// A library for interfacing with the touch screen
#include <TFT_eSPI.h>

// Include the PNG decoder library
#include <PNGdec.h>

// A library for interfacing with the touch screen
#include <XPT2046_Touchscreen.h>

#include <SPI.h>
#include <WiFi.h>
#include <WiFiMulti.h>

#define SSID_WIFI "JaimeTeDaWiFi"
#define PASS_WIFI "JaimeTeQuiere16"

#define TRYDAN_IP "192.168.68.107"
#define REFRESH_SAMPLES 10 * 1000

WiFiMulti WiFiMulti;


void setup() {
  Serial.begin(115200);
  initTouch();//Start touch
  initTFTDisplay();//Start the tft display 
  // We start by connecting to a WiFi network
  WiFiMulti.addAP(SSID_WIFI,PASS_WIFI);

  while((WiFiMulti.run() != WL_CONNECTED)&&(millis()<10000)) {
      Serial.print(".");
      delay(500);
  }
  
  xTaskCreate(manageSamples,"take samples", 2048, NULL, 1, NULL);    
}


void loop() {
  manageTouchTrack();
  manageTFTDisplay(); 
}
