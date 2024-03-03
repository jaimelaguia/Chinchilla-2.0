// Here are the callback functions that the decPNG library
// will use to open files, fetch data and close the file.

#define CAR_LABEL_X 100
#define CAR_LABEL_Y 212

#define HOUSE_LABEL_X 179
#define HOUSE_LABEL_Y 212

#define GRID_LABEL_X 26
#define GRID_LABEL_Y 128

#define BATTERY_LABEL_X 250
#define BATTERY_LABEL_Y 128

#define SUN_LABEL_X 137
#define SUN_LABEL_Y 44

#define REFRESH_SCALE 10

#define MAX_TFT_X 320
#define MAX_TFT_Y 240

#define MAX_TOUCH_X 3850
#define MAX_TOUCH_Y 3850

#define CAR_MENU 1
#define HOUSE_MENU 2
#define BATTERY_MENU 3
#define SUN_MENU 4
#define GRID_MENU 5

#define CUSTOM_DARK 0x18E3
#define CUSTOM_DARK_1 0x18C3

//Manage images files
#define FileSys LittleFS

extern int menu_id;

TFT_eSPI tft = TFT_eSPI();
PNG png;

TFT_eSprite car_label = TFT_eSprite(&tft);
TFT_eSprite house_label = TFT_eSprite(&tft);
TFT_eSprite grid_label = TFT_eSprite(&tft);
TFT_eSprite battery_label = TFT_eSprite(&tft);
TFT_eSprite sun_label = TFT_eSprite(&tft);

TFT_eSprite sun_to_house_point = TFT_eSprite(&tft);
TFT_eSprite sun_to_grid_point = TFT_eSprite(&tft);
TFT_eSprite sun_to_batt_point = TFT_eSprite(&tft);
TFT_eSprite grid_to_house_point = TFT_eSprite(&tft);
TFT_eSprite batt_to_house_point = TFT_eSprite(&tft);

TFT_eSprite chargebox_state_label = TFT_eSprite(&tft);

TFT_eSprite charging_time_label = TFT_eSprite(&tft);
TFT_eSprite charge_values_label = TFT_eSprite(&tft);

TFT_eSprite intensity_label = TFT_eSprite(&tft);

void initTFTDisplay(){
  // Initialise FS
  if (!FileSys.begin()) {
    Serial.println("LittleFS initialisation failed!");
    while (1) yield(); // Stay here twiddling thumbs waiting
  }

  // Start the tft display and set it to black
  tft.init();
  tft.setRotation(1); //Landscape Mode
  tft.invertDisplay(true);//Inverted default
  tft.fillScreen(TFT_BLACK);
  showPicture("/v2c_logo.png",0,0);
  std::string init_msg = "Conectando a " + std::string(SSID_WIFI) + "...";
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString(init_msg.c_str(), 80, 190);
  initFrameWork();
}


void manageTFTDisplay(){
  showLevelMenu(1,menu_id);
}
 

int animation_arc_x[9] = {1, 3, 6, 9, 11, 12, 13, 14, 15};
int animation_arc_y[9] = {0, 0, 2, 3, 4, 7, 10, 12, 15};

File pngfile;

void * pngOpen(const char *filename, int32_t *size) {
  //Serial.printf("Attempting to open %s\n", filename);
  pngfile = FileSys.open(filename, "r");
  *size = pngfile.size();
  return &pngfile;
}

void pngClose(void *handle) {
  File pngfile = *((File*)handle);
  if (pngfile) pngfile.close();
}

int32_t pngRead(PNGFILE *page, uint8_t *buffer, int32_t length) {
  if (!pngfile) return 0;
  page = page; // Avoid warning
  return pngfile.read(buffer, length);
}

int32_t pngSeek(PNGFILE *page, int32_t position) {
  if (!pngfile) return 0;
  page = page; // Avoid warning
  return pngfile.seek(position);
}

int x_img=0;
int y_img=0;
void showPicture(char *name_img,int x, int y)
{
  x_img = x;
  y_img = y;
  // Scan LittleFS and load any *.png files
  File file = LittleFS.open(name_img, "r");
  String strname = file.name();
  strname = "/" + strname;
  // Pass support callback function names to library
  int16_t rc = png.open(strname.c_str(), pngOpen, pngClose, pngRead, pngSeek, pngDraw);
  if (rc == PNG_SUCCESS) {
    tft.startWrite();
    uint32_t dt = millis();
    if (png.getWidth() > MAX_TFT_X) {
      Serial.println("Image too wide for allocated line buffer size!");
    }
    else {
      rc = png.decode(NULL, 0);
      png.close();
    }
    tft.endWrite();
  }

}

