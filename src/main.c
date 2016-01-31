#include "pebble.h"

#define NUM_MENU_ICONS 6
#define NUM_FIRST_MENU_ITEMS 6
#define FIVE = 5

/****************************Global Variable Declarations*******************************************/
static Window *s_main_window,                              //Window where user rates their sleep
              *s_intro_window,                             //Intro Window
              *s_thanks_window,                            //Thank-you window
              *s_last5_window;

static TextLayer *s_sleep_text_layer,                      //Text that says how many hours user slept
                 *s_thanks_text_layer,                      //Thanks user for rating their sleep
                 *z1,
                 *s_last5_text_layer[5],
                 *s_mid[5];

static MenuLayer *s_menu_layer;                            //Menu where user rates their sleep
                                                           //(used in s_main_window)

static GBitmap *s_menu_icons[NUM_MENU_ICONS];              //Images corresponding to menu choices


int rating = 0;                                            //Sleep rating that can be stored

struct healthData {
	int mood;
	int sleepSeconds;
	int steps;
};

// struct sleepMood a[5];
struct healthData today;
struct healthData weekData[5];

 /****************************Brian/Michael*********************************************************/

 void getHealthData() {

	// Use the sleep count metric (sleep seconds)
	HealthMetric metric = HealthMetricSleepSeconds;

	// Create timestamps for now (the end time) and midnight (the start time)
	time_t end = time(NULL);
	time_t start = time_start_of_today();

	// Check the metric has data available for today
	HealthServiceAccessibilityMask mask = health_service_metric_accessible(metric,
		start, end);

	if(mask == HealthServiceAccessibilityMaskAvailable) {
		// Data is available!
		APP_LOG(APP_LOG_LEVEL_INFO, "Sleep seconds today: %d",
          (int)health_service_sum_today(metric));
	today.sleepSeconds = (int)health_service_sum_today(metric);
	} else {
		// No data recorded yet today
		APP_LOG(APP_LOG_LEVEL_ERROR, "Data unavailable!");
	}


	metric = HealthMetricStepCount;
	mask = health_service_metric_accessible(metric,
		start, end);

	if(mask == HealthServiceAccessibilityMaskAvailable) {
		// Data is available!
		APP_LOG(APP_LOG_LEVEL_INFO, "Step count today: %d",
          (int)health_service_sum_today(metric));
	today.steps = (int)health_service_sum_today(metric);
	} else {
		// No data recorded yet today
		APP_LOG(APP_LOG_LEVEL_ERROR, "Data unavailable!");
	}
}

static void getDate(char* buf){
    time_t now;
    struct tm ts;
    //char buf[11];
    ////// Get current time
    time(&now);
    ////// Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
    ts = *localtime(&now);
    strftime(buf, sizeof(buf), "%m-%d-%Y\n", &ts);
    APP_LOG(APP_LOG_LEVEL_INFO, "Got Date: %s", buf);
    //return *buf;
}

static int getCurrentDayNumber(){
    time_t now;
    struct tm ts;
    //////char buf[80];
    char yearBuf[5];
    char monthBuf[3];
    char dayBuf[3];
    ////// Get current time
    time(&now);
    ////// Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
    ts = *localtime(&now);
    strftime(yearBuf, sizeof(yearBuf), "%Y", &ts);
    strftime(monthBuf, sizeof(monthBuf), "%m", &ts);
    strftime(dayBuf, sizeof(dayBuf), "%d", &ts);

    APP_LOG(APP_LOG_LEVEL_INFO, "Current Month: %s", monthBuf);
    APP_LOG(APP_LOG_LEVEL_INFO, "Current dayOfMonth: %s", dayBuf);
    APP_LOG(APP_LOG_LEVEL_INFO, "Current year: %s", yearBuf);

	int month = atoi(monthBuf);
	int dayOfMonth = atoi(dayBuf);
	int year = atoi(yearBuf);

	APP_LOG(APP_LOG_LEVEL_INFO, "Current Month: %d", month);
    APP_LOG(APP_LOG_LEVEL_INFO, "Current dayOfMonth: %d", dayOfMonth);
    APP_LOG(APP_LOG_LEVEL_INFO, "Current year: %d", year);


	return dayOfMonth + ((month < 3) ? (int)((306 * month - 301) / 10) : (int)((306 * month - 913) / 10) + ((year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) ? 60 : 59));
}


