#define BLINKER_PRINT Serial
#define BLINKER_WIFI

#include <Blinker.h>
#include <WS2812FX.h>

#define LED_COUNT 50
#define LED_PIN D4

#define TIMER_MS 5000

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_RGB + NEO_KHZ800);

unsigned long last_change = 0;
unsigned long now = 0;

char auth[] = "e1dce3cee61b";
char ssid[] = "SCU_Makers";
char pswd[] = "iloveSCU";

//新建组件对象
BlinkerButton Button1("BlinkBtn");
BlinkerNumber Number1("BlinkNum");

int counter = 0;
bool flag = false;
//按下会立即执行函数
void button1_callback(const String & state) {
  flag = !flag;
  BLINKER_LOG("get button state: ", state);  
  if(flag){
      ws2812fx.setBrightness(100);
  }
  else{
      ws2812fx.setBrightness(0);
  }
}

//如果未绑定的组件被触发则执行
void dataRead(const String & data) {
  BLINKER_LOG("Blinker readString: ", data);  
  counter ++;
  Number1.print(counter);
}
void setup() {  
  ws2812fx.init();
  ws2812fx.setBrightness(0);
  ws2812fx.setSpeed(1000);
  ws2812fx.setColor(0x007BFF);
  ws2812fx.setMode(FX_MODE_STATIC);
  ws2812fx.start();

  //Blinker部分
  Serial.begin(115200);

  #if defined(BLINKER_PRINT)
    BLINKER_DEBUG.stream(BLINKER_PRINT);
  #endif
  
  //初始化Blinker
  Blinker.begin(auth, ssid, pswd);
  Blinker.attachData(dataRead);
  Button1.attach(button1_callback);
}

void loop() {
  now = millis();

  ws2812fx.service();
  Blinker.run();
  if(now - last_change > TIMER_MS) {
    ws2812fx.setMode((ws2812fx.getMode() + 1) % ws2812fx.getModeCount());
    last_change = now;
  }
}
