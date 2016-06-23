#include <pebble.h>
#include "special-draw-dither.h"
typedef struct dither_struct {
  GColor replace_color;
  uint8_t r;
  uint8_t g;
  uint8_t b;
} dither_struct;

static const uint8_t dither_table[8][8] = {
  { 0, 32,  8, 40,  2, 34, 10, 42}, /* 8x8 Bayer ordered dithering */
  {48, 16, 56, 24, 50, 18, 58, 26}, /* pattern. Each input pixel */
  {12, 44,  4, 36, 14, 46,  6, 38}, /* is scaled to the 0..63 range */
  {60, 28, 52, 20, 62, 30, 54, 22}, /* before looking in this table */
  { 3, 35, 11, 43,  1, 33,  9, 41}, /* to determine the action. */
  {51, 19, 59, 27, 49, 17, 57, 25},
  {15, 47,  7, 39, 13, 45,  5, 37},
  {63, 31, 55, 23, 61, 29, 53, 21}
};

static void prv_replace_color_with_dither(GBitmap * framebuffer, dither_struct *rgb) {
  #if defined(PBL_BW)
    uint8_t intensity = (((rgb->r+rgb->r+rgb->r+rgb->b+rgb->g+rgb->g+rgb->g+rgb->g) >> 3)+5)/4 - 1;
  #endif  
  GRect bounds = gbitmap_get_bounds(framebuffer);
  for(int y = 0; y < bounds.size.h; y++) {
    GBitmapDataRowInfo info = gbitmap_get_data_row_info(framebuffer, y);
    for(int x = info.min_x; x <= info.max_x; x++) {
      #if defined(PBL_BW) && defined(PBL_RECT)
        if(!(info.data[x/8] & (1<<(x%8))) == !((rgb->replace_color).argb&1)) {
          info.data[x/8] &= ~(1 << (x%8));                                         // Color Black
          info.data[x/8] |= (intensity > dither_table[x&7][y&7] ? 1 : 0) << (x%8); // Color White
        }
      #elif defined(PBL_COLOR)
        if(info.data[x]==(rgb->replace_color).argb) {
          uint8_t d = dither_table[x&7][y&7];
          info.data[x] = 0b11000000 +
            ((64*(rgb->r%85)/85 > d ? rgb->r/85+1 : rgb->r/85)<<4) + 
            ((64*(rgb->g%85)/85 > d ? rgb->g/85+1 : rgb->g/85)<<2) + 
            ((64*(rgb->b%85)/85 > d ? rgb->b/85+1 : rgb->b/85)   );
        }
      #endif
    }
  }
}


static void prv_run_modifier(GSpecialSessionModifier * modifier,
        GBitmap * session_bitmap) {
    prv_replace_color_with_dither(session_bitmap, ((dither_struct *) modifier->context));
}


static void prv_destroy_modifier(GSpecialSessionModifier * modifier) {
    free(modifier->context);
    free(modifier);
}


GSpecialSessionModifier * graphics_special_draw_create_dither_modifier(GColor replace_color, uint8_t r, uint8_t g, uint8_t b) {
    GSpecialSessionModifier * mod = malloc(sizeof(GSpecialSessionModifier));
    mod->overrides_draw = false;
    mod->action.modifier_run = prv_run_modifier;
    mod->destroy = prv_destroy_modifier;
    mod->context = malloc(sizeof(dither_struct));
    ((dither_struct *) mod->context)->replace_color = replace_color;
    ((dither_struct *) mod->context)->r = r;
    ((dither_struct *) mod->context)->g = g;
    ((dither_struct *) mod->context)->b = b;
    return mod;
}
