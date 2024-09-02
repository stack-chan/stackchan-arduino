# stackchan-arduino
Stack Chan Library for ArduinoFramework

[日本語](./README.md) | English

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


# 設定ファイル
設定内容は各ファイルに詳しくコメントしています。
- [SC_BasicConfig.yaml](./data/yaml/SC_BasicConfig.yaml)<br>ｽﾀｯｸﾁｬﾝを起動するために必要な基本的なパラメータを集めています。
- [SC_SecConfig.yaml](./data/yaml/SC_SecConfig.yaml)<br>個人情報用の設定ファイルです。(WiFiやAPIKey等)
- [SC_ExConfig.yaml](./data/yaml/SC_ExConfig.yaml)<br>アプリケーション側で、設定を追加したいときに使用します。使い方は[Folder](./examples/Basic/)のソースを参照してください。

# LICENCE

[MIT](./LICENSE)

# Author

[Takao Akaki](https://github.com/mongonta0716)