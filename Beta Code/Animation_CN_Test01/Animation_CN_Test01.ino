#include <Wire.h>                     // IIC库
#include <Adafruit_PWMServoDriver.h>  // PCA9685驱动板库

// 创建对象,用于控制PCA9685驱动板,I2C地址为0x40
Adafruit_PWMServoDriver PCA9685 = Adafruit_PWMServoDriver(0x40);

#define interval 8        // 定义PWM更新间隔时间为8毫秒
#define increment 16      // 定义每次PWM增量为16
#define reduction 16      // 定义每次PWM减量为16
#define frequency 50      // 定义PWM频率为50Hz
#define channelOn 4095    // 定义PWM通道全开
#define channelOff 0      // 定义PWM通道全关
#define channelCount 16   // 定义通道数量为16
#define keepPointCount 4  // 像素点保持的数量

#define keepTime 1500       // 保持时间
#define weakLevel02 500     // 弱磁等级2
#define weakLevel01 1000    // 弱磁等级1
#define weakenedTime01 1400  // 减弱时间1
#define weakenedTime02 500  // 减弱时间2

unsigned long previousMillis = 0;                                 // 上一次更新时间初始化为0
int animationInital01[] = { 0, 1, 2, 3, 4, 11, 12, 13, 14, 15 };  // 动作数组
int animationInital02[] = { 0, 1, 2, 3, 4, 11, 12, 13, 14, 15, 8, 7, 0, 1, 2, 3 };
int animationInital03[] = { 0, 7, 8, 15, 14, 13, 10, 5, 2, 1, 0, 7, 8, 9, 10, 11, 4, 3, 2, 1, 0, 7, 8, 15 };
int animationInital04[] = { 0, 1, 2, 3, 4, 11, 10, 9, 8, 7, 0, 1, 2, 5, 10, 13, 14, 15, 8, 7, 0, 1, 2, 3 };

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
  animationShow04();
}