void pngDraw(PNGDRAW *pDraw) {
  uint16_t lineBuffer[MAX_TFT_X];
  png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xffffffff);
  //center orientation
  tft.pushImage(x_img, y_img + pDraw->y, pDraw->iWidth, 1, lineBuffer);
}

int step_menu = 0;
int current_menu = -1;
void showLevelMenu(int level, int id)
{
  bool refresh = (current_menu != id);

  if(current_menu != id){
    current_menu = id;
  }
  
  Serial.print("refresh: ");
  Serial.print(refresh);
   Serial.println(samples_refeshed);
  
  if(id > 0){
    menuWindowView(refresh);
  }else{
    dynamicView(refresh);
  }
  
  step_menu++;
  if (step_menu > 40) {
    step_menu = 0;
  }
}

int step_dynamic = 0;
void dynamicView(bool refresh)
{
  if(refresh){
    showPicture("/diagram.png",0,0);
    if(WiFiMulti.run() != WL_CONNECTED) {
      showPicture("/wifi_0.png",0,0);
    }else{
      tft.setTextSize(1);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.drawString(WiFi.localIP().toString().c_str(), 30, 10);
      if(WiFi.RSSI() > -70){
        showPicture("/wifi_3.png",0,0);
      }else if(WiFi.RSSI() > -90){
        showPicture("/wifi_2.png",0,0);
      }else{
        showPicture("/wifi_1.png",0,0);
      }
    }
  }

  if(connected_trydan){
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString(common_name.c_str(), 205, 10); 
  }else{
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawString("Error de medicion ", 205, 10); 
  }

  String car_str = String(charge_power) + " kW" ;
  String house_str = String(house_power) + " kW" ;
  String grid_str = String(grid_power) + " kW" ;
  String battery_str = String(battery_power) + " kW" ;
  String sun_str = String(sun_power) + " kW" ;

  int spr_width = car_label.textWidth("-22.22 kW"); // 7 is widest numeral in this font
  car_label.drawString(car_str.c_str(), spr_width / 2, car_label.fontHeight() / 2);
  car_label.pushSprite(CAR_LABEL_X, CAR_LABEL_Y);

  house_label.drawString(house_str.c_str(), spr_width / 2, house_label.fontHeight() / 2);
  house_label.pushSprite(HOUSE_LABEL_X, HOUSE_LABEL_Y);

  grid_label.drawString(grid_str.c_str(), spr_width / 2, grid_label.fontHeight() / 2);
  grid_label.pushSprite(GRID_LABEL_X, GRID_LABEL_Y);

  battery_label.drawString(battery_str.c_str(), spr_width / 2, battery_label.fontHeight() / 2);
  battery_label.pushSprite(BATTERY_LABEL_X, BATTERY_LABEL_Y);

  sun_label.drawString(sun_str.c_str(), spr_width / 2, sun_label.fontHeight() / 2);
  sun_label.pushSprite(SUN_LABEL_X, SUN_LABEL_Y);


  sun_to_house_point.fillSprite(TFT_BLACK);
  if ((sun_power > 0) && (house_power > 0)) {
    sun_to_house_point.drawLine(3, 0, 3, 93, TFT_YELLOW);
    sun_to_house_point.fillCircle(3, 93 * ((float)step_dynamic / 40.0), 3, TFT_YELLOW);
  }else{
    sun_to_house_point.drawLine(3, 0, 3, 93, CUSTOM_DARK);
  }
  sun_to_house_point.pushSprite(158, 73);

  sun_to_grid_point.fillSprite(TFT_BLACK);
  if ((sun_power > 0) && (grid_power < 0)) {
    sun_to_grid_point.drawArc(18, 18, 15, 15, 90, 180, TFT_YELLOW, TFT_BLACK);
    sun_to_grid_point.drawLine(18, 3, 78, 3, TFT_YELLOW);
    sun_to_grid_point.drawLine(3, 18, 3, 46, TFT_YELLOW);

    if (step_dynamic <= 21) {
      sun_to_grid_point.fillCircle(78 - (60 * ((float)step_dynamic / 21.0)), 3, 3, TFT_YELLOW);
    } else if (step_dynamic > 21 && step_dynamic <= 30) {
      sun_to_grid_point.fillCircle(18 - (animation_arc_x[step_dynamic - 22]), 3 + ((animation_arc_y[step_dynamic - 22])), 3, TFT_YELLOW);
    } else {
      sun_to_grid_point.fillCircle(3, 19 + (28 * ((float)(step_dynamic - 30) / 10.0)), 3, TFT_YELLOW);
    }
  }else{
    sun_to_grid_point.drawArc(18, 18, 15, 15, 90, 180, CUSTOM_DARK, TFT_BLACK);
    sun_to_grid_point.drawLine(18, 3, 78, 3, CUSTOM_DARK);
    sun_to_grid_point.drawLine(3, 18, 3, 46, CUSTOM_DARK);
  }
  sun_to_grid_point.pushSprite(46, 36);

  sun_to_batt_point.fillSprite(TFT_BLACK);
  if ((sun_power > 0) && (battery_power < 0)) {
    sun_to_batt_point.drawArc(60, 18, 15, 15, 180, 270, TFT_YELLOW, TFT_BLACK);
    sun_to_batt_point.drawLine(0, 3, 60, 3, TFT_YELLOW);
    sun_to_batt_point.drawLine(75, 18, 75, 46, TFT_YELLOW);

    if (step_dynamic <= 21) {
      sun_to_batt_point.fillCircle((60 * ((float)step_dynamic / 21.0)), 3, 3, TFT_YELLOW);
    } else if (step_dynamic > 21 && step_dynamic <= 30) {
      sun_to_batt_point.fillCircle(60 + (animation_arc_x[step_dynamic - 22]), 3 + ((animation_arc_y[step_dynamic - 22])), 3, TFT_YELLOW);
    } else {
      sun_to_batt_point.fillCircle(75, 19 + (28 * ((float)(step_dynamic - 30) / 10.0)), 3, TFT_YELLOW);
    }
  }else{
    sun_to_batt_point.drawArc(60, 18, 15, 15, 180, 270, CUSTOM_DARK, TFT_BLACK);
    sun_to_batt_point.drawLine(0, 3, 60, 3, CUSTOM_DARK);
    sun_to_batt_point.drawLine(75, 18, 75, 46, CUSTOM_DARK);
  }
  sun_to_batt_point.pushSprite(198, 36);

  grid_to_house_point.fillSprite(TFT_BLACK);
   if ((grid_power > 0)) {
    grid_to_house_point.drawArc(18, 29, 15, 15, 0, 90, TFT_WHITE, TFT_BLACK);
    grid_to_house_point.drawLine(3, 0, 3, 29, TFT_WHITE);
    grid_to_house_point.drawLine(18, 44, 35, 44, TFT_WHITE);
    
    if (step_dynamic <= 21) {
      grid_to_house_point.fillCircle(3, (29 * ((float)step_dynamic / 21.0)), 3, TFT_WHITE);
    } else if (step_dynamic > 21 && step_dynamic <= 30) {
      grid_to_house_point.fillCircle(3 + (animation_arc_y[step_dynamic - 22]), 29 + ((animation_arc_x[step_dynamic - 22])), 3, TFT_WHITE);
    } else {
      grid_to_house_point.fillCircle(18 + (17 * ((float)(step_dynamic - 30)) / 10.0), 44, 3, TFT_WHITE);
    }
  }else{
    grid_to_house_point.drawArc(18, 29, 15, 15, 0, 90, CUSTOM_DARK, TFT_BLACK);
    grid_to_house_point.drawLine(3, 0, 3, 29, CUSTOM_DARK);
    grid_to_house_point.drawLine(18, 44, 35, 44, CUSTOM_DARK);
  }
  grid_to_house_point.pushSprite(47, 158);

  batt_to_house_point.fillSprite(TFT_BLACK);
  if (battery_power > 0) { //BATT
    batt_to_house_point.drawArc(16, 30, 15, 15, 270, 0, TFT_GREEN, TFT_BLACK);
    batt_to_house_point.drawLine(31, 0, 31, 29, TFT_GREEN);
    batt_to_house_point.drawLine(16, 45, 0, 45, TFT_GREEN);
  if (step_dynamic <= 21) {
    batt_to_house_point.fillCircle(31, (29 * ((float)step_dynamic / 21.0)), 3, TFT_GREEN);
  } else if (step_dynamic > 21 && step_dynamic <= 30) {
    batt_to_house_point.fillCircle(31 - (animation_arc_y[step_dynamic - 22]), 29 + ((animation_arc_x[step_dynamic - 22])), 3, TFT_GREEN);
  } else {
    batt_to_house_point.fillCircle(16 - (16 * ((float)(step_dynamic - 30)) / 10.0), 44, 3, TFT_GREEN);
  }
  }else{
    batt_to_house_point.drawArc(16, 30, 15, 15, 270, 0, CUSTOM_DARK, TFT_BLACK);
    batt_to_house_point.drawLine(31, 0, 31, 29, CUSTOM_DARK);
    batt_to_house_point.drawLine(16, 45, 0, 45, CUSTOM_DARK);
  }
  batt_to_house_point.pushSprite(242, 158);
  
  step_dynamic++;
  if (step_dynamic > 40) {
    step_dynamic = 0;
  }
  delay(50);
}

