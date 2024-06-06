#include <Arduino.h>
#include <M5Unified.h>
#include <SD.h>
#include "Stackchan_ex_config.h"

StackchanExConfig config;

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);
  M5.Log.setLogLevel(m5::log_target_display, ESP_LOG_INFO);
  M5.Log.setEnableColor(m5::log_target_serial, false);
  SD.begin(GPIO_NUM_4, SPI, 25000000);
  delay(2000);
  config.loadConfig(SD, "/yaml/SC_BasicConfig.yaml");

  config.getWiFiSetting();

}

void loop() {
  // put your main code here, to run repeatedly:
}