// 逐点遍历
void animationShow01() {
  static bool increasing = true;      // 初始化增加标志为true
  static int currentChannel = 0;      // 当前通道索引初始化为0
  static int currentChannelPwm = 0;   // 当前通道PWM值初始化为0
  static int previousChannelPwm = 0;  // 上一个通道PWM值初始化为0
  // 检查是否达到了更新PWM值的时间间隔
  if (millis() - previousMillis >= interval) {
    previousMillis = millis();  // 更新上一次时间戳为当前时间

    if (increasing) {
      // 如果不是第一个通道,更新上一个通道的PWM值
      if (currentChannel != 0) {
        previousChannelPwm -= increment;
        if (previousChannelPwm <= channelOff) {
          previousChannelPwm = channelOff;
        }
        PCA9685.setPWM(currentChannel - 1, 0, previousChannelPwm);
      }
      // 更新当前通道的PWM值
      PCA9685.setPWM(currentChannel, 0, currentChannelPwm);

      currentChannelPwm += increment;
      // 如果当前通道的PWM值达到最大
      if (currentChannelPwm >= channelOn) {
        if (++currentChannel > channelCount - 1) {
          // 如果所有通道都已经完成增加,切换到减少模式
          increasing = false;
          currentChannel = channelCount - 2;
          currentChannelPwm = channelOff;
          previousChannelPwm = channelOn;
        } else {
          // 切换到下一个通道,重置PWM值
          currentChannelPwm = channelOff;
          previousChannelPwm = channelOn;
        }
      }
    } else {
      // 如果不是最后一个通道,更新下一个通道的PWM值
      if (currentChannel != channelCount - 1) {
        previousChannelPwm -= increment;
        if (previousChannelPwm >= channelOn) {
          previousChannelPwm = channelOn;
        }
        PCA9685.setPWM(currentChannel + 1, 0, previousChannelPwm);
      }
      // 更新当前通道的PWM值
      PCA9685.setPWM(currentChannel, 0, currentChannelPwm);

      currentChannelPwm += increment;
      // 如果当前通道的PWM值达到最大
      if (currentChannelPwm >= channelOn) {
        if (--currentChannel < 0) {
          // 如果所有通道都已经完成减少,切换到增加模式
          increasing = true;
          currentChannel = 0;
          currentChannelPwm = channelOn;
          previousChannelPwm = channelOff;
        } else {
          // 切换到上一个通道,重置PWM值
          currentChannelPwm = channelOff;
          previousChannelPwm = channelOn;
        }
      }
    }
  }
}
// 上升到最高点后下落
void animationShow02() {
  static uint8_t systemState = 0;                                               // 初始化系统状态
  int arraysLength = sizeof(animationInital01) / sizeof(animationInital01[0]);  // 获取数组长度
  switch (systemState) {                                                        // 根据系统状态执行不同的动作
    case 0:                                                                     // 将四个点的磁流体上升到最高点
      for (int i = 0; i < arraysLength; i++) {
        PCA9685.setPWM(animationInital01[i], 0, channelOn);  // 开启当前动画数组下标对应的点
        if (i > keepPointCount - 1) {                        // 如果数组下标小于点数量-1,控制最开始通电的点断电
          PCA9685.setPWM(animationInital01[i - keepPointCount], 0, weakLevel01);
          delay(weakenedTime01);
          PCA9685.setPWM(animationInital01[i - keepPointCount], 0, weakLevel02);
          delay(weakenedTime02);
          PCA9685.setPWM(animationInital01[i - keepPointCount], 0, channelOff);
        }
      }
      systemState++;              // 系统状态+1
      previousMillis = millis();  // 更新时间
      break;

    case 1:                                         // 降落到第三行
      if (millis() - previousMillis >= keepTime) {  // 如果到达保持时间
        PCA9685.setPWM(8, 0, channelOn);            // 开启第三行的电磁铁
        PCA9685.setPWM(9, 0, channelOn);
        PCA9685.setPWM(10, 0, channelOn);
        PCA9685.setPWM(11, 0, channelOn);
        PCA9685.setPWM(12, 0, channelOff);  // 关闭第四行的电磁铁
        PCA9685.setPWM(13, 0, channelOff);
        PCA9685.setPWM(14, 0, channelOff);
        PCA9685.setPWM(15, 0, channelOff);
        systemState++;              // 系统状态+1
        previousMillis = millis();  //更新时间
      }
      break;

    case 2:                                         // 降落到第二行
      if (millis() - previousMillis >= keepTime) {  // 如果到达第二行
        PCA9685.setPWM(4, 0, channelOn);            // 开启第二行的电磁铁
        PCA9685.setPWM(5, 0, channelOn);
        PCA9685.setPWM(6, 0, channelOn);
        PCA9685.setPWM(7, 0, channelOn);
        PCA9685.setPWM(8, 0, channelOff);  // 关闭第三行的电磁铁
        PCA9685.setPWM(9, 0, channelOff);
        PCA9685.setPWM(10, 0, channelOff);
        PCA9685.setPWM(11, 0, channelOff);
        systemState++;              // 系统状态+1
        previousMillis = millis();  // 更新时间
      }
      break;

    case 3:                                         // 降落到第一行
      if (millis() - previousMillis >= keepTime) {  // 如果到达保持时间
        PCA9685.setPWM(0, 0, channelOn);            // 开启第一行电磁铁
        PCA9685.setPWM(1, 0, channelOn);
        PCA9685.setPWM(2, 0, channelOn);
        PCA9685.setPWM(3, 0, channelOn);
        PCA9685.setPWM(4, 0, channelOff);  // 关闭第二行电磁铁
        PCA9685.setPWM(5, 0, channelOff);
        PCA9685.setPWM(6, 0, channelOff);
        PCA9685.setPWM(7, 0, channelOff);
        systemState++;              // 系统状态+1
        previousMillis = millis();  // 更新时间
      }
      break;

    case 4:                                         // 关闭第一行电磁铁
      if (millis() - previousMillis >= keepTime) {  // 如果到达保持时间
        PCA9685.setPWM(0, 0, channelOff);           // 关闭第一行电磁铁
        PCA9685.setPWM(1, 0, channelOff);
        PCA9685.setPWM(2, 0, channelOff);
        PCA9685.setPWM(3, 0, channelOff);
        systemState = 0;            // 系统状态重置
        previousMillis = millis();  // 更新时间
      }
      break;
  }
}
// 外圈循环
void animationShow03() {
  int arraysLength = sizeof(animationInital02) / sizeof(animationInital02[0]);
  for (int i = 0; i < arraysLength; i++) {
    PCA9685.setPWM(animationInital02[i], 0, channelOn);
    if (i > keepPointCount - 1) {
      PCA9685.setPWM(animationInital02[i - keepPointCount], 0, weakLevel01);
      delay(weakenedTime01);
      PCA9685.setPWM(animationInital02[i - keepPointCount], 0, weakLevel02);
      delay(weakenedTime02);
      PCA9685.setPWM(animationInital02[i - keepPointCount], 0, channelOff);
    }
  }
}
// 左侧上升回旋
void animationShow04() {
  int arraysLength = sizeof(animationInital03) / sizeof(animationInital03[0]);
  for (int i = 0; i < arraysLength; i++) {
    PCA9685.setPWM(animationInital03[i], 0, channelOn);
    if (i > keepPointCount - 1) {
      PCA9685.setPWM(animationInital03[i - keepPointCount], 0, weakLevel01);
      delay(weakenedTime01);
      PCA9685.setPWM(animationInital03[i - keepPointCount], 0, weakLevel02);
      delay(weakenedTime02);
      PCA9685.setPWM(animationInital03[i - keepPointCount], 0, channelOff);
    }
  }
}
// 右侧上升回旋
void animationShow05() {
  int arraysLength = sizeof(animationInital04) / sizeof(animationInital04[0]);
  for (int i = 0; i < arraysLength; i++) {
    PCA9685.setPWM(animationInital04[i], 0, channelOn);
    if (i > keepPointCount - 1) {
      PCA9685.setPWM(animationInital04[i - keepPointCount], 0, weakLevel01);
      delay(weakenedTime01);
      PCA9685.setPWM(animationInital04[i - keepPointCount], 0, weakLevel02);
      delay(weakenedTime02);
      PCA9685.setPWM(animationInital04[i - keepPointCount], 0, channelOff);
    }
  }
}