int step_window = 0;
int intensity_select_mode = 1;
void menuWindowView(bool refresh)
{

  //Backgroud images from menu
  if(refresh){
    switch(current_menu){
      case CAR_MENU:
        showPicture("/car_menu.png",0,0);
        break;
      case HOUSE_MENU:
        showPicture("/house_menu.png",0,0);
        break;
      case BATTERY_MENU:
        showPicture("/batt_menu.png",0,0);
        break;
      case SUN_MENU:
        showPicture("/sun_menu.png",0,0);
        break;
      case GRID_MENU:
        showPicture("/grid_menu.png",0,0);
        break;
    }
  }

  //Images that need an update depending the samples values
  if(current_menu == 1){
    if(samples_refeshed){
      showPicture("/bar_clean.png",33,158);
      samples_refeshed = false;
    }

    String locked_str = "Deshabilitado";

    switch(charge_state){
      case 0:
      {
        locked_str = "Esperando VE";
        break;
      }
      case 1:
      {
        if(timer_on){
          locked_str = "Programacion ON";
        }else if(paused){
          locked_str = "Carga en pausa";
        }else{
          locked_str = "VE Conectado";
        }
        break;
      }
      case 2:
      {
        locked_str = "Cargando";
        break;
      }
    }
    
    if(!locked){
      showPicture("/unlocked.png",62,29); 
    }else{
      showPicture("/locked.png",62,29);  
    }

    if(!timer_on){
      showPicture("/timer_off.png",92,27); 
    }else{
      showPicture("/timer_on.png",92,27); 
    }

    chargebox_state_label.fillSprite(CUSTOM_DARK_1);
    chargebox_state_label.drawString(locked_str.c_str(),78, 10);
    chargebox_state_label.pushSprite(126, 29);

    int hours = charge_time / 3600;
    int minutes = (charge_time - hours)/60;

    String time_str = String(hours) + "h " + String(minutes) + "m";
    
    charging_time_label.fillSprite(CUSTOM_DARK_1);
    charging_time_label.drawString(time_str.c_str(),70, 10);
    charging_time_label.pushSprite(33, 80);

    String energy = String(charge_power) + " kW | " + String(charge_energy) + " kWh";

    charge_values_label.fillSprite(CUSTOM_DARK_1);
    charge_values_label.drawString(energy.c_str(),130, 10);
    charge_values_label.pushSprite(33, 110);

    int x_max_int = ((((float)max_intensity - 6.0)/26.0) * 258.0) + 33.0;
    int x_min_int = ((((float)min_intensity - 6.0)/26.0) * 258.0) + 33.0;
    int x_int = ((((float)intensity - 6.0)/26.0) * 258.0) + 33.0;

    if(((max_intensity - min_intensity) <= 2)){
      if(max_intensity == min_intensity){
        showPicture("/on_marker.png",(x_int - 8),160);
      }else if((max_intensity - min_intensity) == 1){
        if(intensity == max_intensity){
          showPicture("/on_marker_l.png",(x_int - 14),160);
        }else{
          showPicture("/on_marker_r.png",(x_int - 8),160);
        }
      }else{
        showPicture("/on_marker_lr.png",(x_int - 15),160);
      }
    }else if((max_intensity - intensity) <= 1){
      if((max_intensity - intensity) == 1){
          showPicture("/on_marker_r.png",(x_int - 8),160);
      }else{
          showPicture("/on_marker.png",(x_int - 8),160);
      }
      showPicture("/off_marker.png",(x_min_int - 5),160);//33-291
    }else if((intensity - min_intensity) <= 1){
      if((intensity - min_intensity) == 1){
          showPicture("/on_marker_l.png",(x_int - 14),160);
      }else{
          showPicture("/on_marker.png",(x_int - 8),160);
      }
      showPicture("/off_marker.png",(x_max_int - 5),160);//33-291
    }else{
      showPicture("/on_marker.png",(x_int - 8),160);//33-291
      showPicture("/off_marker.png",(x_max_int - 5),160);//33-291
      showPicture("/off_marker.png",(x_min_int - 5),160);//33-291
    }

    String int_str = intensity_select_mode == 1 ? "Intensidad ": (intensity_select_mode == 2 ? "Max. ": "Min.");
            int_str += intensity_select_mode == 1 ? intensity : (intensity_select_mode == 2 ? max_intensity : min_intensity);
            int_str += " A"; 

    intensity_label.fillSprite(CUSTOM_DARK_1);
    intensity_label.setTextColor(TFT_WHITE, CUSTOM_DARK);
    if(intensity_select_mode != 1) intensity_label.setTextColor(0x8410, CUSTOM_DARK);
    intensity_label.drawString(int_str.c_str(),90, 10);
    intensity_label.pushSprite(75, 190);
   
  }  

  step_window++;
  if (step_window > 40) {
    step_window = 0;
  }
}

