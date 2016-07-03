#include <pebble.h>
#include <special-draw/special-draw.h>
#include <special-draw/special-draw-opacity.h>
#include <special-draw/special-draw-rotation.h>
#include <special-draw-dither/special-draw-dither.h>

Window *main_window;

void layer_update(Layer *me, GContext *ctx) {
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_context_set_fill_color(ctx, GColorRed);
  graphics_fill_rect(ctx, GRect(32, 54, 80, 60), 0, GCornerNone);
  graphics_draw_rect(ctx, GRect(32, 54, 80, 60));

  // Begin the session.
  GSpecialSession * session = graphics_context_begin_special_draw(ctx);

  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_context_set_fill_color(ctx, GColorRed);
  graphics_fill_rect(ctx, GRect(42, 64, 80, 60), 0, GCornerNone);
  graphics_draw_rect(ctx, GRect(42, 64, 80, 60));

  // Add modifiers as desired.
  graphics_context_special_session_add_modifier(session, graphics_special_draw_create_dither_modifier(GColorRed, 0, 128, 128));
//   graphics_context_special_session_add_modifier(session, graphics_special_draw_create_opacity_modifier(GOpacity2));
  graphics_context_special_session_add_modifier(session, graphics_special_draw_create_rotation_modifier(TRIG_MAX_ANGLE / 100));
  // End the session.
  graphics_context_end_special_draw(session);

}

// ===============================================================================================================================================
//  Main Pebble Functions
// ===============================================================================================================================================
void main_window_load(Window *window) {
  Layer *root_layer = window_get_root_layer(window);
  layer_set_update_proc(root_layer, layer_update);
}

void init() {
  main_window = window_create();
  window_set_window_handlers(main_window, (WindowHandlers) {.load = main_window_load});
  window_set_background_color(main_window, GColorWhite);
  window_stack_push(main_window, true);
}

void deinit() {
  window_destroy(main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
