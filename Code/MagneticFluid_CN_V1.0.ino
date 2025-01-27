#include <Wire.h>                     
#include <Adafruit_PWMServoDriver.h>  


Adafruit_PWMServoDriver PCA9685 = Adafruit_PWMServoDriver(0x40);

#define interval 8        
#define increment 10      
#define reduction 10      
#define frequency 50      
#define channelOn 4095    
#define channelOff 0      
#define channelCount 16   
#define keepPointCount 4  

#define keepTime 1500        
#define weakLevel02 500      
#define weakLevel01 1000     
#define switchIntval 1000    
#define weakenedTime01 1400  
#define weakenedTime02 500   

uint8_t runState = 0;                                             
bool animation01State = false;                                    
bool animation02State = false;                                    
unsigned long previousMillis = 0;                                 
unsigned long previousSwitchMillis = 0;                           
int animationInital01[] = { 0, 1, 2, 3, 4, 11, 12, 13, 14, 15 };  
int animationInital02[] = { 0, 1, 2, 3, 4, 11, 12, 13, 14, 15, 8, 7, 0, 1, 2, 3 };
int animationInital03[] = { 0, 7, 8, 15, 14, 13, 10, 5, 2, 1, 0, 7, 8, 9, 10, 11, 4, 3, 2, 1, 0, 1, 2, 3 };
int animationInital04[] = { 0, 1, 2, 3, 4, 11, 10, 9, 8, 7, 0, 1, 2, 5, 10, 13, 14, 15, 8, 7, 0, 1, 2, 3 };

void setup() {
  Serial.begin(9600);  
  PCA9685.begin();                
  PCA9685.setPWMFreq(frequency);  

  
  for (uint8_t i = 0; i < channelCount; i++) {
    PCA9685.setPWM(i, channelOff, channelOff);
  }
  Serial.println(F("\n系统初始化完成"));  
}

void loop() {
  
  switch (runState) {
    case 0:
      animationShow01();                      
      if (animation01State == true) {         
        runState++;                           
        animation01State = false;             
        Serial.println(F("animation-1 ok"));  
        previousSwitchMillis = millis();      
      }
      break;

    case 1:
      if (millis() - previousSwitchMillis >= switchIntval) {  
        runState++;                                           
      }
      break;

    case 2:
      animationShow02();                      
      if (animation02State == true) {         
        runState++;                           
        Serial.println(F("animation-2 ok"));  
        previousSwitchMillis = millis();      
      }
      break;

    case 3:
      if (millis() - previousSwitchMillis >= switchIntval) {  
        runState++;                                           
      }
      break;

    case 4:
      animationShow03();                    
      runState++;                           
      Serial.println(F("animation-3 ok"));  
      previousSwitchMillis = millis();      
      break;

    case 5:
      if (millis() - previousSwitchMillis >= switchIntval) {  
        runState++;                                           
      }
      break;

    case 6:
      animationShow04();                    
      runState++;                           
      Serial.println(F("animation-4 ok"));  
      previousSwitchMillis = millis();      
      break;

    case 7:
      if (millis() - previousSwitchMillis >= switchIntval) {  
        runState++;                                           
      }
      break;

    case 8:
      animationShow05();                    
      runState++;                           
      Serial.println(F("animation-5 ok"));  
      previousSwitchMillis = millis();      
      break;

    case 9:
      if (millis() - previousSwitchMillis >= switchIntval) {  
        runState = 0;                                         
      }
      break;
  }
}


