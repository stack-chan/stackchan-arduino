# stackchan-arduino
Stack Chan Library for ArduinoFramework

# 使い方

platformio.iniのlib_depsに下記の記述を追加してください。(そのうち公開します。)

```
lib_deps = 
  https://github.com/mongonta0716/stackchan-arduino.git
lib_ldf_mode = deep                                       ; これを忘れるとリンクエラーになります。
```

# 設定ファイル
設定内容は各ファイルに詳しくコメントしています。
- [SC_BasicConfig.yaml](./data/yaml/SC_BasicConfig.yaml)<br>ｽﾀｯｸﾁｬﾝを起動するために必要な基本的なパラメータを集めています。
- [SC_SecConfig.yaml](./data/yaml/SC_SecConfig.yaml)<br>個人情報用の設定ファイルです。(WiFiやAPIKey等)
- [SC_ExConfig.yaml](./data/yaml/SC_ExConfig.yaml)<br>アプリケーション側で、設定を追加したいときに使用します。使い方はexamples/Advanceを参照してください。

