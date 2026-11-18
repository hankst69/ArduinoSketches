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
const char* topicSensor = "test/topic/sensor";
const char* topicLED = "test/topic/led";
const unsigned long interval = 1000;
unsigned long timer = 0;
unsigned long counter = 0;


void setup_mqtt() {
    if (!mqtt.begin()) {
      Serial.print("[APP]: Connection failed, state: ");
      Serial.println(mqtt.state());
      while (1) delay(1000);
    }

    // demo: subscribe MQTT topics
    mqtt.subscribe(topicLED, 1); // QoS 1

    // todo:
    // subscribe here also AT-Servor-Prooject MQTT-Topics (once there are ones)
}


static void btn_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = (lv_obj_t *)lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED) {
        static uint8_t count = 0;
        count++;
        lv_obj_t * label = lv_obj_get_child(btn, 0);
        lv_label_set_text_fmt(label, "Clicks: %d", count);
    }

    Serial.print("\nbtn_event_cb code: ");
    if (code == LV_EVENT_CLICKED) {
        Serial.println("LV_EVENT_CLICKED");
    } else {
        Serial.println(code);
    }
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

    // 3. Build UI with standard LVGL widgets:
    // Header Label
    lv_obj_t * title = lv_label_create(lv_scr_act());
    lv_label_set_text(title, "AMK_CYD Universal - LVGL Demo");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 15);

    // Interactive Button
    lv_obj_t * btn = lv_btn_create(lv_scr_act());
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, -10);
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL);

    lv_obj_t * btn_lbl = lv_label_create(btn);
    lv_label_set_text(btn_lbl, "Click Me!");
    lv_obj_center(btn_lbl);

    // Interactive Slider Widget
    lv_obj_t * slider = lv_slider_create(lv_scr_act());
    lv_obj_align(slider, LV_ALIGN_BOTTOM_MID, 0, -25);
    lv_slider_set_value(slider, 50, LV_ANIM_OFF);
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

    // todo:
    // send proper PhantomHolder axis movement topics based on lvgl ui controls
    // X-Axis(mm)
    // Y-Axis(mm)
    // Rotate(degree)
    // Tilt(degree)

    // demo: publish simulated MQTT events
    if (millis() - timer >= interval) {
      timer = millis();
      counter++;
      mqtt.publish(topicSensor, String(counter), true, 2); // Retain & QoS 2
    }
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
