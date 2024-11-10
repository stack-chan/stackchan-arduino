// Copyright (c) 2022 Takao Akaki

#ifndef _STACKCHAN_SERVO_H_
#define _STACKCHAN_SERVO_H_

// コンパイル時にServoEasing.hppをIncludeしてくださいという警告が出ますが、hppにすると二重定義のリンクエラーが出ます。
// その対処でStackchan_servo.hはh, Stackchan_servo.cppはhppをincludeしています。
#define SUPPRESS_HPP_WARNING
#include <Ticker.h>
#include <ESP32Servo.h>
#include <ServoEasing.h>
#include <SCServo.h>
#include <M5Unified.h>
#include <Dynamixel2Arduino.h>

using namespace ControlTableItem;

#define SERIAL_EASE_DIVISION  5      // シリアルサーボのEasing分割数

enum Motion {
    nomove,    // 動かない
    greet,     // 挨拶
    laugh,     // 笑う
    nod,       // うなづく
    refuse,    // 首を横に振る（拒絶）
    test = 99,      // テスト用
};

enum ServoAxis {
    AXIS_X,
    AXIS_Y
};

enum ServoType {
    PWM,             // SG90 PWM
    SCS,             // Feetech SCS0009
    DYN_XL330,        // Dynamixel XL330
    RT_DYN_XL330     // Dynamixel XL330 on RT version stackchan
};

typedef struct ServoParam {
    int pin;                    // サーボのピン番号
    int16_t start_degree;              // 初期角度
    int16_t offset;                    // オフセット（90°からの+-）
    int16_t degree;                    // 角度
    uint32_t millis_for_move;         // 移動時間(msec)
    uint16_t lower_limit;              // サーボ角度の下限
    uint16_t upper_limit;              // サーボ角度の上限
} servo_param_s;


typedef struct  StackchanServo{
    servo_param_s servo[2];
} stackchan_servo_initial_param_s;


const float DXL_PROTOCOL_VERSION = 2.0f;

class StackchanSERVO {
    protected:
        ServoType _servo_type;
        SCSCL _sc;
        Dynamixel2Arduino _dxl;
        ServoEasing _servo_x;
        ServoEasing _servo_y;
        void attachServos();
        stackchan_servo_initial_param_s _init_param;
        bool _isMoving;
        int _last_degree_x;                              // 前回のX軸の角度
        int _last_degree_y;                              // 前回のY軸の角度
    public:
        StackchanSERVO();
        ~StackchanSERVO();
        
        float getPosition(int x);

        void begin(stackchan_servo_initial_param_s init_params);
        void begin(int servo_pin_x, int16_t start_degree_x, int16_t offset_x, 
                   int servo_pin_y, int16_t start_degree_y, int16_t offset_y,
                   ServoType servo_type=PWM);
        void moveX(int x, uint32_t millis_for_move = 0);
        void moveY(int y, uint32_t millis_for_move = 0);
        void moveXY(int x, int y, uint32_t millis_for_move);
        void moveX(servo_param_s servo_param_x);
        void moveY(servo_param_s servo_param_y);
        void moveXY(servo_param_s servo_param_x, servo_param_s servo_param_y);
        void motion(Motion motion_no);
        bool isMoving() { return _isMoving; }
};
#endif // _STACKCHAN_SERVO_H_