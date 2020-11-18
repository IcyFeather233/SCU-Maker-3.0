#include <WS2812FX.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <string.h>
#include "string.h"

const char* ssid = "SCU_Makers";  // wifi名称
const char* password = "iloveSCU"; // wifi密码
const char* mqtt_server = "scumaker.org";  // mqtt服务器地址，如本地ip192.168.31.22

const byte ledPin = D4; // 需要控制的led灯

#define LED_COUNT 110
#define TIMER_MS 5000

WiFiClient espClient;
PubSubClient client(espClient);
WS2812FX ws2812fx = WS2812FX(LED_COUNT, ledPin, NEO_RGB + NEO_KHZ800);

int nowLeft = 0;
int nowRight = 100;
int idx = 0;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println();

  char* receivedChar = (char*)payload;
  String str_r = receivedChar;
  Serial.println("the str_r before is :" + str_r);
  str_r = str_r.substring(0,length);
  Serial.println("the str_r after is :" + str_r);
  String str_t = topic;
  Serial.println("the str_t is :" + str_t);
  
//----------------------------------------------------------------------
//-------------------------分段部分-------------------------------------
  if(str_t == "character")
  {
      char ch = str_r[0];
      if(ch >= 'a' && ch <= 'z')
      {
        ch -= ('a' - 'A');
      }
      Serial.println(ch);
      switch(ch)
      {
        case 'S':
          Serial.println("enter switch");
          nowLeft = 0;
          nowRight = 20;
          idx = 0;
          break;
        case 'C':
          nowLeft = 30;
          nowRight = 50;
          idx = 1;
          break;
        case 'U':
          nowLeft = 60;
          nowRight = 80;
          idx = 2;
          break;
        case 'M':
          nowLeft = 90;
          nowRight = 110;
          idx = 3;
          break;
      }
  }
//----------------------------------------------------------------------
//-------------------------开关部分-------------------------------------
  if(str_t == "ledStatus")
  {
      if (receivedChar[0] == '1') {  // 收到消息是 '1' 点亮LED灯
        ws2812fx.setBrightness(100);
      }
      if (receivedChar[0] == '0') {  // 收到消息是 '0' 关闭LED灯
        ws2812fx.setBrightness(0);
      }
  }
//----------------------------------------------------------------------
//-------------------------颜色部分-------------------------------------
  if(str_t == "ledColor")
  {
      String str_color = str_r.substring(1,7);
//      Serial.println(str_color);
      String str_color_convert = str_color.substring(2,4) + str_color.substring(0,2) + str_color.substring(4,6);
//      Serial.println(str_color_convert);
      char *offset;
      uint32_t colorhex = strtol(str_color_convert.c_str(), &offset, 16);
//      Serial.println(colorhex);
//      ws2812fx.setColor(colorhex);
      ws2812fx.setSegment(idx, nowLeft, nowRight, FX_MODE_STATIC, colorhex, 1000, false);
  }
//----------------------------------------------------------------------
//-------------------------亮度部分-------------------------------------
  if(str_t == "ledBrt")
  {
      char *offset;
      uint32_t b = strtol(str_r.c_str(), &offset, 10) * (255.0/100.0);
      Serial.println(b);
      ws2812fx.setBrightness(b);
  }

  Serial.println();
}
 
 
void reconnect() {
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266 Client"))
    {
      Serial.println("connected");
      client.subscribe("ledStatus");  // 订阅 'ledStatus' 这个topic
      client.subscribe("ledColor"); 
      client.subscribe("ledBrt"); 
      client.subscribe("character"); 
    }
    else
    {
       Serial.print("failed, rc=");
       Serial.print(client.state());
       Serial.println(" try again in 5 seconds");
       delay(5000);
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
 
  client.setServer(mqtt_server, 1883); // 连接mqtt
  client.setCallback(callback);        // 设置回调，控制led灯
//---------------------初始化--------------------------------------------------------------
  ws2812fx.init();
  ws2812fx.setBrightness(0);
  ws2812fx.setColor(0x007BFF);
  ws2812fx.setMode(FX_MODE_STATIC);
  ws2812fx.start();
//-----------------------------------------------------------------------------------------
//--------------------字段设置-------------------------------------------------------------

}

void loop() {
  ws2812fx.service();
//-----------------------------------mqtt client----------------------------------------
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
//--------------------------------------------------------------------------------------

}
