#ifndef STACKCHAN_SYSTEM_CONFIG_CPP
#define STACKCHAN_SYSTEM_CONFIG_CPP
#include "Stackchan_system_config.h"

StackchanSystemConfig::StackchanSystemConfig() {

};

StackchanSystemConfig::~StackchanSystemConfig() {

}

void StackchanSystemConfig::setDefaultParameters() {
    // 設定ファイルが存在しないときはデフォルトパラメータを使用します。
    // PWM サーボでPort.Aを想定しています。
    switch(M5.getBoard()) {
        case m5::board_t::board_M5StackCore2:
            _servo[AXIS_X].pin = 33;
            _servo[AXIS_Y].pin = 32;
            break;
        case m5::board_t::board_M5Stack:
            _servo[AXIS_X].pin = 22;
            _servo[AXIS_Y].pin = 21;
            break;
        case m5::board_t::board_M5StackCoreS3:
            _servo[AXIS_X].pin = 1;
            _servo[AXIS_Y].pin = 2;
        default:
            M5_LOGI("UnknownBoard:%d\n", M5.getBoard());
            _servo[AXIS_X].pin = 22;
            _servo[AXIS_Y].pin = 21;
            break;
    }
    _servo[AXIS_X].offset = 0;
    _servo[AXIS_X].lower_limit = 0;
    _servo[AXIS_X].upper_limit = 180;
    _servo[AXIS_X].start_degree = 90;
    _servo[AXIS_Y].offset = 0;
    _servo[AXIS_Y].lower_limit = 50;
    _servo[AXIS_Y].upper_limit = 90;
    _servo[AXIS_Y].start_degree = 90;
    _servo_interval[0].mode_name = "normal";
    _servo_interval[0].interval_min = 5000;
    _servo_interval[0].interval_max = 10000;
    _servo_interval[0].move_min = 500;
    _servo_interval[0].move_max = 1500;
    _servo_interval[1].mode_name = "sing_mode";
    _servo_interval[1].interval_min = 1000;
    _servo_interval[1].interval_max = 2000;
    _servo_interval[1].move_min = 500;
    _servo_interval[1].move_max = 1500;
    _mode_num = 2;
    _bluetooth.device_name = "M5Stack_BTSPK";
    _bluetooth.starting_state = true;
    _bluetooth.start_volume = 150;
    _auto_power_off_time = 0;
    _font_language_code = "JA";
    _lyrics[0] = "こんにちは";
    _lyrics[1] = "Hello";
    _lyrics[2] = "你好";
    _lyrics[3] = "Bonjour";
    _lyrics_num = 4;
    _led_lr = 0;
    _led_pin = -1;
    _takao_base = false;
    _servo_type = ServoType::PWM;
    _servo[AXIS_X].start_degree = 90;
    _servo[AXIS_Y].start_degree = 90;
    _secret_config_show = false;

}

void StackchanSystemConfig::loadConfig(fs::FS& fs, const char *app_yaml_filename, uint32_t app_yaml_filesize,
                                        const char* secret_yaml_filename, uint32_t secret_yaml_filesize,
                                        const char* basic_yaml_filename, uint32_t basic_yaml_filesize) {
    M5_LOGI("----- StackchanSystemConfig::loadConfig:%s\n", basic_yaml_filename);
    M5_LOGI("----- app_yaml_filename:%s\n", app_yaml_filename);
    fs::File file = fs.open(basic_yaml_filename);
    DynamicJsonDocument doc(basic_yaml_filesize);
    if (file) {
        DeserializationError err = deserializeYml(doc, file);
        if (err) {
            M5_LOGI("yaml file read error: %s\n", basic_yaml_filename);
            M5_LOGI("error%s\n", err.c_str());
        }
        serializeJsonPretty(doc, Serial);
        setSystemConfig(doc);
    } else {
        Serial.println("ConfigFile Not Found. Default Parameters used.");
        // YAMLファイルが見つからない場合はデフォルト値を利用します。
        setDefaultParameters();
        basicConfigNotFoundCallback();
    }
    if (secret_yaml_filesize > 0) {
        loadSecretConfig(fs, secret_yaml_filename, secret_yaml_filesize);
    }
    if (app_yaml_filesize > 0) {
        loadExtendConfig(fs, app_yaml_filename, app_yaml_filesize);
    }
    printAllParameters();
}

