#include <Wire.h>                     // IIC库
#include <Adafruit_PWMServoDriver.h>  // 舵机驱动板库

#define usMin 600             // 脉冲宽度最小值
#define usMax 2500            // 脉冲宽度最大值
#define minPos 0              // 最小角度
#define maxPos 180            // 最大角度
#define servoFreq 50          // 频率
#define servoNumber 9         // 舵机数量
#define dataSeparator ','     // 数据处理分割符
#define posChangeinterval 10  // 角度变化间隔(ms)

bool servoRunDir = false;                                                           // 舵机运行方向
bool servoAutoState = false;                                                        // 舵机自动模式标志位
unsigned long lastPosChangeTime = 0;                                                // 上次角度改变时间
int servoCurrentPos[servoNumber] = { 20, 15, 13, 20, 13, 20, 13, 23, 13 };          // 舵机当前角度
int servoInitialPos[servoNumber] = { 20, 15, 13, 20, 13, 20, 13, 23, 13 };          // 初始角度
int servoTargetPos[servoNumber] = { 105, 110, 100, 105, 100, 105, 100, 110, 100 };  // 目标角度

Adafruit_PWMServoDriver pwm01 = Adafruit_PWMServoDriver(0x41);  // 创建舵机驱动板对象

void setup() {
  Serial.begin(9600);  // 设置波特率

  pwm01.begin();                // PCA9685模块初始化
  pwm01.setPWMFreq(servoFreq);  // 设置模块PWM频率

  for (uint8_t i = 0; i < servoNumber; i++) {                                           // 舵机角度初始化
    pwm01.writeMicroseconds(i, map(servoInitialPos[i], minPos, maxPos, usMin, usMax));  // 舵机角度控制
  }

  Serial.println(F("\n系统初始化完成"));  // 串口打印
}

void loop() {
  if (Serial.available() > 0) {                                   // 如果串口有数据传入
    String inputStr = Serial.readStringUntil('\n');               // 读取串口数据直到回车符结束
    int dataSeparatorPosition = inputStr.indexOf(dataSeparator);  // 获取数据分割符位置

    if (dataSeparatorPosition < 0) {                                 // 如果本次处理的字符串没有数据分割符
      if (inputStr == "A" || inputStr == "a") {                      // 如果输入为字符A/a
        servoAutoState = true;                                       // 舵机自动模式标志位置true
        lastPosChangeTime = millis();                                // 更新上次角度变化时间
        Serial.println(F("进入自动模式"));                            // 串口打印
        return;                                                      // 跳出本次处理
      } else {                                                       // 如果输入字符不为A/a
        Serial.println(F("指令格式错误,正确指令格式: ID,Command"));    // 串口打印
        return;                                                      // 跳出本次处理
      }
    } else {                     // 如果有数据分割符
      if (servoAutoState) {      // 如果舵机自动模式标志位为true
        servoAutoState = false;  // 自动模式标志位置false
      }
      int ID = inputStr.substring(0, dataSeparatorPosition).toInt() - 1;  // 获取ID
      // 判断ID是否存在,不存在则跳出本次处理
      if (ID < 0 && ID > servoNumber) {
        Serial.println(F("设备ID无效"));
        return;
      }
      int Command = inputStr.substring(dataSeparatorPosition + 1).toInt();  // 获取数据
      Serial.print(F("设备ID: "));
      Serial.println(ID);
      Serial.print(F("设置参数: "));
      Serial.println(Command);
      servoCurrentPos[ID] = Command;                                            // 更新对应舵机的当前角度
      pwm01.writeMicroseconds(ID, map(Command, minPos, maxPos, usMin, usMax));  // 舵机角度控制
    }
  }

  if (servoAutoState) {                                      // 如果舵机自动模式标志位为true
    bool servoState = true;                                  // 定义舵机状态标志位为true
    if (millis() - lastPosChangeTime > posChangeinterval) {  // 如果到达角度变化时间
      lastPosChangeTime = millis();                          // 更新上次角度变化时间
      if (!servoRunDir) {                                    // 如果舵机运行方向为false
        for (uint8_t i = 0; i < servoNumber; i++) {          // 遍历所有舵机
          if (servoTargetPos[i] > servoCurrentPos[i]) {      // 如果目标角度大于当前角度
            servoCurrentPos[i]++;                            // 当前角度+1
            servoState = false;                              //舵机状态标志位置false
          }
          pwm01.writeMicroseconds(i, map(servoCurrentPos[i], minPos, maxPos, usMin, usMax));  // 舵机角度控制
        }
      } else {                                            // 如果舵机运行方向为true
        for (uint8_t i = 0; i < servoNumber; i++) {       // 遍历所有舵机
          if (servoCurrentPos[i] > servoInitialPos[i]) {  // 如果当前角度大于初始角度
            servoCurrentPos[i]--;                         // 当前角度-1
            servoState = false;                           //舵机状态标志位置false
          }
          pwm01.writeMicroseconds(i, map(servoCurrentPos[i], minPos, maxPos, usMin, usMax));  // 舵机角度控制
        }
      }

      if (servoState) {              // 如果舵机状态标志位为true
        servoRunDir = !servoRunDir;  // 舵机运行方向取反
        Serial.println(F("方向切换"));
      }
    }
  }
}
