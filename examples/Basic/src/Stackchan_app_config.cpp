
#include "Stackchan_app_config.h"

namespace {

String argOr(WebServer& server, const char* name, const String& fallback = "") {
    return server.hasArg(name) ? server.arg(name) : fallback;
}

String yamlQuote(const String& value) {
    String quoted = "\"";
    for (size_t i = 0; i < value.length(); ++i) {
        const char c = value[i];
        if (c == '"' || c == '\\') {
            quoted += '\\';
        }
        if (c == '\n') {
            quoted += "\\n";
        } else if (c != '\r') {
            quoted += c;
        }
    }
    quoted += '"';
    return quoted;
}

String jsonArrayToLines(JsonArrayConst array) {
    String lines;
    for (JsonVariantConst item : array) {
        if (lines.length() > 0) {
            lines += '\n';
        }
        if (item.is<const char*>()) {
            lines += item.as<String>();
        } else if (item.is<bool>()) {
            lines += item.as<bool>() ? "true" : "false";
        } else if (!item.isNull()) {
            lines += item.as<String>();
        }
    }
    return lines;
}

String arrayOrCurrentList(JsonArrayConst array, String* values, uint8_t count) {
    String lines = jsonArrayToLines(array);
    if (lines.length() > 0) {
        return lines;
    }

    for (uint8_t i = 0; i < count; ++i) {
        if (i > 0) {
            lines += '\n';
        }
        lines += values[i];
    }
    return lines;
}

String arrayOrCurrentList(JsonArrayConst array, int* values, uint8_t count) {
    String lines = jsonArrayToLines(array);
    if (lines.length() > 0) {
        return lines;
    }

    for (uint8_t i = 0; i < count; ++i) {
        if (i > 0) {
            lines += '\n';
        }
        lines += String(values[i]);
    }
    return lines;
}

void appendStringListYaml(String& yaml, const String& lines) {
    String normalized = lines;
    normalized.replace("\r\n", "\n");
    normalized.replace("\r", "\n");

    int start = 0;
    bool wrote = false;
    while (start <= static_cast<int>(normalized.length())) {
        int end = normalized.indexOf('\n', start);
        if (end < 0) {
            end = normalized.length();
        }

        String line = normalized.substring(start, end);
        if (line.length() > 0) {
            yaml += F("    - ");
            yaml += yamlQuote(line);
            yaml += '\n';
            wrote = true;
        }

        start = end + 1;
        if (end == static_cast<int>(normalized.length())) {
            break;
        }
    }

    if (!wrote) {
        yaml += F("    []\n");
    }
}

void appendNumberListYaml(String& yaml, const String& lines) {
    String normalized = lines;
    normalized.replace("\r\n", "\n");
    normalized.replace("\r", "\n");
    normalized.replace(",", "\n");

    int start = 0;
    bool wrote = false;
    while (start <= static_cast<int>(normalized.length())) {
        int end = normalized.indexOf('\n', start);
        if (end < 0) {
            end = normalized.length();
        }

        String line = normalized.substring(start, end);
        line.trim();
        if (line.length() > 0) {
            yaml += F("    - ");
            yaml += line;
            yaml += '\n';
            wrote = true;
        }

        start = end + 1;
        if (end == static_cast<int>(normalized.length())) {
            break;
        }
    }

    if (!wrote) {
        yaml += F("    []\n");
    }
}

}  // namespace

StackchanAppConfig::StackchanAppConfig() {};
StackchanAppConfig::~StackchanAppConfig() {};

void StackchanAppConfig::loadExtendConfig(fs::FS& fs, const char *yaml_filename, uint32_t yaml_size) {
    (void)yaml_size;
    M5_LOGI("----- StackchanAppConfig::loadConfig:%s\n", yaml_filename);
    File file = fs.open(yaml_filename);
    if (file) {
        JsonDocument doc;
        auto err = deserializeYml( doc, file);
        if (err) {
            M5_LOGE("yaml file read error: %s\n", yaml_filename);
            M5_LOGE("error%s\n", err.c_str());
        }
        serializeJsonPretty(doc, Serial);
        setExtendSettings(doc);
    }
}

