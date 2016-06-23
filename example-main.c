#include <pebble.h>
#include <special-draw/special-draw.h>
#include <special-draw/special-draw-opacity.h>
#include <special-draw/special-draw-rotation.h>
#include <special-draw-dither/special-draw-dither.h>

Window *main_window;
GContext *c;

void draw_pixel_basalt(GContext *ctx, GPoint pt, uint8_t color) {
  if(pt.x>=0 && pt.x<144 && pt.y>=0 && pt.y<168)
    ((uint8_t(*)[144])*(uintptr_t*)ctx)[pt.y][pt.x] = color;
}

// Fill screen with color.  Note: For Aplite, color should be either 0 or 255. Vertical stripes will appear otherwise.
void fill_screen(GContext *ctx, GColor color) {
  uint32_t *framebuffer = (uint32_t*)*(uintptr_t*)ctx;
  #if defined(PBL_PLATFORM_APLITE)
    memset(framebuffer, color.argb, 20*168);
    graphics_release_frame_buffer(ctx, graphics_capture_frame_buffer(ctx));  // Needed on Aplite to force screen to draw
  #elif defined(PBL_PLATFORM_BASALT)
    memset(framebuffer, color.argb, 144*168);
  #elif defined(PBL_PLATFORM_CHALK)
    memset(framebuffer, color.argb, 76 + 25792); // First pixel on PTR doesn't start til framebuffer + 76
  #endif
}

void layer_update(Layer *me, GContext *ctx) {
//   c = ctx;
//   fill_screen(ctx, (GColor){.argb=0b11111111});  // Fill the whole background with Dark Grey
// //   fill_screen(ctx, (GColor){.argb=0b11010101});  // Fill the whole background with Dark Grey
//   graphics_context_set_stroke_color(ctx, (GColor){.argb=0b10000100});
//   //graphics_draw_pixel(ctx, GPoint(x, y));
//   graphics_draw_circle(ctx, GPoint(3, 3), 4);

//   // Get the framebuffer:
//   //uint8_t *framebuffer = (uint8_t*)*(uintptr_t*)ctx;  // framebuffer is now an array of uint8_t's
//   //uint8_t (*screen)[144] = (uint8_t(*)[144]) framebuffer;    //screen[y][x] = color;
//   uint8_t (*screen)[144] = (uint8_t(*)[144])*(uintptr_t*)ctx;  //screen[y][x] = color;
//   for(int y = 0; y<1; y++)
//     for(int x = 0; x<10; x++)
// //       printf("%d x %d = %d", x, y, screen[x][y]);
//       printf("%d x %d = 0b %d%d %d%d %d%d %d%d", x, y, (screen[x][y]>>7)&1, (screen[x][y]>>6)&1, (screen[x][y]>>5)&1, (screen[x][y]>>4)&1, (screen[x][y]>>3)&1, (screen[x][y]>>2)&1, (screen[x][y]>>1)&1, (screen[x][y])&1);
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

  //graphics_draw_rect(ctx, GRect(42, 64, 80, 60));

  // Add modifiers as desired.
  graphics_context_special_session_add_modifier(session, graphics_special_draw_create_dither_modifier(GColorRed, 0, 128, 128));
//   graphics_context_special_session_add_modifier(session, graphics_special_draw_create_opacity_modifier(GOpacity2));
  graphics_context_special_session_add_modifier(session, graphics_special_draw_create_rotation_modifier(TRIG_MAX_ANGLE / 100));
  // End the session.
  graphics_context_end_special_draw(session);

}

// ------------------------------------------------------------------------------------------------------------------------------------------------ //
//  Button Functions
// ------------------------------------------------------------------------ //
static void     up_single_click_handler(ClickRecognizerRef recognizer, void *context) {

}

static void select_single_click_handler(ClickRecognizerRef recognizer, void *context) {

}
static void   select_long_click_handler(ClickRecognizerRef recognizer, void *context) {}
static void   down_single_click_handler(ClickRecognizerRef recognizer, void *context) {
//   uint8_t (*screen)[144] = (uint8_t(*)[144])*(uintptr_t*)c;  //screen[y][x] = color;
//   for(int y = 0; y<1; y++)
//     for(int x = 0; x<10; x++)
//       printf("%d x %d = 0b%d%d%d%d", x, y, (screen[x][y]>>6)&3, (screen[x][y]>>4)&3, (screen[x][y]>>2)&3, (screen[x][y])&3);
}


static void click_config_provider(void *context) {
  window_single_repeating_click_subscribe(BUTTON_ID_UP,    30,  up_single_click_handler);
            window_single_click_subscribe(BUTTON_ID_SELECT, select_single_click_handler);
              window_long_click_subscribe(BUTTON_ID_SELECT, 0, select_long_click_handler, NULL);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN , 30, down_single_click_handler);
}

// ===============================================================================================================================================
//  Main Pebble Functions
// ===============================================================================================================================================
void main_window_load(Window *window) {
  // Set up window
  window_set_click_config_provider(window, click_config_provider);
  Layer *root_layer = window_get_root_layer(window);
  layer_set_update_proc(root_layer, layer_update);
}

void main_window_unload(Window *window) {

}

void init() {
  main_window = window_create();
  window_set_window_handlers(main_window, (WindowHandlers) {.load = main_window_load,.unload = main_window_unload});
  window_set_background_color(main_window, GColorWhite);//IF_BW_COLOR(GColorWhite, GColorRed));
  window_stack_push(main_window, true);
}

void deinit() {
  window_destroy(main_window);  // Destroy main Window
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
