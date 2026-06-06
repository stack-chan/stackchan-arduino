#ifndef __STACKCHAN_APP_CONFIG_H__
#define __STACKCHAN_APP_CONFIG_H__

#include <Stackchan_system_config.h>

typedef struct AppConfig {
    String item1;
    int    item2;
    bool   item3;
} app_config_s;


// StackchanSystemConfigを継承します。
class StackchanAppConfig : public StackchanSystemConfig
{
    protected:
        app_config_s _app_parameters;
        String  _item4;
        uint8_t _list_str_count;
        String  _list_str[10];
        uint8_t _list_num_count;
        int     _list_num[10];
        void appendSetupModeExtendHtml(String& html, JsonDocument& doc, const String& raw_yaml) override;
        bool buildSetupModeExtendYaml(WebServer& server, String& yaml, String& error) override;

    public:
        StackchanAppConfig();
        ~StackchanAppConfig();

        void loadExtendConfig(fs::FS& fs, const char *yaml_filename, uint32_t yaml_size) override;
        void setExtendSettings(JsonDocument& doc) override;
        void printExtParameters(void) override;
        app_config_s getAppConfig() { return _app_parameters; }
        uint8_t getListStrCount() { return _list_str_count; }
        String getListStr(uint8_t no) { return _list_str[no]; }
        uint8_t getListNumCount() { return _list_num_count; }
        int    getListNum(uint8_t no) { return _list_num[no]; }
        void setItem1(String item1) { _app_parameters.item1 = item1; }
        void setItem2(int    item2) { _app_parameters.item2 = item2; }
        void setItem3(bool   item3) { _app_parameters.item3 = item3; }
        void setAppConfig(app_config_s config) { _app_parameters = config; } 
};

#endif // __STACKCHAN_APP_CONFIG_H__
