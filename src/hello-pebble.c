#include <pebble.h>

Window *window;

TextLayer *text_layer;

struct sleepMood {
	int mood;
	int hours;
};



struct sleepMood test;

struct sleepMood test2;

void init() {
	// Create the Window
	window = window_create();
	
	APP_LOG(APP_LOG_LEVEL_INFO, "Main window loaded.");

	text_layer = text_layer_create(GRect(0, 0, 144, 40));
	text_layer_set_text(text_layer, "Hello, Pebble SDK;\nGoodbye CloudPebble!");
	layer_add_child(window_get_root_layer(window), 
                    text_layer_get_layer(text_layer));

	// Push to the stack, animated
	window_stack_push(window, true);
	
	// Use the step count metric
HealthMetric metric = HealthMetricStepCount;

// Create timestamps for now (the end time) and midnight (the start time)
time_t end = time(NULL);
time_t start = time_start_of_today();

// Check the metric has data available for today
HealthServiceAccessibilityMask mask = health_service_metric_accessible(metric, 
  start, end);


test.hours=2;
test2.hours=2;


if(mask == HealthServiceAccessibilityMaskAvailable) {
  // Data is available!
  APP_LOG(APP_LOG_LEVEL_INFO, "Steps today: %d", 
          (int)health_service_sum_today(metric));
test.hours = (int)health_service_sum_today(metric);
} else {
  // No data recorded yet today
  APP_LOG(APP_LOG_LEVEL_ERROR, "Data unavailable!");
}

persist_write_data(0, &test, sizeof(test));

persist_read_data(0, &test2, sizeof(test2));

APP_LOG(APP_LOG_LEVEL_INFO, "TEST: %d", test2.hours);
}

void deinit() {
	
	text_layer_destroy(text_layer);
	
	// Destroy the Window
	window_destroy(window);
}

int main() {
	// Initialize the app
	init();

	// Wait for app events
	app_event_loop();

	// Deinitialize the app
	deinit();

	// App finished without error
	return 0;
}
