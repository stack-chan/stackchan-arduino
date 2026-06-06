#ifndef STACKCHAN_SYSTEM_CONFIG_CPP
#define STACKCHAN_SYSTEM_CONFIG_CPP
#include "Stackchan_system_config.h"

#include <DNSServer.h>
#include <SPIFFS.h>
#include <WebServer.h>
#include <WiFi.h>

namespace {

static const byte DNS_PORT = 53;
static const IPAddress SETUP_MODE_IP(192, 168, 0, 4);
static const IPAddress SETUP_MODE_GATEWAY(192, 168, 0, 4);
static const IPAddress SETUP_MODE_SUBNET(255, 255, 255, 0);

String htmlEscape(const String& value) {
    String escaped;
    escaped.reserve(value.length() + 16);
    for (size_t i = 0; i < value.length(); ++i) {
        const char c = value[i];
        switch (c) {
            case '&': escaped += F("&amp;"); break;
            case '<': escaped += F("&lt;"); break;
            case '>': escaped += F("&gt;"); break;
            case '"': escaped += F("&quot;"); break;
            case '\'': escaped += F("&#39;"); break;
            default: escaped += c; break;
        }
    }
    return escaped;
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

String jsonValueToString(JsonVariantConst value, const String& fallback = "") {
    if (value.isNull()) {
        return fallback;
    }
    if (value.is<const char*>()) {
        return value.as<String>();
    }
    if (value.is<bool>()) {
        return value.as<bool>() ? "true" : "false";
    }
    if (value.is<int>()) {
        return String(value.as<int>());
    }
    if (value.is<unsigned int>()) {
        return String(value.as<unsigned int>());
    }
    if (value.is<long>()) {
        return String(value.as<long>());
    }
    if (value.is<unsigned long>()) {
        return String(value.as<unsigned long>());
    }
    if (value.is<float>() || value.is<double>()) {
        return String(value.as<double>());
    }
    return fallback;
}

String jsonArrayToLines(JsonArrayConst array) {
    String lines;
    for (JsonVariantConst item : array) {
        if (lines.length() > 0) {
            lines += '\n';
        }
        lines += jsonValueToString(item);
    }
    return lines;
}

bool hasYamlContent(const String& content) {
    bool line_has_content = false;
    for (size_t i = 0; i < content.length(); ++i) {
        const char c = content[i];
        if (c == '#') {
            while (i < content.length() && content[i] != '\n') {
                ++i;
            }
            line_has_content = false;
            continue;
        }
        if (c == '\n' || c == '\r') {
            line_has_content = false;
            continue;
        }
        if (c == ' ' || c == '\t') {
            continue;
        }
        line_has_content = true;
        break;
    }
    return line_has_content;
}

String readTextFile(fs::FS& fs, const char* path) {
    File file = fs.open(path, FILE_READ);
    if (!file) {
        return "";
    }

    String content;
    content.reserve(file.size() + 1);
    while (file.available()) {
        content += static_cast<char>(file.read());
    }
    file.close();
    return content;
}

bool writeTextFile(fs::FS& fs, const char* path, const String& content) {
    fs.mkdir("/yaml");
    fs.mkdir("/SC_APP");
    File file = fs.open(path, FILE_WRITE);
    if (!file) {
        return false;
    }
    size_t written = file.print(content);
    file.close();
    return written == content.length();
}

void loadYamlFile(fs::FS& fs, const char* path, JsonDocument& doc) {
    String content = readTextFile(fs, path);
    if (!hasYamlContent(content)) {
        return;
    }
    DeserializationError err = deserializeYml(doc, content.c_str());
    if (err) {
        M5_LOGW("setupMode yaml parse error: %s: %s", path, err.c_str());
    }
}

void appendTextarea(String& html, const __FlashStringHelper* label, const char* name, const String& value, uint8_t rows = 1) {
    html += F("<label><span>");
    html += label;
    html += F("</span><textarea name=\"");
    html += name;
    html += F("\" rows=\"");
    html += rows;
    html += F("\">");
    html += htmlEscape(value);
    html += F("</textarea></label>");
}

String setupModeSsid() {
    uint16_t suffix = static_cast<uint16_t>(ESP.getEfuseMac() & 0xFFFF);
    char suffixText[5];
    snprintf(suffixText, sizeof(suffixText), "%04X", suffix);
    return String("Stackchan-") + suffixText;
}

String argOr(WebServer& server, const char* name, const String& fallback = "") {
    return server.hasArg(name) ? server.arg(name) : fallback;
}

String buildBasicYaml(WebServer& server) {
    String yaml;
    yaml.reserve(2200);
    yaml += F("servo:\n");
    yaml += F("  pin:\n");
    yaml += F("    x: "); yaml += argOr(server, "servo_pin_x", "7"); yaml += '\n';
    yaml += F("    y: "); yaml += argOr(server, "servo_pin_y", "6"); yaml += '\n';
    yaml += F("  offset:\n");
    yaml += F("    x: "); yaml += argOr(server, "servo_offset_x", "0"); yaml += '\n';
    yaml += F("    y: "); yaml += argOr(server, "servo_offset_y", "0"); yaml += '\n';
    yaml += F("  center:\n");
    yaml += F("    x: "); yaml += argOr(server, "servo_center_x", "150"); yaml += '\n';
    yaml += F("    y: "); yaml += argOr(server, "servo_center_y", "90"); yaml += '\n';
    yaml += F("  lower_limit:\n");
    yaml += F("    x: "); yaml += argOr(server, "servo_lower_x", "0"); yaml += '\n';
    yaml += F("    y: "); yaml += argOr(server, "servo_lower_y", "0"); yaml += '\n';
    yaml += F("  upper_limit:\n");
    yaml += F("    x: "); yaml += argOr(server, "servo_upper_x", "300"); yaml += '\n';
    yaml += F("    y: "); yaml += argOr(server, "servo_upper_y", "90"); yaml += '\n';
    yaml += F("  speed:\n");
    yaml += F("    normal_mode:\n");
    yaml += F("      interval_min: "); yaml += argOr(server, "speed_normal_interval_min", "3000"); yaml += '\n';
    yaml += F("      interval_max: "); yaml += argOr(server, "speed_normal_interval_max", "6000"); yaml += '\n';
    yaml += F("      move_min: "); yaml += argOr(server, "speed_normal_move_min", "500"); yaml += '\n';
    yaml += F("      move_max: "); yaml += argOr(server, "speed_normal_move_max", "1500"); yaml += '\n';
    yaml += F("    sing_mode:\n");
    yaml += F("      interval_min: "); yaml += argOr(server, "speed_sing_interval_min", "500"); yaml += '\n';
    yaml += F("      interval_max: "); yaml += argOr(server, "speed_sing_interval_max", "1000"); yaml += '\n';
    yaml += F("      move_min: "); yaml += argOr(server, "speed_sing_move_min", "500"); yaml += '\n';
    yaml += F("      move_max: "); yaml += argOr(server, "speed_sing_move_max", "1000"); yaml += '\n';
    yaml += F("takao_base: "); yaml += argOr(server, "takao_base", "false"); yaml += '\n';
    yaml += F("servo_type: "); yaml += yamlQuote(argOr(server, "servo_type", "M5_SCS")); yaml += '\n';
    yaml += F("bluetooth:\n");
    yaml += F("  device_name: "); yaml += yamlQuote(argOr(server, "bluetooth_device_name", "M5StackBTSPK")); yaml += '\n';
    yaml += F("  starting_state: "); yaml += argOr(server, "bluetooth_starting_state", "false"); yaml += '\n';
    yaml += F("  start_volume: "); yaml += argOr(server, "bluetooth_start_volume", "100"); yaml += '\n';
    yaml += F("auto_power_off_time: "); yaml += argOr(server, "auto_power_off_time", "0"); yaml += '\n';
    yaml += F("balloon:\n");
    yaml += F("  font_language: "); yaml += yamlQuote(argOr(server, "balloon_font_language", "JA")); yaml += '\n';
    yaml += F("  lyrics:\n");

    String lyrics = argOr(server, "balloon_lyrics");
    lyrics.replace("\r\n", "\n");
    lyrics.replace("\r", "\n");
    int start = 0;
    bool wroteLyric = false;
    while (start <= static_cast<int>(lyrics.length())) {
        int end = lyrics.indexOf('\n', start);
        if (end < 0) {
            end = lyrics.length();
        }
        String line = lyrics.substring(start, end);
        if (line.length() > 0) {
            yaml += F("  - ");
            yaml += yamlQuote(line);
            yaml += '\n';
            wroteLyric = true;
        }
        start = end + 1;
        if (end == static_cast<int>(lyrics.length())) {
            break;
        }
    }
    if (!wroteLyric) {
        yaml += F("  - \"\"\n");
    }

    yaml += F("led_lr: "); yaml += argOr(server, "led_lr", "0"); yaml += '\n';
    yaml += F("led_pin: "); yaml += argOr(server, "led_pin", "15"); yaml += '\n';
    yaml += F("extend_config_filename: "); yaml += yamlQuote(argOr(server, "extend_config_filename")); yaml += '\n';
    yaml += F("extend_config_filesize: "); yaml += argOr(server, "extend_config_filesize", "0"); yaml += '\n';
    yaml += F("secret_config_filename: "); yaml += yamlQuote(argOr(server, "secret_config_filename")); yaml += '\n';
    yaml += F("secret_config_filesize: "); yaml += argOr(server, "secret_config_filesize", "0"); yaml += '\n';
    yaml += F("secret_info_show: "); yaml += argOr(server, "secret_info_show", "false"); yaml += '\n';
    return yaml;
}

String buildSecretYaml(WebServer& server) {
    String yaml;
    yaml.reserve(320);
    yaml += F("wifi:\n");
    yaml += F("  ssid: "); yaml += yamlQuote(argOr(server, "wifi_ssid")); yaml += '\n';
    yaml += F("  password: "); yaml += yamlQuote(argOr(server, "wifi_password")); yaml += '\n';
    yaml += F("apikey:\n");
    yaml += F("  stt: "); yaml += yamlQuote(argOr(server, "apikey_stt")); yaml += '\n';
    yaml += F("  aiservice: "); yaml += yamlQuote(argOr(server, "apikey_aiservice")); yaml += '\n';
    yaml += F("  tts: "); yaml += yamlQuote(argOr(server, "apikey_tts")); yaml += '\n';
    return yaml;
}

bool validateYaml(const String& yaml, String& error) {
    JsonDocument doc;
    DeserializationError err = deserializeYml(doc, yaml.c_str());
    if (err) {
        error = err.c_str();
        return false;
    }
    return true;
}

}  // namespace

void StackchanSystemConfig::appendSetupModeTextarea(String& html, const __FlashStringHelper* label,
                                                    const char* name, const String& value, uint8_t rows) {
    appendTextarea(html, label, name, value, rows);
}

String StackchanSystemConfig::setupModeYamlQuote(const String& value) {
    return yamlQuote(value);
}

String StackchanSystemConfig::setupModeJsonValueToString(JsonVariantConst value, const String& fallback) {
    return jsonValueToString(value, fallback);
}

String StackchanSystemConfig::setupModeJsonArrayToLines(JsonArrayConst array) {
    return jsonArrayToLines(array);
}

void StackchanSystemConfig::appendSetupModeExtendHtml(String& html, JsonDocument&, const String& raw_yaml) {
    html += F("<fieldset><legend>Extend YAML</legend>");
    appendSetupModeTextarea(html, F("app yaml"), "app_yaml", raw_yaml, 12);
    html += F("</fieldset>");
}

bool StackchanSystemConfig::buildSetupModeExtendYaml(WebServer& server, String& yaml, String& error) {
    yaml = argOr(server, "app_yaml");
    if (yaml.length() > 0 && !validateYaml(yaml, error)) {
        error = "Extend YAML parse error: " + error;
        return false;
    }
    return true;
}

String StackchanSystemConfig::renderSetupPage(const String& message, const char* appYamlFilename,
                                              const char* secretYamlFilename, const char* basicYamlFilename) {
    JsonDocument basicDoc;
    JsonDocument secretDoc;
    JsonDocument appDoc;
    loadYamlFile(SPIFFS, basicYamlFilename, basicDoc);
    loadYamlFile(SPIFFS, secretYamlFilename, secretDoc);
    loadYamlFile(SPIFFS, appYamlFilename, appDoc);
    String appYaml = readTextFile(SPIFFS, appYamlFilename);

    String html;
    html.reserve(14000);
    html += F("<!doctype html><html lang=\"ja\"><head><meta charset=\"utf-8\">");
    html += F("<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">");
    html += F("<title>Stackchan Setup</title><style>");
    html += F("body{font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',sans-serif;margin:0;background:#f6f7f9;color:#1d232a}");
    html += F("header{position:sticky;top:0;background:#fff;border-bottom:1px solid #d8dde4;padding:14px 16px;z-index:1}");
    html += F("main{max-width:920px;margin:0 auto;padding:16px}");
    html += F("fieldset{border:1px solid #d8dde4;background:#fff;margin:0 0 16px;padding:16px}");
    html += F("legend{font-weight:700;padding:0 6px}");
    html += F("label{display:block;margin:12px 0}span{display:block;font-size:13px;font-weight:600;margin-bottom:4px}");
    html += F("textarea{box-sizing:border-box;width:100%;min-height:38px;padding:8px;border:1px solid #b8c0cc;border-radius:4px;font:14px/1.45 ui-monospace,SFMono-Regular,Menlo,monospace}");
    html += F(".grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(190px,1fr));gap:10px 14px}");
    html += F(".message{background:#e8f5ee;border:1px solid #9bd0b5;padding:10px;margin:0 0 14px}");
    html += F(".error{background:#fff0f0;border-color:#d99}");
    html += F("button{appearance:none;border:0;background:#174ea6;color:#fff;padding:12px 18px;border-radius:4px;font-weight:700}");
    html += F("</style></head><body><header><strong>Stackchan Setup</strong><div>AP: ");
    html += htmlEscape(WiFi.softAPSSID());
    html += F(" / http://192.168.0.4/</div></header><main>");
    if (message.length() > 0) {
        html += F("<div class=\"message");
        if (message.startsWith("ERROR:")) {
            html += F(" error");
        }
        html += F("\">");
        html += htmlEscape(message);
        html += F("</div>");
    }
    html += F("<form method=\"post\" action=\"/save\">");

    html += F("<fieldset><legend>Servo</legend><div class=\"grid\">");
    appendTextarea(html, F("servo.pin.x"), "servo_pin_x", jsonValueToString(basicDoc["servo"]["pin"]["x"], "7"));
    appendTextarea(html, F("servo.pin.y"), "servo_pin_y", jsonValueToString(basicDoc["servo"]["pin"]["y"], "6"));
    appendTextarea(html, F("servo.offset.x"), "servo_offset_x", jsonValueToString(basicDoc["servo"]["offset"]["x"], "0"));
    appendTextarea(html, F("servo.offset.y"), "servo_offset_y", jsonValueToString(basicDoc["servo"]["offset"]["y"], "0"));
    appendTextarea(html, F("servo.center.x"), "servo_center_x", jsonValueToString(basicDoc["servo"]["center"]["x"], "150"));
    appendTextarea(html, F("servo.center.y"), "servo_center_y", jsonValueToString(basicDoc["servo"]["center"]["y"], "90"));
    appendTextarea(html, F("servo.lower_limit.x"), "servo_lower_x", jsonValueToString(basicDoc["servo"]["lower_limit"]["x"], "0"));
    appendTextarea(html, F("servo.lower_limit.y"), "servo_lower_y", jsonValueToString(basicDoc["servo"]["lower_limit"]["y"], "0"));
    appendTextarea(html, F("servo.upper_limit.x"), "servo_upper_x", jsonValueToString(basicDoc["servo"]["upper_limit"]["x"], "300"));
    appendTextarea(html, F("servo.upper_limit.y"), "servo_upper_y", jsonValueToString(basicDoc["servo"]["upper_limit"]["y"], "90"));
    appendTextarea(html, F("servo_type"), "servo_type", jsonValueToString(basicDoc["servo_type"], "M5_SCS"));
    appendTextarea(html, F("takao_base"), "takao_base", jsonValueToString(basicDoc["takao_base"], "false"));
    html += F("</div></fieldset>");

    html += F("<fieldset><legend>Servo Speed</legend><div class=\"grid\">");
    appendTextarea(html, F("normal.interval_min"), "speed_normal_interval_min", jsonValueToString(basicDoc["servo"]["speed"]["normal_mode"]["interval_min"], "3000"));
    appendTextarea(html, F("normal.interval_max"), "speed_normal_interval_max", jsonValueToString(basicDoc["servo"]["speed"]["normal_mode"]["interval_max"], "6000"));
    appendTextarea(html, F("normal.move_min"), "speed_normal_move_min", jsonValueToString(basicDoc["servo"]["speed"]["normal_mode"]["move_min"], "500"));
    appendTextarea(html, F("normal.move_max"), "speed_normal_move_max", jsonValueToString(basicDoc["servo"]["speed"]["normal_mode"]["move_max"], "1500"));
    appendTextarea(html, F("sing.interval_min"), "speed_sing_interval_min", jsonValueToString(basicDoc["servo"]["speed"]["sing_mode"]["interval_min"], "500"));
    appendTextarea(html, F("sing.interval_max"), "speed_sing_interval_max", jsonValueToString(basicDoc["servo"]["speed"]["sing_mode"]["interval_max"], "1000"));
    appendTextarea(html, F("sing.move_min"), "speed_sing_move_min", jsonValueToString(basicDoc["servo"]["speed"]["sing_mode"]["move_min"], "500"));
    appendTextarea(html, F("sing.move_max"), "speed_sing_move_max", jsonValueToString(basicDoc["servo"]["speed"]["sing_mode"]["move_max"], "1000"));
    html += F("</div></fieldset>");

    html += F("<fieldset><legend>Application</legend><div class=\"grid\">");
    appendTextarea(html, F("bluetooth.device_name"), "bluetooth_device_name", jsonValueToString(basicDoc["bluetooth"]["device_name"], "M5StackBTSPK"));
    appendTextarea(html, F("bluetooth.starting_state"), "bluetooth_starting_state", jsonValueToString(basicDoc["bluetooth"]["starting_state"], "false"));
    appendTextarea(html, F("bluetooth.start_volume"), "bluetooth_start_volume", jsonValueToString(basicDoc["bluetooth"]["start_volume"], "100"));
    appendTextarea(html, F("auto_power_off_time"), "auto_power_off_time", jsonValueToString(basicDoc["auto_power_off_time"], "0"));
    appendTextarea(html, F("balloon.font_language"), "balloon_font_language", jsonValueToString(basicDoc["balloon"]["font_language"], "JA"));
    appendTextarea(html, F("led_lr"), "led_lr", jsonValueToString(basicDoc["led_lr"], "0"));
    appendTextarea(html, F("led_pin"), "led_pin", jsonValueToString(basicDoc["led_pin"], "15"));
    appendTextarea(html, F("extend_config_filename"), "extend_config_filename", jsonValueToString(basicDoc["extend_config_filename"]));
    appendTextarea(html, F("extend_config_filesize"), "extend_config_filesize", jsonValueToString(basicDoc["extend_config_filesize"], "0"));
    appendTextarea(html, F("secret_config_filename"), "secret_config_filename", jsonValueToString(basicDoc["secret_config_filename"]));
    appendTextarea(html, F("secret_config_filesize"), "secret_config_filesize", jsonValueToString(basicDoc["secret_config_filesize"], "0"));
    appendTextarea(html, F("secret_info_show"), "secret_info_show", jsonValueToString(basicDoc["secret_info_show"], jsonValueToString(basicDoc["secret_config_show"], "false")));
    html += F("</div>");
    appendTextarea(html, F("balloon.lyrics"), "balloon_lyrics", jsonArrayToLines(basicDoc["balloon"]["lyrics"].as<JsonArrayConst>()), 8);
    html += F("</fieldset>");

    html += F("<fieldset><legend>Secret</legend><div class=\"grid\">");
    appendTextarea(html, F("wifi.ssid"), "wifi_ssid", jsonValueToString(secretDoc["wifi"]["ssid"]));
    appendTextarea(html, F("wifi.password"), "wifi_password", jsonValueToString(secretDoc["wifi"]["password"]));
    appendTextarea(html, F("apikey.stt"), "apikey_stt", jsonValueToString(secretDoc["apikey"]["stt"]));
    appendTextarea(html, F("apikey.aiservice"), "apikey_aiservice", jsonValueToString(secretDoc["apikey"]["aiservice"]));
    appendTextarea(html, F("apikey.tts"), "apikey_tts", jsonValueToString(secretDoc["apikey"]["tts"]));
    html += F("</div></fieldset>");

    appendSetupModeExtendHtml(html, appDoc, appYaml);
    html += F("<button type=\"submit\">保存</button></form></main></body></html>");
    return html;
}

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
        case m5::board_t::board_M5StackCoreS3SE:
            _servo[AXIS_X].pin = 1;
            _servo[AXIS_Y].pin = 2;
            break;
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
    (void)basic_yaml_filesize;
    M5_LOGI("----- StackchanSystemConfig::loadConfig:%s\n", basic_yaml_filename);
    M5_LOGI("----- app_yaml_filename:%s\n", app_yaml_filename);
    fs::File file = fs.open(basic_yaml_filename);
    JsonDocument doc;
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
    (void)yaml_size;
    M5_LOGI("----- StackchanSecretConfig::loadConfig:%s\n", yaml_filename);
    File file = fs.open(yaml_filename);
    if (file) {
        JsonDocument doc;
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

void StackchanSystemConfig::setSystemConfig(JsonDocument& doc) {
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
    if (_servo_type_str.indexOf("M5_SCS") != -1) {
        // SCS0009
        _servo_type = ServoType::M5_SCS;
    } else if (_servo_type_str.indexOf("RT_DYN_XL330") != -1) {
        // Dynamixel XL330 for RT Version
        _servo_type = ServoType::RT_DYN_XL330;
    } else if (_servo_type_str.indexOf("DYN_XL330") != -1) {
        // Dynamixel XL330
        _servo_type = ServoType::DYN_XL330;
    } else if (_servo_type_str.indexOf("SCS") != -1) {
        // M5Stack用SCS0009 (PY32IOExpander使用)
        _servo_type = ServoType::SCS; 
    
    } else {
        // PWMサーボ
        _servo_type = ServoType::PWM;
    }
    if (!doc["secret_config_show"].isNull()) {
        _secret_config_show = doc["secret_config_show"].as<bool>();
    } else {
        _secret_config_show = doc["secret_info_show"].as<bool>();
    }
    
}

void StackchanSystemConfig::setupMode(const char* app_yaml_filename,
                                      const char* secret_yaml_filename,
                                      const char* basic_yaml_filename) {
    if (!SPIFFS.begin(true)) {
        M5_LOGE("SPIFFS begin failed.");
        return;
    }

    String ssid = setupModeSsid();
    WiFi.disconnect(true);
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(SETUP_MODE_IP, SETUP_MODE_GATEWAY, SETUP_MODE_SUBNET);
    WiFi.softAP(ssid.c_str());

    DNSServer dnsServer;
    WebServer server(80);
    dnsServer.start(DNS_PORT, "*", SETUP_MODE_IP);

    M5_LOGI("Setup mode started. SSID:%s IP:%s", ssid.c_str(), SETUP_MODE_IP.toString().c_str());
    if (M5.Display.width() > 0) {
        M5.Display.clear();
        M5.Display.setTextSize(2);
        M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
        M5.Display.setTextWrap(true);
        M5.Display.setCursor(0, 0);
        M5.Display.println("Setup Mode");
        M5.Display.println();
        M5.Display.println("WiFi Access Point");
        M5.Display.printf("SSID:\n%s\n\n", ssid.c_str());
        M5.Display.println("Open browser:");
        M5.Display.println("http://192.168.0.4/");
    }

    String message;
    server.on("/", HTTP_GET, [&]() {
        server.send(200, "text/html; charset=utf-8",
                    renderSetupPage(message, app_yaml_filename, secret_yaml_filename, basic_yaml_filename));
    });

    server.on("/save", HTTP_POST, [&]() {
        String basicYaml = buildBasicYaml(server);
        String secretYaml = buildSecretYaml(server);
        String appYaml;
        String error;

        if (!validateYaml(basicYaml, error)) {
            message = "ERROR: Basic YAML parse error: " + error;
        } else if (!validateYaml(secretYaml, error)) {
            message = "ERROR: Secret YAML parse error: " + error;
        } else if (!buildSetupModeExtendYaml(server, appYaml, error)) {
            message = "ERROR: " + error;
        } else if (!writeTextFile(SPIFFS, basic_yaml_filename, basicYaml)) {
            message = "ERROR: Basic YAML save failed.";
        } else if (!writeTextFile(SPIFFS, secret_yaml_filename, secretYaml)) {
            message = "ERROR: Secret YAML save failed.";
        } else if (appYaml.length() > 0 && !writeTextFile(SPIFFS, app_yaml_filename, appYaml)) {
            message = "ERROR: Extend YAML save failed.";
        } else {
            message = "Saved to SPIFFS.";
        }

        server.send(200, "text/html; charset=utf-8",
                    renderSetupPage(message, app_yaml_filename, secret_yaml_filename, basic_yaml_filename));
    });

    server.on("/generate_204", HTTP_GET, [&]() {
        server.sendHeader("Location", String("http://") + SETUP_MODE_IP.toString(), true);
        server.send(302, "text/plain", "");
    });

    server.on("/hotspot-detect.html", HTTP_GET, [&]() {
        server.send(200, "text/html; charset=utf-8",
                    renderSetupPage(message, app_yaml_filename, secret_yaml_filename, basic_yaml_filename));
    });

    server.onNotFound([&]() {
        server.send(200, "text/html; charset=utf-8",
                    renderSetupPage(message, app_yaml_filename, secret_yaml_filename, basic_yaml_filename));
    });

    server.begin();
    while (true) {
        dnsServer.processNextRequest();
        server.handleClient();
        M5.update();
        delay(2);
    }
}

void StackchanSystemConfig::setSecretConfig(JsonDocument& doc) {

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
        M5_LOGI("FontCodeError:%s\n", _font_language_code.c_str());
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
    M5_LOGI("font_language:%s", _font_language_code.c_str());
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
void StackchanSystemConfig::loadExtendConfig(fs::FS&, const char*, uint32_t) {  };
void StackchanSystemConfig::setExtendSettings(JsonDocument&) {  };
void StackchanSystemConfig::printExtParameters(void) {};

void StackchanSystemConfig::basicConfigNotFoundCallback(void) {};
void StackchanSystemConfig::secretConfigNotFoundCallback(void) {};

#endif
