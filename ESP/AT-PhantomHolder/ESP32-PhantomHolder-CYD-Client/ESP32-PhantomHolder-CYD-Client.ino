// AT-PhantomHolder ESP32 Servo-Project:
const char* ssid = "ESP32";
const char* password = "12345678";
const char* mqtt_server = "192.168.4.1";
const int mqtt_port =  1883;

const char* at_mqtt_topic_servo_x_deg    = "wfauto/Ser1"; //payload = 100 -> X fährt auf 100° (begrenzt auf 60-120°)
const char* at_mqtt_topic_servo_y_deg    = "wfauto/Ser2"; //payload = 110 -> Y fährt auf 110° (begrenzt auf 60-120°)
const char* at_mqtt_topic_servo_rot_deg  = "wfauto/Ser3"; //payload =  80 -> Rotation fährt auf 80° (begrenzt auf 60-120°)
const char* at_mqtt_topic_servo_tilt_deg = "wfauto/Ser4"; //payload = 130 -> Tilt fährt auf 130° (begrenzt auf 90-150°)


#include <Arduino.h>
#include <lvgl.h>
#include <AMK_CYD_Universal_TFT.h>
#include <AMK_CYD_LVGL.h>
AMK_display     display;
AMK_touch       touch;
TFT_eSPI        tft;
AMK_CYD_LVGL    lvgl;

/*
#if defined(ESP32)
#include <WiFiClientSecure.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#endif
WiFiClientSecure net;
//setup()
#if defined(ESP8266)
  net.setInsecure();
#elif defined(ESP32)
  net.setInsecure();
#endif
*/
#include <AViShaMQTT.h>
AViShaMQTT mqtt(ssid, password, mqtt_server, mqtt_port /*,net*/);

// demo: sample MQTT topics
//const char* topicLED = "test/topic/led";
//const char* topicSensor = "test/topic/sensor";
//const unsigned long interval = 1000;
//unsigned long timer = 0;
//unsigned long counter = 0;


void setup_mqtt() {
    if (!mqtt.begin()) {
      Serial.print("[APP]: Connection failed, state: ");
      Serial.println(mqtt.state());
      while (1) delay(1000);
    }

    // demo: subscribe MQTT topics
    //mqtt.subscribe(topicLED, 1); // QoS 1

    // todo:
    // subscribe here also AT-Servor-Prooject MQTT-Topics (once there are ones)
}


// ---- PhantomHolder axis UI ----
enum Axis { AXIS_X = 0, AXIS_Y, AXIS_ROT, AXIS_TILT, AXIS_COUNT };

struct AxisInfo {
    const char* name;
    const char* topic;
    int minVal;
    int maxVal;
    int step;
    lv_obj_t* slider;
    lv_obj_t* valueLabel;
};

static AxisInfo axes[AXIS_COUNT] = {
    { "X",    at_mqtt_topic_servo_x_deg,    60,  120, 5, nullptr, nullptr },
    { "Y",    at_mqtt_topic_servo_y_deg,    60,  120, 5, nullptr, nullptr },
    { "Rot",  at_mqtt_topic_servo_rot_deg,  60,  120, 5, nullptr, nullptr },
    { "Tilt", at_mqtt_topic_servo_tilt_deg, 90,  150, 5, nullptr, nullptr },
};

static int axisCenter(int idx) {
    return (axes[idx].minVal + axes[idx].maxVal) / 2;
}

static void axis_update(int idx, int value, bool publish) {
    AxisInfo& a = axes[idx];
    if (value < a.minVal) value = a.minVal;
    if (value > a.maxVal) value = a.maxVal;
    lv_slider_set_value(a.slider, value, LV_ANIM_OFF);
    lv_label_set_text_fmt(a.valueLabel, "%s: %d", a.name, value);
    if (publish) {
        mqtt.publish(a.topic, String(value), false, 1);
    }
}

// user data = axis index
static void slider_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    int idx = (int)(intptr_t)lv_event_get_user_data(e);
    lv_obj_t * s = (lv_obj_t *)lv_event_get_target(e);
    if (code == LV_EVENT_VALUE_CHANGED) {
        lv_label_set_text_fmt(axes[idx].valueLabel, "%s: %d", axes[idx].name, (int)lv_slider_get_value(s));
    } else if (code == LV_EVENT_RELEASED) {
        // publish only when the slider is released
        axis_update(idx, lv_slider_get_value(s), true);
    }
}

// user data = (axis index << 8) | (direction + 1)  -> direction -1 or +1
static void step_btn_event_cb(lv_event_t * e) {
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;
    uint32_t ud = (uint32_t)(uintptr_t)lv_event_get_user_data(e);
    int idx = (int)(ud >> 8);
    int dir = (int)(ud & 0xFF) - 1;
    int cur = lv_slider_get_value(axes[idx].slider);
    axis_update(idx, cur + dir * axes[idx].step, true);
}

static void reset_btn_event_cb(lv_event_t * e) {
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;
    for (int i = 0; i < AXIS_COUNT; i++) {
        axis_update(i, axisCenter(i), true);
    }
}

