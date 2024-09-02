# stackchan-arduino
Stack Chan Library for ArduinoFramework
Arduino Frameworkで利用できるStackchan用のライブラリです。

日本語 | [English](./README_en.md)

# 使い方

platformio.iniのlib_depsに下記の記述を追加してください。

```
lib_deps = 
  stack-chan/stackchan-arduino@^0.0.1
lib_ldf_mode = deep                                       ; これを忘れるとリンクエラーになります。
```

# 機能
- 設定ファイルの読み込み機能<br>後述の設定ファイルを読み込んで設定を変更することができます。<br>Stackchan_system_config.hをincludeします。
- サーボのコントロール機能<br>現在3種類のサーボを設定で切り替えてコントロールすることが可能です。<br>Stackchan_servo.hをincludeします。
  - PWM<br>SG90等のPWMサーボ
  - SCS<br>Feetech SCS0009 シリアルサーボ
  - DYN_XL330<br>Dynamixel XL330-M288-T
- 【未実装】Stack-chan_Takao_Baseの状態監視<br>Stack-chan_Takao_Baseを使うための便利な機能を実装予定です。

# 使い方
- [Basic](./examples/Basic/)パラメータの読み込み方法はこちらを見てください。
- [stack-chan-tester](https://github.com/mongonta0716/stack-chan-tester)サーボと

# 設定ファイル
設定内容は各ファイルに詳しくコメントしています。YAML形式で記述してください。<br>※ YAMLを利用しているのはコメントを記述できる点が便利だからです。
- [SC_BasicConfig.yaml](./data/yaml/SC_BasicConfig.yaml)<br>ｽﾀｯｸﾁｬﾝを起動するために必要な基本的なパラメータを集めています。
- [SC_SecConfig.yaml](./data/yaml/SC_SecConfig.yaml)<br>個人情報用の設定ファイルです。(WiFiやAPIKey等)
- [SC_ExConfig.yaml](./data/yaml/SC_ExConfig.yaml)<br>アプリケーション側で、設定を追加したいときに使用します。使い方は[Folder](./examples/Basic/)のソースを参照してください。

# 必要なライブラリ

詳細のバージョンは[library.json](./library.json)のdependenciesを参照してください。ArduinoIDEで利用する場合はバージョンを最新にしてしまうとビルドできなくなったり、動かなくなる場合があります。PlatformIOを推奨します。
 - [M5Unified](https://github.com/m5stack/M5Unified)
 - [ESP32Servo](https://github.com/madhephaestus/ESP32Servo)
 - [ServoEasing](https://github.com/ArminJo/ServoEasing)
 - [ArduinoJson](https://github.com/bblanchon/ArduinoJson)
 - [Dynamixel2Arduino](https://github.com/ROBOTIS-GIT/Dynamixel2Arduino)
 - [YAMLDuino](https://github.com/tobozo/YAMLDuino)
 - [SCServo](https://github.com/mongonta0716/SCServo)


# ｽﾀｯｸﾁｬﾝについて

ｽﾀｯｸﾁｬﾝは[ししかわさん](https://github.com/stack-chan)が公開しているオープンソースのプロジェクトです。
https://github.com/stack-chan/stack-chan

# Credit
- [meganetaaan](https://github.com/meganetaaan)
- [lovyan03](https://github.com/lovyan03/LovyanGFX)
- [robo8080](https://github.com/robo8080)
- [tobozo](https://github.com/tobozo)

# LICENCE

[MIT](./LICENSE)

# Author

[Takao Akaki](https://github.com/mongonta0716)