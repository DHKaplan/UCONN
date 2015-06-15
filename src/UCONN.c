#include "pebble.h"

#define DATE_STYLE 0
#define BT_VIBRATE 1  

Window *window;

static GBitmap     *image;
static BitmapLayer *image_layer;

TextLayer *text_degrees_layer;
TextLayer *text_dayname_layer;
TextLayer *text_mmdd_layer;
TextLayer *text_year_layer;
TextLayer *text_time_layer;
TextLayer *text_battery_layer;
TextLayer *text_location_layer;

Layer       *RedLineLayer;
Layer       *BatteryLineLayer;

Layer       *BTLayer;

GFont        fontHelvNewLight20;
GFont		     fontRobotoCondensed21; 
GFont		     fontRobotoCondensed25; 	
GFont        fontSystemGothicBold28;
GFont        fontRobotoBoldSubset49;

static int  batterychargepct;
static int batterycharging = 0;
static int  BatteryVibesDone = 0;

static int FirstTime = 0;

static char date_type[]="us  ";

static char dayname_text[] = "XXX";
static char time_text[] = "00:00";
static char mmdd_text[] = "XXXX00";
static char year_text[] = "0000";
static char seconds_text[]="00";
 
static char date_format[]="%b %d";

GPoint     Linepoint;

static int BTConnected = 1;
static int BTVibesDone = 0;
static char VibOnBTLoss[] = "0"; //From Config Page


GRect        bluetooth_rect;

GColor TextColorHold;
GColor BGColorHold;


void red_line_layer_update_callback(Layer *RedLineLayer, GContext* ctx) { 
         
     #if PBL_COLOR
         graphics_context_set_fill_color(ctx, GColorRed);
     #else 
         //B&W
         graphics_context_set_fill_color(ctx, TextColorHold);
     #endif
       
     graphics_fill_rect(ctx, layer_get_bounds(RedLineLayer), 0, GCornerNone);
}

void battery_line_layer_update_callback(Layer *BatteryLineLayer, GContext* ctx) { 
     graphics_context_set_fill_color(ctx, TextColorHold);
     graphics_fill_rect(ctx, layer_get_bounds(BatteryLineLayer), 3, GCornersAll);

     if (batterycharging == 1) {
       #ifdef PBL_COLOR
          graphics_context_set_fill_color(ctx, GColorBlue);
       #else
          graphics_context_set_fill_color(ctx, GColorBlack);
       #endif

       graphics_fill_rect(ctx, GRect(2, 1, 100, 4), 3, GCornersAll);

     } else if (batterychargepct > 20) {
       #ifdef PBL_COLOR
          graphics_context_set_fill_color(ctx, GColorGreen);
       #else
          graphics_context_set_fill_color(ctx, GColorBlack);
       #endif
         
       graphics_fill_rect(ctx, GRect(2, 1, batterychargepct, 4), 3, GCornersAll);
       
     } else {
      #ifdef PBL_COLOR
          graphics_context_set_fill_color(ctx, GColorRed);
       #else
          graphics_context_set_fill_color(ctx, GColorBlack);
       #endif
         
       graphics_fill_rect(ctx, GRect(2, 1, batterychargepct, 4),3, GCornersAll);
     }
}

void handle_bluetooth(bool connected) {
  if (connected) {
         BTConnected = 1;     // Connected
         BTVibesDone = 0;

    } else {
         BTConnected = 0;      // Not Connected

         if ((BTVibesDone == 0) && (strcmp(VibOnBTLoss,"0") == 0)) {    
             BTVibesDone = 1;
             vibes_long_pulse();
         }
    }
    layer_mark_dirty(BTLayer);
}