static lv_obj_t* create_step_button(lv_obj_t* parent, const char* text, int idx, int dir,
                                    lv_coord_t w, lv_coord_t h, lv_align_t align, lv_coord_t x, lv_coord_t y) {
    lv_obj_t * btn = lv_btn_create(parent);
    lv_obj_set_size(btn, w, h);
    lv_obj_align(btn, align, x, y);
    uint32_t ud = ((uint32_t)idx << 8) | (uint32_t)(dir + 1);
    lv_obj_add_event_cb(btn, step_btn_event_cb, LV_EVENT_CLICKED, (void*)(uintptr_t)ud);
    lv_obj_t * lbl = lv_label_create(btn);
    lv_label_set_text(lbl, text);
    lv_obj_center(lbl);
    return btn;
}

static void create_axis_slider(lv_obj_t* parent, int idx, lv_coord_t w, lv_coord_t h,
                               lv_align_t align, lv_coord_t x, lv_coord_t y) {
    AxisInfo& a = axes[idx];
    a.slider = lv_slider_create(parent);
    lv_obj_set_size(a.slider, w, h);   // height > width -> vertical slider
    lv_obj_align(a.slider, align, x, y);
    lv_slider_set_range(a.slider, a.minVal, a.maxVal);
    lv_obj_add_event_cb(a.slider, slider_event_cb, LV_EVENT_ALL, (void*)(intptr_t)idx);

    a.valueLabel = lv_label_create(parent);
    lv_obj_align_to(a.valueLabel, a.slider, LV_ALIGN_OUT_TOP_MID, 0, -4);
    axis_update(idx, axisCenter(idx), false);
}

void setup_ui() {
    // 1. Universal 1-line auto-setup (v1.6.0)
    tft.autoSetup(3);
    tft.autoCalibrate(); // Auto-calibrates uncalibrated resistive touch (< 1 ms if already saved; 0 ms if capacitive)

    // 2. Initialize LVGL in 1 LINE!
    if (!lvgl.begin(display, touch)) {
        Serial.println("LVGL allocation failed!");
        return;
    }

    lv_obj_t * scr = lv_scr_act();

    // Header Label
    lv_obj_t * title = lv_label_create(scr);
    lv_label_set_text(title, "AMK_CYD Universal - LVGL Demo");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 5);

    // Layout (320x240 landscape):
    //  left : Y slider (vertical) with Y- (bottom) / Y+ (top) buttons
    //  right: X, Rot, Tilt as horizontal sliders with -/+ buttons
    const lv_coord_t btnW = 36, btnH = 28;

    // --- Y axis (vertical) ---
    create_step_button(scr, "Y+", AXIS_Y, +1, btnW, btnH, LV_ALIGN_TOP_LEFT, 10, 50);
    create_axis_slider(scr, AXIS_Y, 12, 90, LV_ALIGN_TOP_LEFT, 22, 90);
    create_step_button(scr, "Y-", AXIS_Y, -1, btnW, btnH, LV_ALIGN_TOP_LEFT, 10, 190);

    // --- X, Rot, Tilt (horizontal) ---
    const lv_coord_t sliderW = 140;
    const lv_coord_t rowY[3] = { 55, 100, 145 };
    const int rowAxis[3] = { AXIS_X, AXIS_ROT, AXIS_TILT };
    const char* minusTxt[3] = { "X-", "Rot-", "Tilt-" };
    const char* plusTxt[3]  = { "X+", "Rot+", "Tilt+" };
    for (int r = 0; r < 3; r++) {
        int idx = rowAxis[r];
        create_step_button(scr, minusTxt[r], idx, -1, 50, btnH, LV_ALIGN_TOP_LEFT, 60, rowY[r]);
        create_axis_slider(scr, idx, sliderW, 10, LV_ALIGN_TOP_LEFT, 60 + 50 + 10, rowY[r] + 9);
        create_step_button(scr, plusTxt[r], idx, +1, 50, btnH, LV_ALIGN_TOP_RIGHT, -5, rowY[r]);
    }

    // --- Reset ---
    lv_obj_t * resetBtn = lv_btn_create(scr);
    lv_obj_set_size(resetBtn, 100, 32);
    lv_obj_align(resetBtn, LV_ALIGN_BOTTOM_RIGHT, -10, -8);
    lv_obj_add_event_cb(resetBtn, reset_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * resetLbl = lv_label_create(resetBtn);
    lv_label_set_text(resetLbl, "Reset");
    lv_obj_center(resetLbl);
}


void setup() {
    Serial.begin(115200);
    delay(100);
    Serial.println("\n[AMK_CYD_Universal_TFT] Starting LVGL_QuickStart...");

    setup_ui();
    setup_mqtt();
}


void loop() {
    lvgl.tick(); // Process LVGL timers, animations & touch events
    mqtt.loop(); // Process MQTT

    // demo: publish simulated MQTT events
    //if (millis() - timer >= interval) {
    //  timer = millis();
    //  counter++;
    //  mqtt.publish(topicSensor, String(counter), true, 2); // Retain & QoS 2
    //}
    // demo: read MQTT message and represent value via LED
    String msg = mqtt.getIncomingMessage();
    if (msg != "") {
      for (int i=0; i<msg.toInt(); i++) {
        tft.setRgbLed(true, false, false); // Red pulse
        delay(150); // debounce tap
        tft.turnLedOff();
      }
      mqtt.setIncomingMessage("");
    }

    //delay(5);
}