void animationShow01() {
  static bool increasing = true;      
  static int currentChannel = 0;      
  static int currentChannelPwm = 0;   
  static int previousChannelPwm = 0;  
  
  if (millis() - previousMillis >= interval) {
    previousMillis = millis();  

    if (increasing) {
      
      if (currentChannel != 0) {
        previousChannelPwm -= increment;
        if (previousChannelPwm <= channelOff) {
          previousChannelPwm = channelOff;
        }
        PCA9685.setPWM(currentChannel - 1, 0, previousChannelPwm);
      }
      
      PCA9685.setPWM(currentChannel, 0, currentChannelPwm);

      currentChannelPwm += increment;
      
      if (currentChannelPwm >= channelOn) {
        if (++currentChannel > channelCount - 1) {
          
          increasing = false;
          currentChannel = channelCount - 2;
          currentChannelPwm = channelOff;
          previousChannelPwm = channelOn;
        } else {
          
          currentChannelPwm = channelOff;
          previousChannelPwm = channelOn;
        }
      }
    } else {
      
      if (currentChannel != channelCount - 1) {
        previousChannelPwm -= increment;
        if (previousChannelPwm >= channelOn) {
          previousChannelPwm = channelOn;
        }
        PCA9685.setPWM(currentChannel + 1, 0, previousChannelPwm);
      }
      
      PCA9685.setPWM(currentChannel, 0, currentChannelPwm);

      currentChannelPwm += increment;
      
      if (currentChannelPwm >= channelOn) {
        if (--currentChannel < 0) {
          
          increasing = true;
          animation01State = true;
          currentChannel = 0;
          currentChannelPwm = channelOn;
          previousChannelPwm = channelOff;
        } else {
          
          currentChannelPwm = channelOff;
          previousChannelPwm = channelOn;
        }
      }
    }
  }
}
// 上升到最高点后下落
void animationShow02() {
  static uint8_t systemState = 0;                                               
  int arraysLength = sizeof(animationInital01) / sizeof(animationInital01[0]);  
  switch (systemState) {                                                        
    case 0:                                                                     
      for (int i = 0; i < arraysLength; i++) {
        PCA9685.setPWM(animationInital01[i], 0, channelOn);  
        if (i > keepPointCount - 1) {                        
          PCA9685.setPWM(animationInital01[i - keepPointCount], 0, weakLevel01);
          delay(weakenedTime01);
          PCA9685.setPWM(animationInital01[i - keepPointCount], 0, weakLevel02);
          delay(weakenedTime02);
          PCA9685.setPWM(animationInital01[i - keepPointCount], 0, channelOff);
        }
      }
      systemState++;              
      previousMillis = millis();  
      break;

    case 1:                                         
      if (millis() - previousMillis >= keepTime) {  
        PCA9685.setPWM(8, 0, channelOn);            
        PCA9685.setPWM(9, 0, channelOn);
        PCA9685.setPWM(10, 0, channelOn);
        PCA9685.setPWM(11, 0, channelOn);
        PCA9685.setPWM(12, 0, channelOff);  
        PCA9685.setPWM(13, 0, channelOff);
        PCA9685.setPWM(14, 0, channelOff);
        PCA9685.setPWM(15, 0, channelOff);
        systemState++;              
        previousMillis = millis();  
      }
      break;

    case 2:                                         
      if (millis() - previousMillis >= keepTime) {  
        PCA9685.setPWM(4, 0, channelOn);            
        PCA9685.setPWM(5, 0, channelOn);
        PCA9685.setPWM(6, 0, channelOn);
        PCA9685.setPWM(7, 0, channelOn);
        PCA9685.setPWM(8, 0, channelOff);  
        PCA9685.setPWM(9, 0, channelOff);
        PCA9685.setPWM(10, 0, channelOff);
        PCA9685.setPWM(11, 0, channelOff);
        systemState++;              
        previousMillis = millis();  
      }
      break;

    case 3:                                         
      if (millis() - previousMillis >= keepTime) {  
        PCA9685.setPWM(0, 0, channelOn);            
        PCA9685.setPWM(1, 0, channelOn);
        PCA9685.setPWM(2, 0, channelOn);
        PCA9685.setPWM(3, 0, channelOn);
        PCA9685.setPWM(4, 0, channelOff);  
        PCA9685.setPWM(5, 0, channelOff);
        PCA9685.setPWM(6, 0, channelOff);
        PCA9685.setPWM(7, 0, channelOff);
        systemState++;              
        previousMillis = millis();  
      }
      break;

    case 4:                                         
      if (millis() - previousMillis >= keepTime) {  
        PCA9685.setPWM(0, 0, channelOff);           
        PCA9685.setPWM(1, 0, channelOff);
        PCA9685.setPWM(2, 0, channelOff);
        PCA9685.setPWM(3, 0, channelOff);
        systemState = 0;  
        animation02State = true;
        previousMillis = millis();  
      }
      break;
  }
}
// 外圈循环
void animationShow03() {
  for (uint8_t i = 0; i < channelCount; i++) {
    PCA9685.setPWM(i, channelOff, channelOff);
  }
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

void animationShow04() {
  for (uint8_t i = 0; i < channelCount; i++) {
    PCA9685.setPWM(i, channelOff, channelOff);
  }
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

void animationShow05() {
  for (uint8_t i = 0; i < channelCount; i++) {
    PCA9685.setPWM(i, channelOff, channelOff);
  }
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