void StackchanAppConfig::setExtendSettings(JsonDocument& doc) {
    JsonObject app_param1 = doc["app_parameters1"];
    _app_parameters.item1 = doc["app_parameters1"]["item1"].as<String>();            // 文字列はこのように記述
    _app_parameters.item2 = app_param1["item2"];                          // 数値
    _app_parameters.item3 = app_param1["item3"].as<bool>();              // True/False/0/1 
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

void StackchanAppConfig::appendSetupModeExtendHtml(String& html, JsonDocument& doc, const String& raw_yaml) {
    (void)raw_yaml;
    html += F("<fieldset><legend>SC_AppConfig.yaml</legend><div class=\"grid\">");
    appendSetupModeTextarea(html, F("app_parameters1.item1"), "ext_item1",
                            setupModeJsonValueToString(doc["app_parameters1"]["item1"], _app_parameters.item1));
    appendSetupModeTextarea(html, F("app_parameters1.item2"), "ext_item2",
                            setupModeJsonValueToString(doc["app_parameters1"]["item2"], String(_app_parameters.item2)));
    appendSetupModeTextarea(html, F("app_parameters1.item3"), "ext_item3",
                            setupModeJsonValueToString(doc["app_parameters1"]["item3"], _app_parameters.item3 ? "true" : "false"));
    appendSetupModeTextarea(html, F("app_parameters2.item4"), "ext_item4",
                            setupModeJsonValueToString(doc["app_parameters2"]["item4"], _item4));
    html += F("</div>");
    appendSetupModeTextarea(html, F("app_parameters2.list_str"), "ext_list_str",
                            arrayOrCurrentList(doc["app_parameters2"]["list_str"].as<JsonArrayConst>(),
                                               _list_str, _list_str_count),
                            5);
    appendSetupModeTextarea(html, F("app_parameters2.list_num"), "ext_list_num",
                            arrayOrCurrentList(doc["app_parameters2"]["list_num"].as<JsonArrayConst>(),
                                               _list_num, _list_num_count),
                            5);
    html += F("</fieldset>");
}

bool StackchanAppConfig::buildSetupModeExtendYaml(WebServer& server, String& yaml, String& error) {
    yaml.reserve(512);
    yaml += F("app_parameters1:\n");
    yaml += F("  item1: ");
    yaml += setupModeYamlQuote(argOr(server, "ext_item1"));
    yaml += '\n';
    yaml += F("  item2: ");
    yaml += argOr(server, "ext_item2", "0");
    yaml += '\n';
    yaml += F("  item3: ");
    yaml += argOr(server, "ext_item3", "false");
    yaml += F("\n\n");
    yaml += F("app_parameters2:\n");
    yaml += F("  item4: ");
    yaml += setupModeYamlQuote(argOr(server, "ext_item4"));
    yaml += '\n';
    yaml += F("  list_str:\n");
    appendStringListYaml(yaml, argOr(server, "ext_list_str"));
    yaml += F("  list_num:\n");
    appendNumberListYaml(yaml, argOr(server, "ext_list_num"));

    JsonDocument doc;
    DeserializationError err = deserializeYml(doc, yaml.c_str());
    if (err) {
        error = String("Extend YAML parse error: ") + err.c_str();
        return false;
    }
    return true;
}

void StackchanAppConfig::printExtParameters(void) {
    M5_LOGI("item1:%s", _app_parameters.item1.c_str());
    M5_LOGI("item2:%d", _app_parameters.item2);
    M5_LOGI("item3:%s", _app_parameters.item3 ? "true":"false");
    M5_LOGI("item4:%s", _item4.c_str());
    for (int i=0; i<_list_str_count; i++) {
        M5_LOGI("list_str[%d]: %s", i, _list_str[i].c_str());
    }
    for (int i=0; i<_list_num_count; i++) {
        M5_LOGI("list_num[%d]: %d", i, _list_num[i]);
    }
}