//BT Logo Callback;
void BTLine_update_callback(Layer *BTLayer, GContext* BT1ctx) {

       GPoint BTLinePointStart;
       GPoint BTLinePointEnd;
  
      graphics_context_set_stroke_color(BT1ctx, TextColorHold);
      
      if (BTConnected == 0) {
        
        #ifdef PBL_COLOR 
            graphics_context_set_stroke_color(BT1ctx, GColorRed);
            graphics_context_set_fill_color(BT1ctx, GColorWhite);
            graphics_fill_rect(BT1ctx, layer_get_bounds(BTLayer), 0, GCornerNone);
        #else
            graphics_context_set_stroke_color(BT1ctx, GColorBlack);
            graphics_context_set_fill_color(BT1ctx, GColorWhite);
            graphics_fill_rect(BT1ctx, layer_get_bounds(BTLayer), 0, GCornerNone);

        #endif 
      
        // "X"" Line 1
          BTLinePointStart.x = 1;
          BTLinePointStart.y = 1;
  
          BTLinePointEnd.x = 20;
          BTLinePointEnd.y = 20;
          graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);   
          
         // "X"" Line 2
          BTLinePointStart.x = 1;
          BTLinePointStart.y =20;
  
          BTLinePointEnd.x = 20;
          BTLinePointEnd.y = 1;
          graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);
         
      } else {   

       //Line 1
       BTLinePointStart.x = 10;
       BTLinePointStart.y = 1;
  
       BTLinePointEnd.x = 10;
       BTLinePointEnd.y = 20;
       graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);
         
       //Line 1a
       BTLinePointStart.x = 11;
       BTLinePointStart.y = 1;
  
       BTLinePointEnd.x = 11;
       BTLinePointEnd.y = 20;
       graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);
   
       //Line 2
       BTLinePointStart.x = 10;
       BTLinePointStart.y = 1;
  
       BTLinePointEnd.x = 17;
       BTLinePointEnd.y = 6;
       graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);
      
       //Line 2a
       BTLinePointStart.x = 11;
       BTLinePointStart.y = 1;
  
       BTLinePointEnd.x = 17;
       BTLinePointEnd.y = 5;
       graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);
 
       //Line 3
       BTLinePointStart.x = 4;
       BTLinePointStart.y = 5;
  
       BTLinePointEnd.x = 17;
       BTLinePointEnd.y = 15;
       graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);
  
       //Line 3a
       BTLinePointStart.x = 4;
       BTLinePointStart.y = 6;
  
       BTLinePointEnd.x = 17;
       BTLinePointEnd.y = 16;
       graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);

       //Line 4
       BTLinePointStart.x = 4;
       BTLinePointStart.y = 15;
  
       BTLinePointEnd.x = 17;
       BTLinePointEnd.y = 5;
       graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);
  
       //Line 4a
       BTLinePointStart.x = 4;
       BTLinePointStart.y = 16;
  
       BTLinePointEnd.x = 17;
       BTLinePointEnd.y = 6;
       graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);
  
       //Line 5
       BTLinePointStart.x = 10;
       BTLinePointStart.y = 20;
  
       BTLinePointEnd.x = 17;
       BTLinePointEnd.y = 15;
       graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);
  
       //Line 5a
       BTLinePointStart.x = 11;
       BTLinePointStart.y = 20;
  
       BTLinePointEnd.x = 17;
       BTLinePointEnd.y = 16;
       graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);   
      }            
}    

void handle_battery(BatteryChargeState charge_state) {
    static char BatteryPctTxt[] = "+100%";

  batterychargepct = charge_state.charge_percent;

  if (charge_state.is_charging) {
    batterycharging = 1;
  } else {
    batterycharging = 0;
  }

  // Reset if Battery > 20% ********************************
  if (batterychargepct > 20) {
     if (BatteryVibesDone == 1) {     //OK Reset to normal
         BatteryVibesDone = 0;
     }
  }

  //
  if (batterychargepct < 30) {
     if (BatteryVibesDone == 0) {            // Do Once
         BatteryVibesDone = 1;
         vibes_long_pulse();
      }
  }

   if (charge_state.is_charging) {
     strcpy(BatteryPctTxt, "Chrg");
  } else {
     snprintf(BatteryPctTxt, 5, "%d%%", charge_state.charge_percent);
  }
   text_layer_set_text(text_battery_layer, BatteryPctTxt);

  layer_mark_dirty(BatteryLineLayer);
}


void handle_appfocus(bool in_focus){
    if (in_focus) {
        handle_bluetooth(bluetooth_connection_service_peek());
    }
}