void StackchanSystemConfig::loadSecretConfig(fs::FS& fs, const char* yaml_filename, uint32_t yaml_size) {
    M5_LOGI("----- StackchanSecretConfig::loadConfig:%s\n", yaml_filename);
    File file = fs.open(yaml_filename);
    if (file) {
        DynamicJsonDocument doc(yaml_size);
        auto err = deserializeYml( doc, file);
        if (err) {
            M5_LOGE("yaml file read error: %s\n", yaml_filename);
            M5_LOGE("error%s\n", err.c_str());
        }
        else{
            setSecretConfig(doc);
        }

        if (_secret_config_show) {
            // 個人的な情報をログに表示する。
            M5_LOGI("=======================================================================================");
            M5_LOGI("下記の情報は公開してはいけません。(The following information must not be disclosed.)");
            M5_LOGI("");
            serializeJsonPretty(doc, Serial);
            M5_LOGI("");
            printSecretParameters();
            M5_LOGI("");
            M5_LOGI("ここまでの情報は公開してはいけません。(No information should be disclosed so far.)");
            M5_LOGI("=======================================================================================");
        }
    }
    else{
        secretConfigNotFoundCallback();
    }
}

void StackchanSystemConfig::setSystemConfig(DynamicJsonDocument doc) {
    JsonObject servo = doc["servo"];
    _servo[AXIS_X].pin = servo["pin"]["x"];
    _servo[AXIS_Y].pin = servo["pin"]["y"];

    _servo[AXIS_X].offset = servo["offset"]["x"];
    _servo[AXIS_Y].offset = servo["offset"]["y"];

    _servo[AXIS_X].start_degree = servo["center"]["x"];
    _servo[AXIS_Y].start_degree = servo["center"]["y"];

    _servo[AXIS_X].lower_limit = servo["lower_limit"]["x"];
    _servo[AXIS_X].upper_limit = servo["upper_limit"]["x"];
    _servo[AXIS_Y].lower_limit = servo["lower_limit"]["y"];
    _servo[AXIS_Y].upper_limit = servo["upper_limit"]["y"];
    int i = 0;
    for (JsonPair servo_speed_item : servo["speed"].as<JsonObject>()) {
        _servo_interval[i].mode_name = servo_speed_item.key().c_str();
        _servo_interval[i].interval_min = servo_speed_item.value()["interval_min"];
        _servo_interval[i].interval_max = servo_speed_item.value()["interval_max"];
        _servo_interval[i].move_min = servo_speed_item.value()["move_min"];
        _servo_interval[i].move_max = servo_speed_item.value()["move_max"];
        i++;
    }
    _mode_num = i;

    _bluetooth.device_name = doc["bluetooth"]["device_name"].as<String>();
    _bluetooth.starting_state = doc["bluetooth"]["starting_state"];//.as<bool>();
    _bluetooth.start_volume = doc["bluetooth"]["start_volume"];

    _auto_power_off_time = doc["auto_power_off_time"];
    _font_language_code = doc["balloon"]["font_language"].as<String>();
    
    JsonArray balloon_lyrics = doc["balloon"]["lyrics"];
        
    _lyrics_num = balloon_lyrics.size();
    M5_LOGI("lyrics_num:%d\n", _lyrics_num);
    for (int j=0;j<_lyrics_num;j++) {
        _lyrics[j] = balloon_lyrics[j].as<String>();
    }
    _led_lr = doc["led_lr"];
    _led_pin = doc["led_pin"];
    _takao_base = doc["takao_base"];
    _servo_type_str = doc["servo_type"].as<String>();
    if (_servo_type_str.indexOf("SCS") != -1) {
        // SCS0009
        _servo_type = ServoType::SCS;
    } else if (_servo_type_str.indexOf("DYN_XL330") != -1) {
        // Dynamixel XL330
        _servo_type = ServoType::DYN_XL330;
    } else  {
        // PWMサーボ
        _servo_type = ServoType::PWM;
    }
    _secret_config_show     = doc["secret_config_show"].as<bool>(); 
    
}

