/*
        Inversion by Jack Carey
    
    I will need to use images instead of fonts,
    as the Pebble cannot handle the correct font
    sizes that I want to use.
    
    Images done in GIMP. 
    
    Lots of credit to Pebble's
    "Big Time" watch face, where I got lots of
    logic/help from.
*/

#include <pebble.h>

#define MINUTES 60
  
static Window *s_main_window;

static InverterLayer *s_invert_blocks[MINUTES];

//static TextLayer *s_time_layer;
//static GFont s_time_font;

static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static int hour_to_display = 0;
static int prev_hour = 0;
static bool first_update = true;
static bool first_update_2 = true;
static int mins_from_hour = 0;
static int curr_min_blocks = 0;
static int test_run_through = 0;


static int get_left_origin(int number){

  int mod_num = number%5;

  int return_num = 0;
  
  if(mod_num == 0){
    return_num = 2;
  }
  else if(mod_num == 1){
    return_num = 30;
  }
  else if(mod_num == 2){
    return_num = 58;
  }
  else if(mod_num == 3){
    return_num = 86;
  }
  else if(mod_num == 4){
    return_num = 114;
  }

  return return_num;

}

static int get_top_origin(int number){

  int return_num = 0;
  
  if(number < 5){
    return_num = 154;
  }
  else if(number >= 5 && number < 10){
    return_num = 140;
  }
  else if(number >= 10 && number < 15){
    return_num = 126;
  }
  else if(number >= 15 && number < 20){
    return_num = 112;
  }
  else if(number >= 20 && number < 25){
    return_num = 98;
  }
  else if(number >= 25 && number < 30){
    return_num = 84;
  }
  else if(number >= 30 && number < 35){
    return_num = 70;
  }
  else if(number >= 35 && number < 40){
    return_num = 56;
  }
  else if(number >= 40 && number < 45){
    return_num = 42;
  }
  else if(number >= 45 && number < 50){
    return_num = 28;
  }
  else if(number >= 50 && number < 55){
    return_num = 14;
  }
  else if(number >= 55 && number < 60){
    return_num = 0;
  }

return return_num;

}



static int get_display_img(unsigned/*int*/ hour){
  if(hour > 12){
    hour -= 12;
  }
  else if(hour == 0){
    hour = 12;
  }
  return hour;
}





static void update_hour(struct tm *tick_time) {
  

  //Hour first
  hour_to_display = get_display_img(tick_time->tm_hour);
  if(hour_to_display != prev_hour){
    
    switch(hour_to_display){
    case 1:
      s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NUM_1);
      break;
    case 2:
      s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NUM_2);
      break;
    case 3:
      s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NUM_3);
      break;
    case 4:
      s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NUM_4);
      break;
    case 5:
      s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NUM_5);
      break;
    case 6:
      s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NUM_6);
      break;
    case 7:
      s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NUM_7);
      break;
    case 8:
      s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NUM_8);
      break;
    case 9:
      s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NUM_9);
      break;
    case 10:
      s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NUM_10);
      break;
    case 11:
      s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NUM_11);
      break;
    case 12:
      s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NUM_12);
      break;
    default:
      s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ERROR);
      break;
    }

    bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  }

  prev_hour = hour_to_display;
  hour_to_display = 0;

  //Now Minutes: TODO: create layers every hour or 1 every minute? Convert multiple layers (blocks) into one - reduce memory consumption
  //TODO ALSO: make EDGE blocks slightly longer to fill gaps? IF I don't use lines between the blocks

  mins_from_hour = tick_time->tm_min;
  
  if(first_update == true && first_update_2 == true){

    for(int i = 0; i < mins_from_hour; i++){

      s_invert_blocks[i] = inverter_layer_create(GRect(get_left_origin(i), get_top_origin(i), 27, 13));
      layer_add_child(bitmap_layer_get_layer(s_background_layer), inverter_layer_get_layer(s_invert_blocks[i]));

      curr_min_blocks++;
    }
    
  }
    
    test_run_through++;
    if(test_run_through == 2){
      first_update_2 = false;
    }
  
  else if(first_update == false && first_update_2 == false){
    s_invert_blocks[mins_from_hour] = inverter_layer_create(GRect(get_left_origin(mins_from_hour-1), get_top_origin(mins_from_hour-1), 27, 13));
    layer_add_child(bitmap_layer_get_layer(s_background_layer), inverter_layer_get_layer(s_invert_blocks[mins_from_hour]));

    curr_min_blocks++;

    if(curr_min_blocks >= 58){
      for(int j = 0; j < 60; j++){
        inverter_layer_destroy(s_invert_blocks[j]);
      }
      curr_min_blocks = 0;
    }
  }

}





static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_hour(tick_time);
} 


static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);

  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  layer_add_child(window_get_root_layer(s_main_window), bitmap_layer_get_layer(s_background_layer));

  //Update time on startup
  time_t now = time(NULL);
  struct tm *tick_time = localtime(&now);
  update_hour(tick_time);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  first_update = false;
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);

  //Destroy Bitmap and bitmap layer
  gbitmap_destroy(s_background_bitmap);
  bitmap_layer_destroy(s_background_layer);
}




int main(void) {
  init();
  app_event_loop();
  deinit();
}






////////////////////////////////////////////////////////////////////////////////////
/*static void main_window_load(Window *window) {
  //Create GBitmap, then set to created BitmapLayer
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
  
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(5, 52, 139, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  
  //Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_48));

  //Apply to TextLayer
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  
  //Make sure the time is displayed from the start
  update_time();
}*/

/*static void main_window_unload(Window *window) {
  //Unload GFont
  fonts_unload_custom_font(s_time_font);
  
  //Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);

  //Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);
  
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
}*/

/////////////////////////////////////////////////////////////////////////////////////////////