void handle_tick(struct tm *tick_time, TimeUnits units_changed) {  
  
  char time_format[] = "%I:%M";
  
  strftime(seconds_text, sizeof(seconds_text), "%S", tick_time);
  
  if (clock_is_24h_style()) {
       strcpy(time_format,"%R");
     } else {
       strcpy(time_format,"%I:%M");
     }

  strftime(time_text, sizeof(time_text), time_format, tick_time); 
 
  // Kludge to handle lack of non-padded hour format string
  // for twelve hour clock.
  if (!clock_is_24h_style() && (time_text[0] == '0')) {
    memmove(time_text, &time_text[1], sizeof(time_text) - 1);
  }
  if((strcmp(seconds_text,"00") == 0) || (FirstTime == 0)) {
      //Set Day and Date
      strftime(dayname_text, sizeof(dayname_text), "%a",    tick_time);
      strftime(mmdd_text,    sizeof(mmdd_text),    date_format, tick_time); 
      strftime(year_text,    sizeof(year_text),    "%Y",    tick_time); 
   
      text_layer_set_text(text_dayname_layer, dayname_text);
      text_layer_set_text(text_mmdd_layer, mmdd_text);
      text_layer_set_text(text_year_layer, year_text);    
  }
  
if (units_changed & DAY_UNIT) {
   // Only update the day name & date when it's changed. 
    text_layer_set_text(text_dayname_layer, dayname_text);
    text_layer_set_text(text_mmdd_layer, mmdd_text);
    text_layer_set_text(text_year_layer, year_text);    
  }
  
 // Always update time of day
  text_layer_set_text(text_time_layer, time_text);
   FirstTime = 1;
}

void handle_deinit(void) {
  tick_timer_service_unsubscribe();
    
  persist_write_string(DATE_STYLE, date_type);
  persist_write_string(BT_VIBRATE, VibOnBTLoss);

  battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
  app_focus_service_unsubscribe();
  
  text_layer_destroy(text_time_layer);
  text_layer_destroy(text_year_layer);
  text_layer_destroy(text_mmdd_layer);
  text_layer_destroy(text_dayname_layer);
  text_layer_destroy(text_battery_layer);

  layer_destroy(RedLineLayer);
  layer_destroy(BatteryLineLayer);
  layer_destroy(BTLayer);
 
  gbitmap_destroy(image);
  bitmap_layer_destroy(image_layer);
  
  fonts_unload_custom_font(fontHelvNewLight20);
  fonts_unload_custom_font(fontRobotoCondensed25);
  fonts_unload_custom_font(fontRobotoBoldSubset49); 
  
  window_destroy(window);
  
}
//Receive Input from Config html to js file
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  char BTVibeConfig[] = "0";
  
  APP_LOG(APP_LOG_LEVEL_INFO, "In Inbox received callback");

  FirstTime = 0;

  // Read first item
  Tuple *t = dict_read_first(iterator);

  while(t != NULL) {

    // Which key was received?
    switch(t->key) {
    case 0:
      strcpy(date_type, t->value->cstring);

      if (strcmp(date_type, "us") == 0) {
         strcpy(date_format, "%b %d");
      } else {
         strcpy(date_format, "%d %b");
      }

      text_layer_set_text(text_mmdd_layer, mmdd_text);
      break;
     
      case 1:
      strcpy(BTVibeConfig, t->value->cstring); 
      if (strcmp(BTVibeConfig, "0") == 0) {
         strcpy(VibOnBTLoss,"0");
      } else {
         strcpy(VibOnBTLoss,"1");
      }
      break;
    default:
      APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }

    // Look for next item
    t = dict_read_next(iterator);
    }
  }

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Inbox Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

void handle_init(void) { 
  GColor BGCOLOR   = COLOR_FALLBACK(GColorDukeBlue, GColorBlack);
  BGColorHold = BGCOLOR;
  
  GColor TEXTCOLOR = COLOR_FALLBACK(GColorWhite, GColorWhite);
  TextColorHold = TEXTCOLOR;

  window = window_create();
  window_stack_push(window, true /* Animated */);
  window_set_background_color(window, BGCOLOR);

  Layer *window_layer = window_get_root_layer(window);
   
  fontHelvNewLight20     = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_HELV_NEW_LIGHT_20));
  fontRobotoCondensed21  = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21));
  fontRobotoCondensed25  = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_25));
  fontSystemGothicBold28 = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
  fontRobotoBoldSubset49 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_49));
 
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  // year
  text_year_layer = text_layer_create(GRect(56, 86, 88, 82));  //was 62, 86, 82, 82   144wx168tall
  text_layer_set_text_color(text_year_layer, TEXTCOLOR);
  text_layer_set_background_color(text_year_layer, BGCOLOR);
  text_layer_set_font(text_year_layer, fontRobotoCondensed25);
  text_layer_set_text_alignment(text_year_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_year_layer));
  
  // mmdd
  text_mmdd_layer = text_layer_create(GRect(56, 62, 88, 28));
  text_layer_set_text_color(text_mmdd_layer, TEXTCOLOR);
  text_layer_set_background_color(text_mmdd_layer, BGCOLOR);
  text_layer_set_font(text_mmdd_layer, fontRobotoCondensed25);
  text_layer_set_text_alignment(text_mmdd_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_mmdd_layer));
  
   // Dayname
  text_dayname_layer = text_layer_create(GRect(56, 36, 88, 28));
  text_layer_set_text_color(text_dayname_layer, TEXTCOLOR);
  text_layer_set_background_color(text_dayname_layer, BGCOLOR);
  text_layer_set_font(text_dayname_layer, fontSystemGothicBold28);
  text_layer_set_text_alignment(text_dayname_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_dayname_layer)); 
  
