#include <pebble.h>
#include "message_keys.auto.h"

static Window *s_window;
static Layer *s_face;
static GFont s_time_font;
static GFont s_date_font;
static char s_time[8];
static char s_date[7];
static BatteryChargeState s_battery;
static bool s_dark_mode;

enum { SETTINGS_DARK_MODE = 1 };

static GColor background_color(void) {
  return s_dark_mode ? GColorBlack : GColorWhite;
}

static GColor foreground_color(void) {
  return s_dark_mode ? GColorWhite : GColorBlack;
}

static void inbox_received(DictionaryIterator *iterator, void *context) {
  Tuple *theme = dict_find(iterator, MESSAGE_KEY_DarkMode);
  if (!theme || (theme->type != TUPLE_INT && theme->type != TUPLE_UINT)) {
    return;
  }
  bool dark_mode = theme->value->int32 != 0;
  if (dark_mode == s_dark_mode) {
    return;
  }
  s_dark_mode = dark_mode;
  persist_write_bool(SETTINGS_DARK_MODE, s_dark_mode);
  window_set_background_color(s_window, background_color());
  if (s_face) {
    layer_mark_dirty(s_face);
  }
}

static void update_time(void) {
  time_t now = time(NULL);
  struct tm *local = localtime(&now);
  bool use_24h = clock_is_24h_style();
  strftime(s_time, sizeof(s_time), use_24h ? "%H:%M" : "%I:%M", local);
  if (!use_24h && s_time[0] == '0') {
    memmove(s_time, s_time + 1, strlen(s_time));
  }
  static const char *months[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
  };
  snprintf(s_date, sizeof(s_date), "%s %d", months[local->tm_mon], local->tm_mday);
  if (s_face) {
    layer_mark_dirty(s_face);
  }
}

static void draw_battery(GContext *ctx, int16_t x, int16_t top, int16_t bottom) {
  const int16_t width = 3;
  const int16_t height = bottom - top + 1;
  const int16_t filled_height =
      (height * s_battery.charge_percent + 99) / 100;
  const int16_t fill_top = bottom + 1 - filled_height;

#ifdef PBL_COLOR
  graphics_context_set_fill_color(ctx, s_dark_mode ? GColorDarkGray : GColorLightGray);
  graphics_fill_rect(ctx, GRect(x, top, width, height), 0, GCornerNone);
#else
  graphics_context_set_fill_color(ctx, background_color());
  graphics_fill_rect(ctx, GRect(x, top, width, height), 0, GCornerNone);
  graphics_context_set_fill_color(ctx, foreground_color());
  for (int16_t y = top; y + 1 < fill_top; y += 4) {
    graphics_fill_rect(ctx, GRect(x, y, width, 2), 0, GCornerNone);
  }
#endif
  graphics_context_set_fill_color(ctx, foreground_color());
  if (filled_height > 0) {
    graphics_fill_rect(ctx, GRect(x, fill_top, width, filled_height), 0, GCornerNone);
  }
}

static void draw_face(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
#if defined(PBL_PLATFORM_EMERY) || defined(PBL_PLATFORM_GABBRO)
  const int16_t date_glyph_top = 7;
  const int16_t date_glyph_height = 17;
  const int16_t time_glyph_top = 14;
  const int16_t time_glyph_height = 34;
  const int16_t text_gap = 15;
#else
  const int16_t date_glyph_top = 6;
  const int16_t date_glyph_height = 15;
  const int16_t time_glyph_top = 12;
  const int16_t time_glyph_height = 30;
  const int16_t text_gap = 13;
#endif
  const int16_t group_height = date_glyph_height + time_glyph_height + text_gap;
  const int16_t optical_offset = -3;
  const int16_t group_top = (bounds.size.h - group_height) / 2 + optical_offset;
  const int16_t date_y = group_top - date_glyph_top;
  const int16_t time_y = group_top + date_glyph_height + text_gap - time_glyph_top;
  const int16_t battery_width = 3;
  const int16_t battery_gap = PBL_IF_ROUND_ELSE(21, 9);
  const int16_t battery_x = bounds.size.w - battery_gap - battery_width;
  const int16_t text_right_bearing = 1;
  const int16_t text_width = battery_x - battery_gap + text_right_bearing;
  graphics_context_set_text_color(ctx, foreground_color());
  graphics_draw_text(ctx, s_time, s_time_font,
                     GRect(0, time_y, text_width, time_glyph_top + time_glyph_height + 4),
                     GTextOverflowModeTrailingEllipsis, GTextAlignmentRight, NULL);
  graphics_draw_text(ctx, s_date, s_date_font,
                     GRect(0, date_y, text_width, date_glyph_top + date_glyph_height + 4),
                     GTextOverflowModeTrailingEllipsis, GTextAlignmentRight, NULL);

  draw_battery(ctx, battery_x, group_top, group_top + group_height - 1);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void battery_handler(BatteryChargeState state) {
  s_battery = state;
  if (s_face) {
    layer_mark_dirty(s_face);
  }
}

static void window_load(Window *window) {
#if defined(PBL_PLATFORM_EMERY) || defined(PBL_PLATFORM_GABBRO)
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_MEDIUM_48));
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_MEDIUM_24));
#else
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_MEDIUM_42));
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_MEDIUM_21));
#endif
  Layer *root = window_get_root_layer(window);
  s_face = layer_create(layer_get_bounds(root));
  layer_set_update_proc(s_face, draw_face);
  layer_add_child(root, s_face);
  update_time();
}

static void window_unload(Window *window) {
  layer_destroy(s_face);
  s_face = NULL;
  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(s_date_font);
}

int main(void) {
  s_dark_mode = persist_read_bool(SETTINGS_DARK_MODE);
  s_battery = battery_state_service_peek();
  s_window = window_create();
  window_set_background_color(s_window, background_color());
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, false);
  app_message_register_inbox_received(inbox_received);
  app_message_open(128, 128);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  battery_state_service_subscribe(battery_handler);
  app_event_loop();
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  app_message_deregister_callbacks();
  window_destroy(s_window);
}
