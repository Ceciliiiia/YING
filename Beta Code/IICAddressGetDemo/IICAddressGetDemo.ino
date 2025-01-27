#include <Wire.h>  // 引入 Wire 库,用于 I2C 通信

void setup() {
  Wire.begin();                              // 初始化 I2C 通信
  Serial.begin(9600);                        // 初始化串口通信,设置波特率为 9600
  Serial.println("\nI2C Scanner working!");  // 向串口输出欢迎信息
}

void loop() {
  byte error, address;            // 定义错误码和地址变量
  int nDevices;                   // 记录找到的 I2C 设备数量
  Serial.println("Scanning...");  // 向串口输出扫描开始的提示信息
  nDevices = 0;                   // 初始化找到的设备数量为 0

  // 扫描所有可能的 I2C 地址（范围从 1 到 126）
  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);  // 开始与指定地址的 I2C 设备进行通信
    error = Wire.endTransmission();   // 结束通信并获取返回的错误码

    // 检查错误码
    if (error == 0) {                                  // 如果返回的错误码是 0,说明成功找到设备
      Serial.print("I2C device found at address 0x");  // 输出找到的设备地址
      if (address < 16)                                // 如果地址小于 16,输出前导零
        Serial.print("0");
      Serial.print(address, HEX);                   // 以十六进制格式输出地址
      Serial.println("  !");                        // 输出感叹号
      nDevices++;                                   // 设备数量加 1
    } else if (error == 4) {                        // 如果返回的错误码是 4,说明发生了未知错误
      Serial.print("Unknown error at address 0x");  // 输出未知错误的信息
      if (address < 16)                             // 如果地址小于 16,输出前导零
        Serial.print("0");
      Serial.println(address, HEX);  // 以十六进制格式输出地址
    }
  }

  // 根据找到的设备数量输出相应的信息
  if (nDevices == 0) {
    Serial.println("No I2C devices found");  // 如果没有找到设备,输出提示信息
  } else {
    Serial.println("done");  // 如果找到了设备,输出完成的信息
  }

  delay(5000);  // 延迟 5 秒,防止过于频繁的扫描
}
