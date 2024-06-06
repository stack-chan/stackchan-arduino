#ifndef __AISTACKCHAN_CONFIG_H__
#define __AISTACKCHAN_CONFIG_H__

#include <Stackchan_system_config.h>

typedef struct AIConfig {
    String stt;
    String aiservice;
    String tts;
} ai_config_s;

class AIStackchanConfig : public StackchanSystemConfig
{
    protected:
        ai_config_s _ai_parameters;

    public:
        AIStackchanConfig();
        ~AIStackchanConfig();
        void loadExtendConfig(fs::FS& fs, const char *yaml_filename, uint32_t yaml_size) override;
        void setExtendSettings(DynamicJsonDocument doc) override;
        void printExtParameters(void) override;
        ai_config_s getAIConfig() { return _ai_parameters; }
        void setSTTApikey(String apikey) { _ai_parameters.stt = apikey; }
        void setAIServiceApikey(String apikey) { _ai_parameters.aiservice = apikey; }
        void setTTSApikey(String apikey) { _ai_parameters.tts = apikey; }
};

#endif // __AISTACKCHAN_SYSTEM_CONFIG_H__