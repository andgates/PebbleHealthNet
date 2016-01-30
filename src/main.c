#include "pebble.h"

#define NUM_MENU_ICONS 6
#define NUM_FIRST_MENU_ITEMS 6


/****************************Global Variable Declarations*******************************************/
static Window *s_main_window,                              //Window where user rates their sleep
              *s_intro_window,                             //Intro Window
              *s_thanks_window;                            //Thank-you window

static TextLayer *s_sleep_text_layer,                      //Text that says how many hours user slept
                 *s_thanks_text_layer;                     //Thanks user for rating their sleep

static MenuLayer *s_menu_layer;                            //Menu where user rates their sleep
                                                           //(used in s_main_window)

static GBitmap *s_menu_icons[NUM_MENU_ICONS],              //Images corresponding to menu choices     
               *s_sleep_image;                             //Image corresponding to sleep image(not uploaded)

int rating = 0;                                            //Sleep rating that can be stored
/****************************************************************************************************/

/*****************************Number returning Functions*********************************************/
 static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
       return NUM_FIRST_MENU_ITEMS;
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}
/****************************************************************************************************/




/******************************************Pebble Round Shit********************************************/
#ifdef PBL_ROUND 
static int16_t get_cell_height_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) { 
  if (menu_layer_is_index_selected(menu_layer, cell_index)) {
    switch (cell_index->row) {
      case 0:
        return MENU_CELL_ROUND_FOCUSED_SHORT_CELL_HEIGHT;
        break;
      default:
        return MENU_CELL_ROUND_FOCUSED_TALL_CELL_HEIGHT;
    }
  } else {
    return MENU_CELL_ROUND_UNFOCUSED_SHORT_CELL_HEIGHT;
  }
}
#endif
/******************************************************************************************************/

/***************************************Click Recognizers**********************************************/
//Recognizes clicks select on s_intro_window
static void intro_select_handler(ClickRecognizerRef recognizer, void *context){
    window_stack_push(s_main_window, true);  //Displays main window on select push
}
//Recognizes when user hits back on s_menu_window
//IRONICALLY DOES NOT WORK ON S_MENU
static void back_handler(ClickRecognizerRef recognizer, void *context) {
  window_stack_pop_all(true);    //Click back at any time to exit app
}
//Config for clicking any button on s_intro_window
static void intro_click_config_provider(void *context){
  window_single_click_subscribe(BUTTON_ID_BACK, back_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, intro_select_handler);
}
//Config for clicking back on s_menu_window
//IRONICALLY DOES NOT WORK
static void back_click_config_provider(void *context){
  window_single_click_subscribe(BUTTON_ID_BACK, back_handler);
}
//Recognizers when user clicks anything on s_thanks_window
static void thanks_click_handler(ClickRecognizerRef recognizer, void *context) {
  window_stack_pop_all(true);    //After providing input, exits app
}
//Config for clicking anything on s_thanks_window
static void thanks_click_config_provider(void *context){
  window_single_click_subscribe(BUTTON_ID_BACK, thanks_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, thanks_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, thanks_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, thanks_click_handler);
  
}
/******************************************************************************************************/

/*************************************s_menu_window****************************************************/
//Displays "How Satisfied" as a header on s_menu_window
static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
        menu_cell_basic_header_draw(ctx, cell_layer, "How Satisfied?");
}

//Draws rows of s_menu_window
static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {

  // Use the row to specify which item we'll draw
  switch(cell_index->row){
    case 0:
      //5 - Very Happy
      menu_cell_basic_draw(ctx, cell_layer, "5", "Very Happy",  s_menu_icons[0]);
      break;
    case 1:
      //4 - Happy
      menu_cell_basic_draw(ctx, cell_layer, "4", "Happy", s_menu_icons[1]);
      break;
    case 2:
      //3 - No Difference
      menu_cell_basic_draw(ctx, cell_layer, "3", "No Difference", s_menu_icons[2]);
      break;
    case 3:
      //2 - Unhappy
      menu_cell_basic_draw(ctx, cell_layer, "2", "Unhappy",  s_menu_icons[3]);
      break;
    case 4:
      //1 - Very Unhappy
      menu_cell_basic_draw(ctx, cell_layer, "1", "Very Unhappy", s_menu_icons[4]);
      break;
    case 5:
      //NA - Not Applicable
      menu_cell_basic_draw(ctx, cell_layer, "N/A", NULL, s_menu_icons[5]);
      break;
  }
}

