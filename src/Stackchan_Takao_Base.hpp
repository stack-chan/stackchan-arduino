#ifndef STACKCHAN_TAKAO_BASE_HPP
#define STACKCHAN_TAKAO_BASE_HPP


#include <M5Unified.h>
#include "Stackchan_servo.h"

enum PowerStatus {
    SidePower,
    BackPower,
    Battery
};

PowerStatus checkTakaoBasePowerStatus(m5::Power_Class* power, int16_t battery_threshold = 3200) {
  if (!power->getExtOutput() && power->getBatteryCurrent() < 0) {
    // TakaoBase使用時(ExtOutput=falseの場合)、バッテリー駆動の時はtrueに切り替える。 
    // 切り替えないとサーボが動きません。
    power->setExtOutput(true);
    power->setLed(0);
    return PowerStatus::Battery;  
  }

  if (power->getExtOutput() && (power->getBatteryCurrent() >= 0) {
    // M5StackのUSB-C端子から給電されている状態。
    power->setLed(80);
    return PowerStatus::SidePower;
  }

  if (power->getBatteryLevel() < battery_threshold) {
    // Batteryの電圧が閾値よりも下がったときの処理
    power->setExtOutput(false); // 後側のUSB-Cの給電状態を把握するためにfalseにする必要があります。
    if (power->getVBUSVoltage() > 3000) {
      // 後ろから給電されている状態。
      power->setLed(80);
      return PowerStatus::BackPower; 
    } else {
      // 給電されていない場合は電源OFF
      M5.Power.powerOff();
    }
  }
  return PowerStatus::BackPower;
}

#endif // STACKCHAN_TAKAO_BASE_HPP