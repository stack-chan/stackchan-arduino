// Copyright (c) Takao Akaki
#include "Stackchan_servo.h"

#include <ServoEasing.hpp>

static long convertSCS0009Pos(int16_t degree) {
  //Serial.printf("Degree: %d\n", degree);
  return map(degree, 0, 300, 1023, 0);
}

static long convertDYNIXELXL330(int16_t degree) {
  M5_LOGI("Degree: %d\n", degree);
  
  long ret =  map(degree, 0, 360, 0, 4095);
  M5_LOGI("Position: %d\n", ret);
  return ret;
}

// シリアルサーボ用のEasing関数
float quadraticEaseInOut(float p) {
  //return p;
  if(p < 0.5)
	{
		return 2 * p * p;
	}
	else
	{
		return (-2 * p * p) + (4 * p) - 1;
	}
}


StackchanSERVO::StackchanSERVO() {}

StackchanSERVO::~StackchanSERVO() {}


void StackchanSERVO::attachServos() {
  if (_servo_type == ServoType::SCS) {
    // SCS0009
    Serial2.begin(1000000, SERIAL_8N1, _init_param.servo[AXIS_X].pin, _init_param.servo[AXIS_Y].pin);
    _sc.pSerial = &Serial2;
    _sc.WritePos(AXIS_X + 1, convertSCS0009Pos(_init_param.servo[AXIS_X].start_degree + _init_param.servo[AXIS_X].offset), 1000);
    _sc.WritePos(AXIS_Y + 1, convertSCS0009Pos(_init_param.servo[AXIS_Y].start_degree + _init_param.servo[AXIS_Y].offset), 1000);
    vTaskDelay(1000/portTICK_PERIOD_MS);

  } else if (_servo_type == ServoType::DYN_XL330) {
    M5_LOGI("DYN_XL330");
    Serial2.begin(1000000, SERIAL_8N1, _init_param.servo[AXIS_X].pin, _init_param.servo[AXIS_Y].pin);
    _dxl = Dynamixel2Arduino(Serial2);
    _dxl.begin(1000000);
    _dxl.setPortProtocolVersion(DXL_PROTOCOL_VERSION);
    _dxl.ping(AXIS_X + 1);
    _dxl.ping(AXIS_Y + 1);
    _dxl.setOperatingMode(AXIS_X + 1, OP_POSITION);
    _dxl.setOperatingMode(AXIS_Y + 1, OP_POSITION);
    _dxl.writeControlTableItem(DRIVE_MODE, AXIS_X + 1, 4);  // Velocityのパラメータを移動時間(msec)で指定するモードに変更
    _dxl.writeControlTableItem(DRIVE_MODE, AXIS_Y + 1, 4);  // Velocityのパラメータを移動時間(msec)で指定するモードに変更
    _dxl.torqueOn(AXIS_X + 1);
    delay(10); // ここでWaitを入れないと、Y(tilt)サーボが動かない場合がある。
    _dxl.torqueOn(AXIS_Y + 1);
    delay(100);
    _dxl.writeControlTableItem(PROFILE_VELOCITY, AXIS_X + 1, 1000);
    _dxl.writeControlTableItem(PROFILE_VELOCITY, AXIS_Y + 1, 1000);
    delay(100);
    _dxl.setGoalPosition(AXIS_X + 1, 2048);
    _dxl.setGoalPosition(AXIS_Y + 1, 3073);
    //_dxl.torqueOff(AXIS_X + 1);
    //_dxl.torqueOff(AXIS_Y + 1);
    
  } else {
    // SG90 PWM
    if (_servo_x.attach(_init_param.servo[AXIS_X].pin, 
                        _init_param.servo[AXIS_X].start_degree + _init_param.servo[AXIS_X].offset,
                        DEFAULT_MICROSECONDS_FOR_0_DEGREE,
                        DEFAULT_MICROSECONDS_FOR_180_DEGREE)) {
      Serial.print("Error attaching servo x");
    }
    if (_servo_y.attach(_init_param.servo[AXIS_Y].pin, 
                        _init_param.servo[AXIS_Y].start_degree + _init_param.servo[AXIS_Y].offset,
                        DEFAULT_MICROSECONDS_FOR_0_DEGREE,
                        DEFAULT_MICROSECONDS_FOR_180_DEGREE)) {
      Serial.print("Error attaching servo y");
    }

    _servo_x.setEasingType(EASE_QUADRATIC_IN_OUT);
    _servo_y.setEasingType(EASE_QUADRATIC_IN_OUT);
  }
  _last_degree_x = _init_param.servo[AXIS_X].start_degree;
  _last_degree_y = _init_param.servo[AXIS_Y].start_degree;
}