//Stores users rating in global variable "rating"
static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  // Specifies how the user rated their sleep
  switch (cell_index->row) {
    case 0:
      rating = 5;
      break;
    case 1:
      rating = 4;
      break;
    case 2:
      rating = 3;
      break;
    case 3:
      rating = 2;
      break;
    case 4:
      rating = 1;
      break;
    case 5:
      rating = -1;
      break;
  }
  
  //Calls thank you window
  window_stack_push(s_thanks_window, false);
  
}
//Loads main_window
static void main_window_load(Window *window) {
  // Here we load the bitmap assets
  s_menu_icons[0] = gbitmap_create_with_resource(RESOURCE_ID_VERYGOOD);
  s_menu_icons[1] = gbitmap_create_with_resource(RESOURCE_ID_GOOD);
  s_menu_icons[2] = gbitmap_create_with_resource(RESOURCE_ID_MED);
  s_menu_icons[3] = gbitmap_create_with_resource(RESOURCE_ID_BAD);
  s_menu_icons[4] = gbitmap_create_with_resource(RESOURCE_ID_VERYBAD);
  s_menu_icons[5] = gbitmap_create_with_resource(RESOURCE_ID_EXAMPLE1);


  // Now we prepare to initialize the menu layer
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  
  //if user presses BACK, the app exits
  window_set_click_config_provider(window, back_click_config_provider);

  // Create the menu layer
  s_menu_layer = menu_layer_create(bounds);
  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = PBL_IF_RECT_ELSE(menu_get_header_height_callback, NULL),
    .draw_header = PBL_IF_RECT_ELSE(menu_draw_header_callback, NULL),
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
    .get_cell_height = PBL_IF_ROUND_ELSE(get_cell_height_callback, NULL),
  });

  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(s_menu_layer, window);

  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
}

//Unloads main window
static void main_window_unload(Window *window) {
  // Destroy the menu layer
  menu_layer_destroy(s_menu_layer);

  // Cleanup the menu icons
  for (int i = 0; i < NUM_MENU_ICONS; i++) {
    gbitmap_destroy(s_menu_icons[i]);
  }
}
/******************************************************************************************************/

/***************************************Intro Window***************************************************/
//Need to pass in X, where X is hours slept
//IDEA: Use a global variable

//Loads intro window
static void intro_window_load(Window *window) {
  // Now we prepare to initialize the intro
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer); 
  
  //if user presses BACK, the app exits
  window_set_click_config_provider(window, intro_click_config_provider);
  
  //Converts hours slept into a string
  static char hoursSlept[32];  
  snprintf(hoursSlept, sizeof(hoursSlept), "You slept for %d hours", 0);
  
  //Sets size of sleep text
  s_sleep_text_layer = text_layer_create(GRect(0, 72, bounds.size.w, 20));
  //adds text to sleep_text
  text_layer_set_text(s_sleep_text_layer, hoursSlept);
  //sets background color to cyan
  text_layer_set_background_color(s_sleep_text_layer, GColorCyan);
  //sets font color to yellow
  text_layer_set_text_color(s_sleep_text_layer, GColorYellow);
  //centers text
  text_layer_set_text_alignment(s_sleep_text_layer, GTextAlignmentCenter);
  //don't know what this does, think we need it though
  layer_add_child(window_layer, text_layer_get_layer(s_sleep_text_layer));
  
}

//Unloads intro_window
static void intro_window_unload(Window *window) {
  text_layer_destroy(s_sleep_text_layer);
}
/******************************************************************************************************/

/************************************thanks_window*****************************************************/
//Loads thanks window
static void thanks_window_load(Window *window) {
  // Now we prepare to initialize the thank you
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  //if user presses BACK, the app exits
  window_set_click_config_provider(window, thanks_click_config_provider);
  
  //converts user rating to char **FOR TESTING**
  static char userRating[32];  
  snprintf(userRating, sizeof(userRating), "You rated %d ", rating);
  
  //sets size of thanks_text
  s_thanks_text_layer = text_layer_create(GRect(0, 72, bounds.size.w, 20));
  //adds text to thanks_text
  text_layer_set_text(s_thanks_text_layer, userRating);
  //sets background color to cyan
  text_layer_set_background_color(s_thanks_text_layer, GColorCyan);
  //sets text color to yellow
  text_layer_set_text_color(s_thanks_text_layer, GColorYellow);
  //centers text
  text_layer_set_text_alignment(s_thanks_text_layer, GTextAlignmentCenter);
  //dont know, think its important
  layer_add_child(window_layer, text_layer_get_layer(s_thanks_text_layer));
  
}

//Unloads thanks window
static void thanks_window_unload(Window *window) {
  text_layer_destroy(s_thanks_text_layer);
}
/******************************************************************************************************/


/****************Initialization, Deinitialization, and Main********************************************/
static void init() {

  //Creates intro window
  s_intro_window = window_create();
  window_set_window_handlers(s_intro_window, (WindowHandlers){
    //This is where we call intro_window_load, if we wanted to pass something in we should do it here
    .load = intro_window_load,
    .unload = intro_window_unload,
  });
  
  //creates main window
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  
  //creates thanks window
  s_thanks_window = window_create();
  window_set_window_handlers(s_thanks_window, (WindowHandlers) {
    .load = thanks_window_load,
    .unload = thanks_window_unload,
  });
  //window_stack_push(s_main_window, true);
  window_stack_push(s_intro_window, true);
}

//Deinitialization, only features main
static void deinit() {
  window_destroy(s_main_window);
}

//main
int main(void) {
  init();
  app_event_loop();
  deinit();
}
/******************************************************************************************************/