static void saveTodayData() {
    int currentDay = getCurrentDayNumber();
    APP_LOG(APP_LOG_LEVEL_INFO, "Current Day: %d", currentDay);
	persist_write_data(currentDay, &today, sizeof(today));
    weekData[0] = today;
	//saveDateDataToStorage(currentDay, &today, sizeof(today));
}

static void loadDemoData(){
    //struct sleepMood demoData[6];
    int currentDay = getCurrentDayNumber();

    for(int i = 1; i < 5; i++){
        weekData[i].mood = (rand() % (5 + 1 - 1) + 1);
        weekData[i].sleepSeconds = (rand() % (36000 + 1 - 0) + 0);
        weekData[i].steps = (rand() % (6000 + 1 - 0) + 0);
        persist_write_data(currentDay - (i+1), &weekData[i], sizeof(weekData[i]));
    }


}

static void readDemoData(){
    int currentDay = getCurrentDayNumber();

        for(int i = 0; i < 6; i++){
        persist_read_data(currentDay - (i+1), &weekData[i], sizeof(weekData[i]));
        APP_LOG(APP_LOG_LEVEL_INFO, "Week data %d: seconds:%d mood:%d", currentDay - (i+1), weekData[i].sleepSeconds, weekData[i].mood);
    }
}

static void setTodayMood(int mood){
    today.mood = mood;
}



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
  //window_stack_pop_all(true);    //After providing input, exits app
  window_stack_push(s_last5_window, true);
}
//Config for clicking anything on s_thanks_window
static void thanks_click_config_provider(void *context){
  window_single_click_subscribe(BUTTON_ID_BACK, back_handler);
  //Continue
  window_single_click_subscribe(BUTTON_ID_UP, thanks_click_handler);
  //Exit
  window_single_click_subscribe(BUTTON_ID_DOWN, back_handler);
  //continue
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
    today.mood = rating;
    saveTodayData();
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
  s_sleep_text_layer = text_layer_create(GRect(0, 40, bounds.size.w, 200));
  s_mid[0] = text_layer_create(GRect(0, 33, bounds.size.w, 1));
  s_last5_text_layer[0] = text_layer_create(GRect(0, 0, bounds.size.w, 40));
  //adds text to sleep_text
  char date1[25];
      time_t now;
    struct tm ts;
    //char buf[11];
    ////// Get current time
    time(&now);
    ////// Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
    ts = *localtime(&now);
    strftime(date1, sizeof(date1), "%m-%d-%Y", &ts);
    APP_LOG(APP_LOG_LEVEL_INFO, "Got Date: %s", date1);
  text_layer_set_text(s_sleep_text_layer, hoursSlept);
  text_layer_set_text(s_last5_text_layer[0], date1);
  //sets background color to cyan
  text_layer_set_background_color(s_sleep_text_layer, GColorCyan);
  text_layer_set_background_color(s_last5_text_layer[0], GColorCyan);
  text_layer_set_background_color(s_mid[0], GColorBlack);
  //sets font color to black
  text_layer_set_text_color(s_sleep_text_layer, GColorBlack);
  text_layer_set_text_color(s_last5_text_layer[0], GColorBlack);

  text_layer_set_font(s_sleep_text_layer, fonts_get_system_font(FONT_KEY_DROID_SERIF_28_BOLD));

  //centers text
  text_layer_set_text_alignment(s_sleep_text_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_last5_text_layer[0], GTextAlignmentCenter);
  //don't know what this does, think we need it though
  layer_add_child(window_layer, text_layer_get_layer(s_sleep_text_layer));

  z1 = text_layer_create(GRect(10, 145, bounds.size.w, 20));
  text_layer_set_text(z1, "Zzz");
  text_layer_set_text_alignment(z1, GTextAlignmentLeft);
  text_layer_set_font(z1, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_background_color(z1, GColorCyan);

  layer_add_child(window_layer, text_layer_get_layer(z1));
  layer_add_child(window_layer, text_layer_get_layer(s_last5_text_layer[0]));
  layer_add_child(window_layer, text_layer_get_layer(s_mid[0]));


}

//Unloads intro_window
static void intro_window_unload(Window *window) {
  text_layer_destroy(s_sleep_text_layer);
  text_layer_destroy(z1);
  text_layer_destroy(s_last5_text_layer[0]);
  text_layer_destroy(s_mid[0]);

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
  s_thanks_text_layer = text_layer_create(GRect(0, 40, bounds.size.w, 200));
  s_mid[0] = text_layer_create(GRect(0, 33, bounds.size.w, 1));
  s_last5_text_layer[0] = text_layer_create(GRect(0, 0, bounds.size.w, 40));
  //adds text to thanks_text
  char date[25];
      time_t now;
    struct tm ts;
    //char buf[11];
    ////// Get current time
    time(&now);
    ////// Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
    ts = *localtime(&now);
    strftime(date, sizeof(date), "%m%d/%Y", &ts);
    APP_LOG(APP_LOG_LEVEL_INFO, "Got Date: %s", date);
  text_layer_set_text(s_thanks_text_layer, userRating);
  text_layer_set_text(s_last5_text_layer[0], "1/3/22");
  //sets background color to cyan
  text_layer_set_background_color(s_thanks_text_layer, GColorCyan);
  text_layer_set_background_color(s_last5_text_layer[0], GColorCyan);
  text_layer_set_background_color(s_mid[0], GColorBlack);
  //sets text color to black
  text_layer_set_text_color(s_thanks_text_layer, GColorBlack);
  text_layer_set_text_color(s_last5_text_layer[0], GColorBlack);
  //centers text
  text_layer_set_text_alignment(s_thanks_text_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_last5_text_layer[0], GTextAlignmentCenter);

  text_layer_set_font(s_thanks_text_layer, fonts_get_system_font(FONT_KEY_DROID_SERIF_28_BOLD));
  //dont know, think its important
  layer_add_child(window_layer, text_layer_get_layer(s_thanks_text_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_last5_text_layer[0]));
  layer_add_child(window_layer, text_layer_get_layer(s_mid[0]));

}

//Unloads thanks window
static void thanks_window_unload(Window *window) {
  text_layer_destroy(s_thanks_text_layer);
  text_layer_destroy(s_last5_text_layer[0]);
  text_layer_destroy(s_mid[0]);
}
/******************************************************************************************************/

/************************************ur face*****************************************************/
//Loads thanks window
static void last5_window_load(Window *window) {
  // Now we prepare to initialize the thank you
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  struct healthData *a = weekData;
  static char day[5][32];
  int rectBounds = 0;
  for(int i=0; i<5; i++){
    snprintf(day[i], sizeof(day[i]),"Mood: %d, Hours Slept: %d", a[i].mood, a[i].sleepSeconds/3600);
    //snprintf(day[i], sizeof(day[i]),"Mood: %d, Hours Slept: %d", a[i].mood, a[i].steps);
    s_last5_text_layer[i] = text_layer_create(GRect(0, rectBounds, bounds.size.w, 34));
    s_mid[i] = text_layer_create(GRect(0, rectBounds + 34 - 1, bounds.size.w, 1));
    rectBounds += 34;

    //add text to last5
    text_layer_set_text(s_last5_text_layer[i], day[i]);

    //alternate blue/gray
    if(i%2 != 0)
        text_layer_set_background_color(s_last5_text_layer[i], GColorBlue);
    else
        text_layer_set_background_color(s_last5_text_layer[i], GColorLightGray);

    text_layer_set_background_color(s_mid[i], GColorBlack);

    //centers text
    text_layer_set_text_alignment(s_last5_text_layer[i], GTextAlignmentLeft);
    layer_add_child(window_layer, text_layer_get_layer(s_last5_text_layer[i]));
    layer_add_child(window_layer, text_layer_get_layer(s_mid[i]));

  }
}

//Unloads last5 window
static void last5_window_unload(Window *window) {
  for(int i = 0; i < 4; i++){
    text_layer_destroy(s_last5_text_layer[i]);
    text_layer_destroy(s_mid[i]);
  }
}

/******************************************************************************************************/


/****************Initialization, Deinitialization, and Main********************************************/
static void init() {
    loadDemoData();
    getHealthData();
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
  //creates last5 window
  s_last5_window = window_create();
  window_set_window_handlers(s_last5_window, (WindowHandlers) {
    .load = last5_window_load,
    .unload = last5_window_unload,
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