void StackchanSERVO::begin(stackchan_servo_initial_param_s init_param) {
  _init_param = init_param;
  attachServos();
}

void StackchanSERVO::begin(int servo_pin_x, int16_t start_degree_x, int16_t offset_x, 
                           int servo_pin_y, int16_t start_degree_y, int16_t offset_y,
                           ServoType servo_type) {
  _init_param.servo[AXIS_X].pin          = servo_pin_x;
  _init_param.servo[AXIS_X].start_degree = start_degree_x;
  _init_param.servo[AXIS_X].offset       = offset_x;
  _init_param.servo[AXIS_Y].pin          = servo_pin_y;
  _init_param.servo[AXIS_Y].start_degree = start_degree_y;
  _init_param.servo[AXIS_Y].offset       = offset_y;
  _servo_type = servo_type;
  attachServos();
}

void StackchanSERVO::moveX(int x, uint32_t millis_for_move) {
  if (_servo_type == SCS) {
    _sc.WritePos(AXIS_X + 1, convertSCS0009Pos(x + _init_param.servo[AXIS_X].offset), millis_for_move);
    _isMoving = true;
    vTaskDelay(millis_for_move/portTICK_PERIOD_MS);
    _isMoving = false;
  } else if (_servo_type == ServoType::DYN_XL330) {
    _dxl.writeControlTableItem(PROFILE_VELOCITY, AXIS_X + 1, millis_for_move);
    vTaskDelay(10/portTICK_PERIOD_MS);
    _dxl.setGoalPosition(AXIS_X + 1, convertDYNIXELXL330(x + _init_param.servo[AXIS_X].offset));
    vTaskDelay(10/portTICK_PERIOD_MS);
    _isMoving = true;
    vTaskDelay(millis_for_move/portTICK_PERIOD_MS);
    _isMoving = false;
  } else {
    if (millis_for_move == 0) {
      _servo_x.easeTo(x + _init_param.servo[AXIS_X].offset);
    } else {
      _servo_x.easeToD(x + _init_param.servo[AXIS_X].offset, millis_for_move);
    }
    _isMoving = true;
    synchronizeAllServosStartAndWaitForAllServosToStop();
    _isMoving = false;
  }
  _last_degree_x = x;
}

void StackchanSERVO::moveX(servo_param_s servo_param_x) {
  _init_param.servo[AXIS_X].offset = servo_param_x.offset;
  moveX(servo_param_x.degree, servo_param_x.millis_for_move);
}

void StackchanSERVO::moveY(int y, uint32_t millis_for_move) {
  if (_servo_type == ServoType::SCS) {
    _sc.WritePos(AXIS_Y + 1, convertSCS0009Pos(y + _init_param.servo[AXIS_Y].offset), millis_for_move);
    _isMoving = true;
    vTaskDelay(millis_for_move/portTICK_PERIOD_MS);
    _isMoving = false;
  } else if (_servo_type == ServoType::DYN_XL330) {
    _dxl.writeControlTableItem(PROFILE_VELOCITY, AXIS_Y + 1, millis_for_move);
    vTaskDelay(10/portTICK_PERIOD_MS);
    _dxl.setGoalPosition(AXIS_Y + 1, convertDYNIXELXL330(y + _init_param.servo[AXIS_Y].offset)); // RT版に合わせて+180°しています。
    vTaskDelay(10/portTICK_PERIOD_MS);
    _isMoving = true;
    vTaskDelay(millis_for_move/portTICK_PERIOD_MS);
    _isMoving = false;
  } else {
    if (millis_for_move == 0) {
      _servo_y.easeTo(y + _init_param.servo[AXIS_Y].offset);
    } else {
      _servo_y.easeToD(y + _init_param.servo[AXIS_Y].offset, millis_for_move);
    }
    _isMoving = true;
    synchronizeAllServosStartAndWaitForAllServosToStop();
    _isMoving = false;
  }
  _last_degree_y = y;
}

