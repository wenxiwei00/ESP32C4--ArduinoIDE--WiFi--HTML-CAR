#include <WiFi.h>
#include <WebServer.h>
#include "WebPage.h"  // Including the HTML code

// Wi-Fi credentials for AP mode
const char* ssid = "xi";
const char* password = "0401wei?";

WebServer server(80);

const int MOTOR_ENABLE_PIN = 4; // Motor ENABLE i.e. Motor PWM
const int MOTOR_INA_PIN = 5;    // Motor INA direction
const int MOTOR_INB_PIN = 6;    // Motor INB direction
const int LEDC_CHANNEL = 0;  

const int MOTOR2_ENABLE_PIN = 1; // 第二个舵机的ENABLE引脚，使用GPIO1
const int MOTOR2_INA_PIN = 8;    // 第二个舵机的INA引脚
const int MOTOR2_INB_PIN = 10;   // 第二个舵机的INB引脚
const int LEDC_CHANNEL2 = 1;     // 第二个舵机的PWM通道，确保不与其他PWM通道冲突

const int LEDC_RESOLUTION = 14; //0--16383. 16383全速转动 full duty，0表示不转动
const int LEDC_BASE_FREQ = 30;   // Set the base frequency for PWM. 




void setup() {
  Serial.begin(115200);

  // Setup as Access Point to AP mode & DHCP
  WiFi.mode(WIFI_AP); //设置esp32为AP模式，我的esp32作为路由器 
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP(); //随机分配一个网址给我 
  Serial.print("AP IP address: "); //把这个网址打印出来 要不我不知道随机分配的网址是什么
  Serial.println(IP);

  //setup第一个motor
  // Setup motor control pins
  pinMode(MOTOR_ENABLE_PIN, OUTPUT); //把enable和控制方向的两个pin脚设置成output
  pinMode(MOTOR_INA_PIN, OUTPUT);
  pinMode(MOTOR_INB_PIN, OUTPUT);

  // 设置LEDC PWM控制 第一个motor的PWM由LEDC的channel 0产生
  ledcSetup(LEDC_CHANNEL, LEDC_BASE_FREQ, LEDC_RESOLUTION); 
  ledcAttachPin(MOTOR_ENABLE_PIN, LEDC_CHANNEL);

  //setup第二个motor
  pinMode(MOTOR2_ENABLE_PIN, OUTPUT);
  pinMode(MOTOR2_INA_PIN, OUTPUT);
  pinMode(MOTOR2_INB_PIN, OUTPUT);

  // 设置LEDC PWM控制 第二个motor的PWM由LEDC的channel 1产生
  ledcSetup(LEDC_CHANNEL2, LEDC_BASE_FREQ, LEDC_RESOLUTION);
  ledcAttachPin(MOTOR2_ENABLE_PIN, LEDC_CHANNEL2);

  // Set server routes
  server.on("/", HTTP_GET, handleRoot);  //最初始的网页界面什么样子 取决于你返回给手机的html是什么样子
  server.on("/set", HTTP_GET, handleSetSpeed);  // 用户在网页上 滑动调整了speed--->网页会通过wifi传回来一个/set 具体为什么传的是set看我的WebPage.h码 -->收到这个/set之后，我调用handleSetSpeed这个函数
  server.on("/stop", HTTP_GET, handleEmergencyStop); // Add emergency stop route
  server.begin();
}

void loop() {
  server.handleClient(); //检查客户端是否连接到服务器了，并且根据客户发的请求来执行handleRoot(), handleSetSpeed(), handleEmergencyStop(), 和 handleGetSpeed()这些函数
}

void handleRoot() {
  server.send(200, "text/html", WEBPAGE_HTML);
}

void handleSetSpeed() { //set speed这个步骤要和WebPage.h一起看。
  int leftSpeed = 0;
  int rightSpeed = 0;

  // Check if the speed argument for left motor exists
  if (server.hasArg("leftSpeed")) {
    leftSpeed = server.arg("leftSpeed").toInt();  //我从客户端哪里控制的left speed是多少
    // Handle the speed and direction for the left motor
    if (leftSpeed > 0) {
      digitalWrite(MOTOR_INA_PIN, LOW);
      digitalWrite(MOTOR_INB_PIN, HIGH);
    } else if (leftSpeed < 0) {               //如果我控制的是负数代表我希望小车后退，我要用函数把他转化为正数，只是方向改变
      digitalWrite(MOTOR_INA_PIN, HIGH);
      digitalWrite(MOTOR_INB_PIN, LOW);
      leftSpeed = -leftSpeed; // Make the speed positive for PWM
    }
    ledcWrite(LEDC_CHANNEL, leftSpeed);
  }

  // Check if the speed argument for right motor exists
  if (server.hasArg("rightSpeed")) {
    rightSpeed = server.arg("rightSpeed").toInt();
    // Handle the speed and direction for the right motor
    if (rightSpeed > 0) {
      digitalWrite(MOTOR2_INA_PIN, LOW);
      digitalWrite(MOTOR2_INB_PIN, HIGH);
    } else if (rightSpeed < 0) {
      digitalWrite(MOTOR2_INA_PIN, HIGH);
      digitalWrite(MOTOR2_INB_PIN, LOW);
      rightSpeed = -rightSpeed; // Make the speed positive for PWM
    }
    ledcWrite(LEDC_CHANNEL2, rightSpeed);
  }
  
  // Serial.print("Left motor speed set to: ");
  // Serial.println(leftSpeed);
  // Serial.print("Right motor speed set to: ");
  // Serial.println(rightSpeed);

  server.send(200, "text/plain", "OK"); //在HTTP协议中 向客户端发送HTTP响应200表示请求成功
}

void handleEmergencyStop() {
  // Set both motors to stop
  digitalWrite(MOTOR_INA_PIN, LOW); //控制motor方向的pin口全部设置为0
  digitalWrite(MOTOR_INB_PIN, LOW);
  digitalWrite(MOTOR2_INA_PIN, LOW);
  digitalWrite(MOTOR2_INB_PIN, LOW);

  ledcWrite(LEDC_CHANNEL, 0); // Stop the first motor //把enable的那个pin口的pwm设置为0
  ledcWrite(LEDC_CHANNEL2, 0); // Stop the second motor


  server.send(200, "text/plain", "STOPPED");
}