// The bitmap layer holds the image for logo display
  image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_UCONN_COLOR_LOGO);
  image_layer = bitmap_layer_create(GRect(1, 40, 60, 77));
  bitmap_layer_set_bitmap(image_layer, image);
  bitmap_layer_set_background_color(image_layer, BGCOLOR);
  bitmap_layer_set_alignment(image_layer, GAlignCenter);
  layer_add_child(window_layer, bitmap_layer_get_layer(image_layer));
  
    //Persistent Value Date Format:
  if (persist_exists(DATE_STYLE)) {
       persist_read_string(DATE_STYLE, date_type, sizeof(date_type));
  }  else {
       strcpy(date_type, "us");
  }

  if (strcmp(date_type, "us") == 0) {
      strcpy(date_format, "%b %d");
  } else {
      strcpy(date_format, "%d %b");
  }
 
  //Persistent Value VibOnBTLoss
  if(persist_exists(BT_VIBRATE)) {
     persist_read_string(BT_VIBRATE, VibOnBTLoss, sizeof(VibOnBTLoss));  
  }  else {
     strcpy(VibOnBTLoss, "0"); // Default
  } 
  
  // Time of Day is here
  text_time_layer = text_layer_create(GRect(1, 115, 144, 53));
  text_layer_set_text_color(text_time_layer, TEXTCOLOR);
  text_layer_set_background_color(text_time_layer, BGCOLOR);
  text_layer_set_font(text_time_layer, fontRobotoBoldSubset49);
  text_layer_set_text_alignment(text_time_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_time_layer));

  // Red Line
  GRect line_frame = GRect(10, 33, 124, 2);
  RedLineLayer = layer_create(line_frame);
  layer_set_update_proc(RedLineLayer, red_line_layer_update_callback);
  layer_add_child(window_layer, RedLineLayer);
 
  // Battery Line
  GRect battery_line_frame = GRect(22, 117, 104, 6);
  BatteryLineLayer = layer_create(battery_line_frame);
  layer_set_update_proc(BatteryLineLayer, battery_line_layer_update_callback);
  layer_add_child(window_layer, BatteryLineLayer);
    
  tick_timer_service_subscribe(SECOND_UNIT, handle_tick);

    //Bluetooth Logo Setup area
    GRect BTArea = GRect(1, 5, 20, 20);
    BTLayer = layer_create(BTArea);

    layer_add_child(window_layer, BTLayer); 
    
    layer_set_update_proc(BTLayer, BTLine_update_callback);  
  
    bluetooth_connection_service_subscribe(&handle_bluetooth);
  
    handle_bluetooth(bluetooth_connection_service_peek());
         
    text_battery_layer = text_layer_create(GRect(85,2,55,30));
    text_layer_set_text_color(text_battery_layer, TEXTCOLOR);
    text_layer_set_background_color(text_battery_layer, BGCOLOR);
    text_layer_set_font(text_battery_layer,  fontHelvNewLight20);
    text_layer_set_text_alignment(text_battery_layer, GTextAlignmentRight);

    layer_add_child(window_layer, text_layer_get_layer(text_battery_layer));

    battery_state_service_subscribe(&handle_battery);
        
    app_focus_service_subscribe(&handle_appfocus);

    handle_battery(battery_state_service_peek());
  
}


int main(void) {
   handle_init();

   app_event_loop();
   
   handle_deinit();
}