void initFrameWork() {
  
  // Create a sprite of defined size
  car_label.setTextSize(1);
  int spr_width = car_label.textWidth("-22.22 kW"); // 7 is widest numeral in this font
  car_label.createSprite(spr_width, car_label.fontHeight());
  car_label.setTextColor(TFT_CYAN, CUSTOM_DARK);
  car_label.setTextDatum(MC_DATUM);
  car_label.setTextPadding(spr_width);

  house_label.setTextSize(1);
  house_label.createSprite(spr_width, house_label.fontHeight());
  house_label.setTextColor(TFT_CYAN, CUSTOM_DARK);
  house_label.setTextDatum(MC_DATUM);
  house_label.setTextPadding(spr_width);

  grid_label.setTextSize(1);
  grid_label.createSprite(spr_width, grid_label.fontHeight());
  grid_label.setTextColor(TFT_WHITE, CUSTOM_DARK);
  grid_label.setTextDatum(MC_DATUM);
  grid_label.setTextPadding(spr_width);

  battery_label.setTextSize(1);
  battery_label.createSprite(spr_width, battery_label.fontHeight());
  battery_label.setTextColor(TFT_GREEN, CUSTOM_DARK);
  battery_label.setTextDatum(MC_DATUM);
  battery_label.setTextPadding(spr_width);

  sun_label.setTextSize(1);
  sun_label.createSprite(spr_width, sun_label.fontHeight());
  sun_label.setTextColor(TFT_YELLOW, CUSTOM_DARK);
  sun_label.setTextDatum(MC_DATUM);
  sun_label.setTextPadding(spr_width);

  sun_to_house_point.createSprite(7, 93);

  sun_to_grid_point.createSprite(78, 46);

  sun_to_batt_point.createSprite(78, 46);

  grid_to_house_point.createSprite(35, 48);
  
  batt_to_house_point.createSprite(35, 48);

  chargebox_state_label.setTextSize(2);
  chargebox_state_label.createSprite(160, 18);
  chargebox_state_label.setTextColor(TFT_WHITE, CUSTOM_DARK);
  chargebox_state_label.setTextDatum(MC_DATUM);
  chargebox_state_label.setTextPadding(spr_width);

  intensity_label.setTextSize(2);
  intensity_label.createSprite(180, 18);
  intensity_label.setTextColor(TFT_WHITE, CUSTOM_DARK);
  intensity_label.setTextDatum(MC_DATUM);
  intensity_label.setTextPadding(60);

  charging_time_label.setTextSize(2);
  charging_time_label.createSprite(180, 18);
  charging_time_label.setTextColor(TFT_WHITE, CUSTOM_DARK);
  charging_time_label.setTextDatum(MC_DATUM);
  charging_time_label.setTextPadding(60);

  charge_values_label.setTextSize(2);
  charge_values_label.createSprite(250, 18);
  charge_values_label.setTextColor(TFT_WHITE, CUSTOM_DARK);
  charge_values_label.setTextDatum(MC_DATUM);
  charge_values_label.setTextPadding(60);
}
