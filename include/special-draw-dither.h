#include <pebble.h>
#include <@smallstoneapps/linked-list/linked-list.h>
#include "special-draw/special-draw.h"

GSpecialSessionModifier * graphics_special_draw_create_dither_modifier(GColor replace_color, uint8_t r, uint8_t g, uint8_t b);