#include <pebble.h>
  
static Window *s_main_window = NULL;
static Layer *s_canvas_layer = NULL;
static int cur_hour = 0;
static int cur_minute = 0;


static void canvas_update_proc(Layer *this_layer, GContext *ctx) {
  // draw black background
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, layer_get_bounds(this_layer), 0, GCornerNone);

  // circles are drawn in white
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_context_set_stroke_color(ctx, GColorWhite);

  // hour circles
  for (int i = 0; i < 5; i++) {
    GPoint p = GPoint(132 - 30*i, 68);
    // check bits of time
    if ((cur_hour >> i) & 1)
      graphics_fill_circle(ctx, p, 10);
    else
      graphics_draw_circle(ctx, p, 10);
  }

  // minute circles
  for (int i = 0; i < 6; i++) {
    GPoint p = GPoint(132 - 24*i, 100);
    // check bits of time
    if ((cur_minute >> i) & 1)
      graphics_fill_circle(ctx, p, 10);
    else
      graphics_draw_circle(ctx, p, 10);
  }
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  // Create Layer
  s_canvas_layer = layer_create(layer_get_bounds(window_layer));
  layer_add_child(window_layer, s_canvas_layer);

  // Set the update_proc
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);
}

static void main_window_unload(Window *window) {
  layer_destroy(s_canvas_layer);
}

static void update_time(struct tm *cur_time) {
  // set globals
  cur_hour = cur_time->tm_hour;
  cur_minute = cur_time->tm_min;

  // make sure new time gets displayed
  if (s_canvas_layer)
    layer_mark_dirty(s_canvas_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time(tick_time);
}
  
static void init() {
  // set initial time
  time_t now = time(NULL);
  update_time(localtime(&now));

  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
