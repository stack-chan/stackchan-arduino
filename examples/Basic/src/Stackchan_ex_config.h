#ifndef __STACKCHAN_EX_CONFIG_H__
#define __STACKCHAN_EX_CONFIG_H__

#include <Stackchan_system_config.h>

typedef struct ExConfig {
    String item1;
    int    item2;
    bool   item3;
} ex_config_s;


// StackchanSystemConfigを継承します。
class StackchanExConfig : public StackchanSystemConfig
{
    protected:
        ex_config_s _ex_parameters;
        String  _item4;
        uint8_t _list_str_count;
        String  _list_str[10];
        uint8_t _list_num_count;
        int     _list_num[10];

    public:
        StackchanExConfig();
        ~StackchanExConfig();

        void loadExtendConfig(fs::FS& fs, const char *yaml_filename, uint32_t yaml_size) override;
        void setExtendSettings(DynamicJsonDocument doc) override;
        void printExtParameters(void) override;
        ex_config_s getExConfig() { return _ex_parameters; }
        uint8_t getListStrCount() { return _list_str_count; }
        String getListStr(uint8_t no) { return _list_str[no]; }
        uint8_t getListNumCount() { return _list_num_count; }
        int    getListNum(uint8_t no) { return _list_num[no]; }
        void setItem1(String item1) { _ex_parameters.item1 = item1; }
        void setItem2(int    item2) { _ex_parameters.item2 = item2; }
        void setItem3(bool   item3) { _ex_parameters.item3 = item3; }
        void setExConfig(ex_config_s config) { _ex_parameters = config; } 
};

#endif // __STACKCHAN_EX_CONFIG_H__