void StackchanSERVO::moveY(servo_param_s servo_param_y) {
  _init_param.servo[AXIS_Y].offset = servo_param_y.offset;
  moveY(servo_param_y.degree, servo_param_y.millis_for_move);
}
void StackchanSERVO::moveXY(int x, int y, uint32_t millis_for_move) {
  if (_servo_type == ServoType::SCS) {
    int increase_degree_x = x - _last_degree_x;
    int increase_degree_y = y - _last_degree_y;
    uint32_t division_time = millis_for_move / SERIAL_EASE_DIVISION;
    _isMoving = true;
    //M5_LOGI("SCS: %d, %d, %d", increase_degree_x, increase_degree_y, division_time);
    for (float f=0.0f; f<1.0f; f=f+(1.0f/SERIAL_EASE_DIVISION)) {
      int x_pos = _last_degree_x + increase_degree_x * quadraticEaseInOut(f);
      int y_pos = _last_degree_y + increase_degree_y * quadraticEaseInOut(f);
      _sc.WritePos(AXIS_X + 1, convertSCS0009Pos(x_pos + _init_param.servo[AXIS_X].offset), division_time);
      _sc.WritePos(AXIS_Y + 1, convertSCS0009Pos(y_pos + _init_param.servo[AXIS_Y].offset), division_time);
      //vTaskDelay(division_time);
    }
    _isMoving = false;
  } else if (_servo_type == ServoType::DYN_XL330) {
    _isMoving = true;
    _dxl.setGoalPosition(AXIS_X + 1, convertDYNIXELXL330(x + _init_param.servo[AXIS_X].offset)); 
    _dxl.setGoalPosition(AXIS_Y + 1, convertDYNIXELXL330(y + _init_param.servo[AXIS_Y].offset)); // RT版に合わせて+180°しています。
    _isMoving = false;
  } else {
    _servo_x.setEaseToD(x + _init_param.servo[AXIS_X].offset, millis_for_move);
    _servo_y.setEaseToD(y + _init_param.servo[AXIS_Y].offset, millis_for_move);
    _isMoving = true;
    synchronizeAllServosStartAndWaitForAllServosToStop();
    _isMoving = false;
  }
  _last_degree_x = x;
  _last_degree_y = y;
  //M5_LOGI("SCS: %d, %d", _last_degree_x, _last_degree_y);
}

void StackchanSERVO::moveXY(servo_param_s servo_param_x, servo_param_s servo_param_y) {
  if (_servo_type == ServoType::SCS) {
    _sc.WritePos(AXIS_X + 1, convertSCS0009Pos(servo_param_x.degree + servo_param_x.offset), servo_param_x.millis_for_move);
    _sc.WritePos(AXIS_Y + 1, convertSCS0009Pos(servo_param_y.degree + servo_param_y.offset), servo_param_y.millis_for_move);
    _isMoving = true;
    vTaskDelay(max(servo_param_x.millis_for_move, servo_param_y.millis_for_move)/portTICK_PERIOD_MS);
    _isMoving = false;
  } else if (_servo_type == ServoType::DYN_XL330) {
    _isMoving = true;
    _dxl.setGoalPosition(AXIS_X + 1, convertDYNIXELXL330(servo_param_x.degree + _init_param.servo[AXIS_X].offset)); 
    _dxl.setGoalPosition(AXIS_Y + 1, convertDYNIXELXL330(servo_param_y.degree + _init_param.servo[AXIS_Y].offset)); // RT版に合わせて+180°しています。
    _isMoving = false;
  } else {
    if (servo_param_x.degree != 0) {
      _servo_x.setEaseToD(servo_param_x.degree + servo_param_x.offset, servo_param_x.millis_for_move);
    }
    if (servo_param_y.degree != 0) {
      _servo_y.setEaseToD(servo_param_y.degree + servo_param_y.offset, servo_param_y.millis_for_move);
    }
    _isMoving = true;
    synchronizeAllServosStartAndWaitForAllServosToStop();
    _isMoving = false;
  }
  _last_degree_x = servo_param_x.degree;
  _last_degree_y = servo_param_y.degree;
}

void StackchanSERVO::motion(Motion motion_number) {
    if (motion_number == nomove) return; 
    moveXY(90, 75, 500);
    switch(motion_number) {
        case greet: 
            moveY(90, 1000);
            moveY(75, 1000);
            break;
        case laugh:
            for (int i=0; i<5; i++) {
                moveY(80, 500);
                moveY(60, 500);
            }
            break;
        case nod:
            for (int i=0; i<5; i++) {
                moveY(90, 1000);
                moveY(60, 1000);
            }
            break;
        case refuse:
            for (int i=0; i<2; i++) {
                moveX(70,  500);
                moveX(110, 500);
            }
            break;
        case test:
            moveX(45,  1000);
            moveX(135, 1000);
            moveX(90, 1000);
            moveY(50, 1000);
            moveY(90, 1000);
            break;
        default:
            Serial.printf("invalid motion number: %d\n", motion_number);
            break;
    }
    delay(1000);
    moveXY(_init_param.servo[AXIS_X].start_degree, _init_param.servo[AXIS_Y].degree, 1000);
}

