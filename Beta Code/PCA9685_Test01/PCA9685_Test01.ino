#include <Wire.h>                     // IIC库
#include <Adafruit_PWMServoDriver.h>  // PCA9685驱动板库

#define frequency 50     // 定义PWM频率为50Hz
#define channelOn 4095   // 定义PWM通道全开
#define channelOff 0     // 定义PWM通道全关
#define channelCount 16  // 定义通道数量为16

// 创建对象,用于控制PCA9685驱动板,I2C地址为0x40
Adafruit_PWMServoDriver PCA9685 = Adafruit_PWMServoDriver(0x40);

void setup() {
  Serial.begin(9600);  // 初始化串口通信,波特率为9600

  PCA9685.begin();                // 初始化PCA9685驱动板
  PCA9685.setPWMFreq(frequency);  // 设置PCA9685的PWM频率

  // 初始化所有通道为关
  for (uint8_t i = 0; i < channelCount; i++) {
    PCA9685.setPWM(i, channelOff, channelOff);
  }
  Serial.println(F("\n系统初始化完成"));  // 在串口监视器中打印初始化完成信息
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();  // 去除字符串中的空格
    Serial.println(input);
    int delimiterIndex = input.indexOf(',');
    if (delimiterIndex > 0) {
      int ID = input.substring(0, delimiterIndex).toInt();
      int data = input.substring(delimiterIndex + 1).toInt();
      if (ID >= 0 && data >= 0) {
        PCA9685.setPWM(ID - 1, 0, data);
      }
    } else {
      Serial.println(F("指令格式错误,正确格式:ID,data"));
      return;
    }
  }
}
