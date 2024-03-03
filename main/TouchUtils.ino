// ----------------------------
// Touch Screen pins
// ----------------------------
#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33
/********************************/
#define MAX_TOUCH_X 3850
#define MAX_TOUCH_Y 3850

int menu_id = 0;

SPIClass mySpi = SPIClass(VSPI);
XPT2046_Touchscreen ts(XPT2046_CS, XPT2046_IRQ);

void initTouch() {
  // Start the SPI for the touch screen and init the TS library
  mySpi.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  ts.begin(mySpi);
  ts.setRotation(1);
}

int delay_press_detect = millis();
void manageTouchTrack(){
  if ((ts.tirqTouched() && ts.touched()) && ((millis() - delay_press_detect) >1000)){
    TS_Point p = transformTouchCoordinates(ts.getPoint());
    printTouchToSerial(p);
    if(manageMenuPulse(p)){
      delay_press_detect = millis();
    }
  }
}

void printTouchToSerial(TS_Point p) {
  Serial.print("Pressure = ");
  Serial.print(p.z);
  Serial.print(", x = ");
  Serial.print(p.x);
  Serial.print(", y = ");
  Serial.print(p.y);
  Serial.println();
}

bool manageMenuPulse(TS_Point p){
  bool pressed = false;
  if(menu_id == 0){ 
    if(((p.x > 90) && (p.x < 160)) &&
      ((p.y > 180) && (p.y < (230)))) 
    {
      Serial.print("Car Menu Button!");
      menu_id = CAR_MENU;
      pressed = true;
    }

    //House Button
    if (((p.x > 160) && (p.x < 250)) &&
      ((p.y > 180) && (p.y < (230)))) 
    {
      Serial.print("House Menu Button!");
      menu_id = HOUSE_MENU;
      pressed = true;
    }

    //Battery Button
    if (((p.x > 250) && (p.x < 310)) &&
      ((p.y > 100) && (p.y < (150)))) 
    {
      Serial.print("Battery Menu Button!");
      menu_id = BATTERY_MENU;
      pressed = true;
    }

    //Solar Button
    if (((p.x > 130) && (p.x < 190)) &&
      ((p.y > 20) && (p.y < (70)))) 
    {
      Serial.print("Sun Menu Button!");
      menu_id = SUN_MENU;
      pressed = true;
    }

    //Grid Button
    if (((p.x > 30) && (p.x < 80)) &&
      ((p.y > 100) && (p.y < (150)))) 
    {
      Serial.print("Grid Menu Button!");
      menu_id = GRID_MENU;
      pressed = true;
    }
  }else{
    if(((p.x > 20) && (p.x < 50)) &&
      ((p.y > 20) && (p.y < (50)))) 
    {
      Serial.print("Close Window Button!");
      menu_id = 0;
      pressed = true;
    }

    if(menu_id == 1){
      if (((p.x > 60) && (p.x < 90)) &&
        ((p.y > 50) && (p.y < 70))) 
      {
        locked = !locked;
        pressed = true;
      } 

      if (((p.x > 265) && (p.x < 295)) &&
        ((p.y > 190) && (p.y < 215))) 
      {
        intensity_select_mode++;
        pressed = true;
      } 

      if (((p.x > 40) && (p.x < 70)) &&
        ((p.y > 190) && (p.y < 215))) 
      {
        intensity_select_mode--;
        pressed = true;
      } 

      if (intensity_select_mode < 0) 
      {
        intensity_select_mode = 0;
        pressed = true;
      } 
      
      if (intensity_select_mode > 2) 
      {
        intensity_select_mode = 2;
        pressed = true;
      } 
     
      if (((p.x > 40) && (p.x < 290)) &&
        ((p.y > 160) && (p.y < 185))) 
      {
        int x_max_int = ((((float)max_intensity - 6.0)/26.0) * 258.0) + 33.0;
        int x_min_int = ((((float)min_intensity - 6.0)/26.0) * 258.0) + 33.0;
        int x_int = ((((float)intensity - 6.0)/26.0) * 258.0) + 33.0;
        bool on_marker = true;

        if(intensity_select_mode == 2){
          x_max_int = ((((float)32 - 6.0)/26.0) * 258.0) + 33.0;
          x_min_int = ((((float)intensity - 6.0)/26.0) * 258.0) + 33.0;
          x_int = ((((float)max_intensity - 6.0)/26.0) * 258.0) + 33.0;
          on_marker = false;
        }else if(intensity_select_mode == 0){
          x_max_int = ((((float)intensity - 6.0)/26.0) * 258.0) + 33.0;
          x_min_int = ((((float)6 - 6.0)/26.0) * 258.0) + 33.0;
          x_int = ((((float)min_intensity - 6.0)/26.0) * 258.0) + 33.0;
          on_marker = false;
        }

        while(ts.tirqTouched() && ts.touched()) {
          TS_Point p_1 = transformTouchCoordinates(ts.getPoint());
          
          if(x_int > p_1.x){ 
            x_int--;
          }else{
            x_int++;
          }   
    
          if(x_int <= x_min_int){
            x_int = x_min_int;
          }else if(x_int >= x_max_int){
            x_int = x_max_int;
          }

          if(on_marker){
            showPicture("/on_marker.png",(x_int - 8),160);
          }else{
            showPicture("/off_marker.png",(x_int - 5),160);
          }

          int new_val_str = ((((float)x_int - 33.0) / 258.0) * 26.0) + 6.5;  
          String int_str = intensity_select_mode == 1 ? "Intensidad ": (intensity_select_mode == 2 ? "Max. ": "Min.");
          int_str += new_val_str;
          int_str += " A";   
          intensity_label.setTextColor(TFT_WHITE, CUSTOM_DARK);
          if(intensity_select_mode != 1) intensity_label.setTextColor(0x8410, CUSTOM_DARK);
          intensity_label.drawString(int_str.c_str(),90, 10);
          intensity_label.pushSprite(75, 190);
        }

        int x_result = x_int;
        int new_x = ((((float)x_int - 33.0) / 258.0) * 26.0) + 6.5;      
        int x_calc_int = ((((float)new_x - 6.0)/26.0) * 258.0) + 33.0;

        while(x_int != x_calc_int) {
          if(x_int > x_calc_int){ 
            x_int--;
          }else{
            x_int++;
          }   
          if(on_marker){
            showPicture("/on_marker.png",(x_int - 8),160);
          }else{
            showPicture("/off_marker.png",(x_int - 5),160);
          }
        }
        
        if(intensity_select_mode == 0){
          min_intensity = ((((float)x_result - 33.0) / 258.0) * 26.0) + 6.5;  
        }else if(intensity_select_mode == 2){
          max_intensity = ((((float)x_result - 33.0) / 258.0) * 26.0) + 6.5;  
        }else{
          intensity = ((((float)x_result - 33.0) / 258.0) * 26.0) + 6.5;  
        } 
        pressed = true;
      } 
    }
  }
  return pressed;
}

TS_Point transformTouchCoordinates(TS_Point p) {
  TS_Point new_p;
  new_p.x = (float)p.x * ((float)MAX_TFT_X / (float)MAX_TOUCH_X);
  new_p.y = (float)p.y * ((float)MAX_TFT_Y / (float)MAX_TOUCH_Y);
  new_p.z = (float)p.z;
  return new_p;
}
