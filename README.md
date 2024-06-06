# stackchan-arduino
Stack Chan Library for ArduinoFramework

# 使い方

platformio.iniのlib_depsに下記の記述を追加してください。(そのうち公開します。)

```
lib_deps = 
  m5stack/M5Unified@0.1.16
  arminjo/ServoEasing@^2.4.0
  madhephaestus/ESP32Servo @ 0.13.0
  bblanchon/ArduinoJson @ ^6
  tobozo/YAMLDuino
  https://github.com/mongonta0716/stackchan-arduino.git
  https://github.com/mongonta0716/SCServo
lib_ldf_mode = deep                                       ; これを忘れるとリンクエラーになります。
```

# 設定ファイル
- [SC_BasicConfig.yaml](./data/yaml/SC_BasicConfig.yaml)<br>ｽﾀｯｸﾁｬﾝを起動するために必要な基本的なパラメータを集めています。
- [SC_SecConfig.yaml](./data/yaml/SC_SecConfig.yaml)<br>個人情報用の設定ファイルです。(WiFiやAPIKey等)
- [SC_ExConfig.yaml](./data/yaml/SC_ExConfig.yaml)<br>アプリケーション側で、設定を追加したいときに使用します。使い方はexamples/Advanceを参照してください。