void StackchanSystemConfig::setSecretConfig(DynamicJsonDocument doc) {

    _secret_config.wifi_info.ssid     = doc["wifi"]["ssid"].as<String>();
    _secret_config.wifi_info.password = doc["wifi"]["password"].as<String>();
    
    _secret_config.api_key.stt        = doc["apikey"]["stt"].as<String>();
    _secret_config.api_key.ai_service = doc["apikey"]["aiservice"].as<String>();
    _secret_config.api_key.tts        = doc["apikey"]["tts"].as<String>();

}

const lgfx::IFont* StackchanSystemConfig::getFont() {
    if (_font_language_code.compareTo("JA")) {
        return &fonts::efontJA_16;
    } else if (_font_language_code.compareTo("CN")) {
        return &fonts::efontCN_16;
    } else {
        M5_LOGI("FontCodeError:%s\n", _font_language_code);
        return &fonts::Font0;
    }
} 

void StackchanSystemConfig::printAllParameters() {
    M5_LOGI("servo:pin_x:%d", _servo[AXIS_X].pin);
    M5_LOGI("servo:pin_y:%d", _servo[AXIS_Y].pin);
    M5_LOGI("servo:offset_x:%d", _servo[AXIS_X].offset);
    M5_LOGI("servo:offset_y:%d", _servo[AXIS_Y].offset);
    M5_LOGI("servo.start_degree_x:%d", _servo[AXIS_X].start_degree);
    M5_LOGI("servo.start_degree_y:%d", _servo[AXIS_Y].start_degree);
    M5_LOGI("servo.lower_limit_x:%d", _servo[AXIS_X].lower_limit);
    M5_LOGI("servo.lower_limit_y:%d", _servo[AXIS_Y].lower_limit);
    M5_LOGI("servo.upper_limit_x:%d", _servo[AXIS_X].upper_limit);
    M5_LOGI("servo.upper_limit_y:%d", _servo[AXIS_Y].upper_limit);
    for (int i=0;i<_mode_num;i++) {
        M5_LOGI("mode:%s", _servo_interval[i].mode_name);
        M5_LOGI("interval_min:%d", _servo_interval[i].interval_min);
        M5_LOGI("interval_max:%d", _servo_interval[i].interval_max);
        M5_LOGI("move_min:%d", _servo_interval[i].move_min);
        M5_LOGI("move_max:%d", _servo_interval[i].move_max);
    }
    M5_LOGI("mode_num:%d", _mode_num);
    M5_LOGI("Bluetooth_device_name:%s", _bluetooth.device_name.c_str());
    M5_LOGI("Bluetooth_starting_state:%s", _bluetooth.starting_state ? "true":"false");
    M5_LOGI("Bluetooth_start_volume:%d", _bluetooth.start_volume);
    M5_LOGI("auto_power_off_time:%d", _auto_power_off_time);
    M5_LOGI("font_language:%s", _font_language_code);
    for (int i=0;i<_lyrics_num;i++) {
        M5_LOGI("lyrics:%d:%s", i, _lyrics[i].c_str());
    }
    M5_LOGI("led_lr:%d", _led_lr);
    M5_LOGI("led_pin:%d", _led_pin);
    M5_LOGI("use takao_base:%s", _takao_base ? "true":"false");
    M5_LOGI("ServoTypeStr:%s", _servo_type_str.c_str());
    M5_LOGI("ServoType: %d", _servo_type);
    M5_LOGI("secret_config_show:%s", _secret_config_show ? "true":"false");

    printExtParameters();
}

void StackchanSystemConfig::printSecretParameters() {
    M5_LOGI("wifi_ssid: %s", _secret_config.wifi_info.ssid.c_str());
    M5_LOGI("wifi_passws: %s", _secret_config.wifi_info.password.c_str());
    M5_LOGI("apikey_stt: %s", _secret_config.api_key.stt.c_str());
    M5_LOGI("apikey_aiservice: %s", _secret_config.api_key.ai_service.c_str());
    M5_LOGI("apikey_tts: %s", _secret_config.api_key.tts.c_str());
}
void StackchanSystemConfig::loadExtendConfig(fs::FS& fs, const char* filename, uint32_t yaml_size) {  };
void StackchanSystemConfig::setExtendSettings(DynamicJsonDocument doc) {  };
void StackchanSystemConfig::printExtParameters(void) {};

void StackchanSystemConfig::basicConfigNotFoundCallback(void) {};
void StackchanSystemConfig::secretConfigNotFoundCallback(void) {};

#endif