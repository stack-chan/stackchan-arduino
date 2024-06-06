
#include "AIStackchan_config.h"

AIStackchanConfig::AIStackchanConfig() {};
AIStackchanConfig::~AIStackchanConfig() {};

void AIStackchanConfig::loadExtendConfig(fs::FS& fs, const char *yaml_filename, uint32_t yaml_size) {
    M5_LOGI("----- StackchanAIConfig::loadConfig:%s\n", yaml_filename);
    File file = fs.open(yaml_filename);
    if (file) {
        DynamicJsonDocument doc(yaml_size);
        auto err = deserializeYml( doc, file);
        if (err) {
            M5_LOGI("yaml file read error: %s\n", yaml_filename);
            M5_LOGI("error%s\n", err.c_str());
        }
        serializeJsonPretty(doc, Serial);
        setExtendSettings(doc);
    }
}

void AIStackchanConfig::setExtendSettings(DynamicJsonDocument doc) {
    JsonObject apikeys = doc["apikeys"];
    _ai_parameters.stt       = apikeys["stt_apikey"].as<String>();
    _ai_parameters.aiservice = apikeys["aiservice_apikey"].as<String>();
    _ai_parameters.tts       = apikeys["tts_apikey"].as<String>(); 
}

void AIStackchanConfig::printExtParameters(void) {
    M5_LOGI("stt_apikey:%s\n", _ai_parameters.stt.c_str());
    M5_LOGI("aiservice_apikey:%s\n", _ai_parameters.aiservice.c_str());
    M5_LOGI("tts_apikey:%s\n", _ai_parameters.tts.c_str());
}