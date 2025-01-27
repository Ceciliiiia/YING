#include <Wire.h>                      // IIC库
#include <Adafruit_PWMServoDriver.h>   // PCA9685驱动板库

// 创建对象,用于控制PCA9685驱动板,I2C地址为0x40
Adafruit_PWMServoDriver PCA9685 = Adafruit_PWMServoDriver(0x40);

#define frequency 50  // 定义PWM频率为1000Hz

int receiveData = 0;        // 定义变量用于接收串口数据
bool receiveState = false;  // 定义变量用于切换接收状态

void setup() {
  Serial.begin(9600);                     // 初始化串口通信,波特率为9600

  PCA9685.begin();                        // 初始化PCA9685驱动板
  PCA9685.setPWMFreq(frequency);          // 设置PCA9685的PWM频率

  Serial.println(F("\n系统初始化完成"));   // 在串口监视器中打印初始化完成信息
}

// 串口输入 1——16 == 通道 0——15 正值打开,负值关闭

void loop() {
  if (Serial.available() > 0) {                         // 检查串口是否有可用数据
    receiveData = Serial.readString().toInt();          // 读取串口数据并转换为整数
    receiveState = !receiveState;                       // 切换接收状态
  }

  if (receiveState) {                                   // 如果接收状态为true处理接收到的数据
    if (receiveData > 0) {                              // 如果接收到的数据大于0
      receiveData = receiveData - 1;                    // 计算控制通道
      Serial.print(F("\n通道"));
      Serial.print(receiveData);
      Serial.println(F(":打开"));
      PCA9685.setPWM(receiveData, 4096, 0);             // 打开对应的PWM通道
    } else {                                            // 如果接收到的数据小于0
      receiveData = abs(receiveData + 1);               // 计算控制通道
      Serial.print(F("\n通道"));
      Serial.print(receiveData);
      Serial.println(F(":关闭"));
      PCA9685.setPWM(receiveData, 0, 4096);             // 关闭对应的PWM通道
    }
    receiveState = !receiveState;                       // 切换接收状态
  }
}
