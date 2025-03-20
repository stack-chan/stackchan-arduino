# Stackchan Arduino Library API ドキュメント

## 概要
Stackchan Arduino Library は、M5Stack デバイス上で Stack-chan を制御するためのライブラリです。このライブラリは、サーボモーターの制御、設定ファイルの読み込み、Bluetooth 設定などの機能を提供します。

---

## クラス一覧

### 1. `StackchanSystemConfig`
設定ファイルを読み込み、システム設定を管理するクラス。

#### メソッド
- **`loadConfig(fs::FS& fs, const char *app_yaml_filename, uint32_t app_yaml_filesize, const char* secret_yaml_filename, uint32_t secret_yaml_filesize, const char* basic_yaml_filename, uint32_t basic_yaml_filesize)`**
  - 設定ファイルを読み込み、システム設定を初期化します。
  - 引数:
    - `fs`: ファイルシステムオブジェクト。
    - `app_yaml_filename`: アプリケーション設定ファイルのパス。
    - `app_yaml_filesize`: アプリケーション設定ファイルのサイズ。
    - `secret_yaml_filename`: 個人情報設定ファイルのパス。
    - `secret_yaml_filesize`: 個人情報設定ファイルのサイズ。
    - `basic_yaml_filename`: 基本設定ファイルのパス。
    - `basic_yaml_filesize`: 基本設定ファイルのサイズ。

- **`printAllParameters()`**
  - 全ての設定パラメータをログに出力します。

- **`getServoInfo(uint8_t servo_axis_no)`**
  - 指定したサーボ軸の情報を取得します。

- **`getWiFiSetting()`**
  - WiFi 設定を取得します。

- **`getAPISetting()`**
  - API キー設定を取得します。

---

### 2. `StackchanSERVO`
サーボモーターを制御するクラス。

#### メソッド
- **`begin(stackchan_servo_initial_param_s init_params)`**
  - サーボモーターを初期化します。

- **`moveX(int x, uint32_t millis_for_move = 0)`**
  - X 軸のサーボを指定した角度に移動します。
  - 引数:
    - `x`: 移動する角度。
    - `millis_for_move`: 移動にかける時間（ミリ秒）。

- **`moveY(int y, uint32_t millis_for_move = 0)`**
  - Y 軸のサーボを指定した角度に移動します。

- **`moveXY(int x, int y, uint32_t millis_for_move)`**
  - X 軸と Y 軸のサーボを同時に移動します。

- **`motion(Motion motion_no)`**
  - プリセットされたモーションを実行します。
  - 引数:
    - `motion_no`: 実行するモーションの種類（例: `greet`, `laugh`）。

---

### 3. `StackchanExConfig`
`StackchanSystemConfig` を拡張したクラスで、アプリケーション固有の設定を管理します。

#### メソッド
- **`loadExtendConfig(fs::FS& fs, const char *yaml_filename, uint32_t yaml_size)`**
  - 拡張設定ファイルを読み込みます。

- **`setExtendSettings(DynamicJsonDocument doc)`**
  - 拡張設定を適用します。

- **`printExtParameters()`**
  - 拡張設定のパラメータをログに出力します。

---

## 列挙型

### `Motion`
サーボモーターのプリセットモーションを定義します。
- `nomove`: 動かない。
- `greet`: 挨拶。
- `laugh`: 笑う。
- `nod`: うなずく。
- `refuse`: 拒否する。
- `test`: テスト用。

### `ServoType`
サーボモーターの種類を定義します。
- `PWM`: PWM サーボ (例: SG90)。
- `SCS`: シリアルサーボ (例: Feetech SCS0009)。
- `DYN_XL330`: Dynamixel XL330。
- `RT_DYN_XL330`: RT バージョンの Dynamixel XL330。

---

## 設定ファイル

### SC_BasicConfig.yaml
基本設定ファイル。サーボのピン番号、初期位置、可動範囲、速度などを定義します。

### SC_SecConfig.yaml
個人情報設定ファイル。WiFi の SSID やパスワード、API キーを定義します。

### SC_ExtConfig.yaml
拡張設定ファイル。アプリケーション固有の設定を記述します。

---

## 使用例

### サーボの初期化と移動
```cpp
#include <M5Unified.h>
#include "Stackchan_servo.h"

StackchanSERVO servo;

void setup() {
  M5.begin();
  servo.begin(33, 90, 0, 32, 90, 0, ServoType::PWM); // サーボの初期化
  servo.moveX(45, 1000); // X軸を45度に移動（1秒かけて）
  servo.moveY(60, 1000); // Y軸を60度に移動（1秒かけて）
  servo.moveXY(90, 90, 1000); // 正面を向く(1秒かけて)
}

void loop() {
  // 繰り返し処理
}
```

---
