
#include "Stackchan_ex_config.h"

StackchanExConfig::StackchanExConfig() {};
StackchanExConfig::~StackchanExConfig() {};

void StackchanExConfig::loadExtendConfig(fs::FS& fs, const char *yaml_filename, uint32_t yaml_size) {
    M5_LOGI("----- StackchanExConfig::loadConfig:%s\n", yaml_filename);
    File file = fs.open(yaml_filename);
    if (file) {
        DynamicJsonDocument doc(yaml_size);
        auto err = deserializeYml( doc, file);
        if (err) {
            M5_LOGE("yaml file read error: %s\n", yaml_filename);
            M5_LOGE("error%s\n", err.c_str());
        }
        serializeJsonPretty(doc, Serial);
        setExtendSettings(doc);
    }
}

void StackchanExConfig::setExtendSettings(DynamicJsonDocument doc) {
    JsonObject app_param1 = doc["app_parameters1"];
    _ex_parameters.item1 = doc["app_parameters1"]["item1"].as<String>();            // 文字列はこのように記述
    _ex_parameters.item2 = app_param1["item2"];                          // 数値
    _ex_parameters.item3 = app_param1["item3"].as<bool>();              // True/False/0/1 
    JsonObject app_param2 = doc["app_parameters2"];
    _item4 = app_param2["item4"].as<String>();
    JsonArray list_str = app_param2["list_str"];
    _list_str_count = list_str.size();
    for (int i=0; i<_list_str_count; i++) {
        _list_str[i] = list_str[i].as<String>();
    }
    JsonArray list_num = app_param2["list_num"];
    _list_num_count = list_num.size();
    for (int i=0; i<_list_num_count; i++) {
        _list_num[i] = list_num[i];
    }
}


void StackchanExConfig::printExtParameters(void) {
    M5_LOGI("item1:%s", _ex_parameters.item1.c_str());
    M5_LOGI("item2:%d", _ex_parameters.item2);
    M5_LOGI("item3:%s", _ex_parameters.item3 ? "true":"false");
    M5_LOGI("item4:%s", _item4.c_str());
    for (int i=0; i<_list_str_count; i++) {
        M5_LOGI("list_str[%d]: %s", i, _list_str[i].c_str());
    }
    for (int i=0; i<_list_num_count; i++) {
        M5_LOGI("list_num[%d]: %d", i, _list_num[i]);
    }
}