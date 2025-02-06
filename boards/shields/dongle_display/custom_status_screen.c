/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include "custom_status_screen.h"
#include "widgets/battery_status.h"
#include "widgets/bongo_cat.h"
#include "widgets/hid_indicators.h"
#include "widgets/layer_status.h"
#include "widgets/modifiers.h"
#include "widgets/output_status.h"
#include "widgets/wpm.h"

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static struct zmk_widget_output_status output_status_widget;
static struct zmk_widget_layer_status layer_status_widget;
static struct zmk_widget_peripheral_battery_status
    peripheral_battery_status_widget;
static struct zmk_widget_modifiers modifiers_widget;
static struct zmk_widget_bongo_cat bongo_cat_widget;
static struct zmk_widget_wpm_init wpm_widget;
static struct wpm_status_state wpm_widget;

#if IS_ENABLED(CONFIG_ZMK_HID_INDICATORS)
static struct zmk_widget_hid_indicators hid_indicators_widget;
#endif

lv_style_t global_style;

lv_obj_t *zmk_display_status_screen() {
  lv_obj_t *screen;

  screen = lv_obj_create(NULL);

  lv_style_init(&global_style);
  lv_style_set_text_font(&global_style, &lv_font_unscii_8);
  lv_style_set_text_letter_space(&global_style, 1);
  lv_style_set_text_line_space(&global_style, 1);
  lv_obj_add_style(screen, &global_style, LV_PART_MAIN);

  zmk_widget_output_status_init(&output_status_widget, screen);
  lv_obj_align(zmk_widget_output_status_obj(&output_status_widget),
               LV_ALIGN_TOP_MID, 0, 0);

  // zmk_widget_bongo_cat_init(&bongo_cat_widget, screen);
  // lv_obj_align(zmk_widget_bongo_cat_obj(&bongo_cat_widget), LV_ALIGN_CENTER,
  // 0, 0);
  /**
 * WPM status
 **/

static void set_wpm_status(struct zmk_widget_screen *widget, struct wpm_status_state state) {
    for (int i = 0; i < 9; i++) {
        widget->state.wpm[i] = widget->state.wpm[i + 1];
    }
    widget->state.wpm[9] = state.wpm;

    draw_middle(widget->obj, widget->cbuf2, &widget->state);
}

static void wpm_status_update_cb(struct wpm_status_state state) {
    struct zmk_widget_screen *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_wpm_status(widget, state); }
}

struct wpm_status_state wpm_status_get_state(const zmk_event_t *eh) {
    return (struct wpm_status_state){.wpm = zmk_wpm_get_state()};
};

ZMK_DISPLAY_WIDGET_LISTENER(widget_wpm_status, struct wpm_status_state, wpm_status_update_cb,
                            wpm_status_get_state)
ZMK_SUBSCRIPTION(widget_wpm_status, zmk_wpm_state_changed);

  zmk_widget_wpm_init(&wpm_widget, screen);
  lv_obj_align(zmk_widget_wpm_obj(&wpm_widget), LV_ALIGN_CENTER, 0, 0);

  zmk_widget_modifiers_init(&modifiers_widget, screen);
  lv_obj_align(zmk_widget_modifiers_obj(&modifiers_widget),
               LV_ALIGN_BOTTOM_LEFT, 0, 0);

#if IS_ENABLED(CONFIG_ZMK_HID_INDICATORS)
  zmk_widget_hid_indicators_init(&hid_indicators_widget, screen);
  lv_obj_align_to(zmk_widget_hid_indicators_obj(&hid_indicators_widget),
                  zmk_widget_modifiers_obj(&modifiers_widget),
                  LV_ALIGN_OUT_TOP_LEFT, 0, -2);
#endif

  zmk_widget_layer_status_init(&layer_status_widget, screen);
  lv_obj_align(zmk_widget_layer_status_obj(&layer_status_widget),
               LV_ALIGN_BOTTOM_RIGHT, 0, 0);
  // lv_obj_align_to(zmk_widget_layer_status_obj(&layer_status_widget),
  // zmk_widget_bongo_cat_obj(&bongo_cat_widget), LV_ALIGN_BOTTOM_LEFT, 0, 0);

  zmk_widget_peripheral_battery_status_init(&peripheral_battery_status_widget,
                                            screen);
  lv_obj_align(zmk_widget_peripheral_battery_status_obj(
                   &peripheral_battery_status_widget),
               LV_ALIGN_TOP_LEFT, 0, 0);

  return screen;
}
