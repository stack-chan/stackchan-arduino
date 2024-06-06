#include <Arduino.h>
#include <M5Unified.h>
#include <SD.h>
#include "Stackchan_ex_config.h"
#include <Stackchan_servo.h>
#include <Avatar.h>

using namespace m5avatar;

Avatar avatar;

StackchanSERVO servo;
StackchanExConfig system_config;

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);
  M5.Log.setLogLevel(m5::log_target_display, ESP_LOG_INFO);
  M5.Log.setEnableColor(m5::log_target_serial, false);
  SD.begin(GPIO_NUM_4, SPI, 25000000);
  delay(2000);
  system_config.loadConfig(SD, "/yaml/SC_BasicConfig.yaml");
  
  // servo
  servo.begin(system_config.getServoInfo(AXIS_X)->pin, system_config.getServoInfo(AXIS_X)->start_degree,
              system_config.getServoInfo(AXIS_X)->offset,
              system_config.getServoInfo(AXIS_Y)->pin, system_config.getServoInfo(AXIS_Y)->start_degree,
              system_config.getServoInfo(AXIS_Y)->offset,
              (ServoType)system_config.getServoType());
  delay(2000);
  avatar.init();
  
  servo_interval_s* servo_interval = system_config.getServoInterval(AvatarMode::NORMAL); // ノーマルモード時のサーボインターバル情報を取得
  servo_interval_s* servo_interval_sing = system_config.getServoInterval(AvatarMode::SINGING); // 歌っているときのサーボインターバル情報を取得

  // wifi
  wifi_s*     wifi_info = system_config.getWiFiSetting();
  api_keys_s* api_key   = system_config.getAPISetting();

}

void loop() {
  // put your main code here, to run repeatedly:
}
