// #include <Adafruit_SSD1306.h>

// #include "defines.h"
// #include "wifi_connector.h"

// Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

// void setupDisplay() {
//   Serial.println("init display");
//   display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_ADDRESS);
//   display.display();
// }

// void showText(String text, uint8_t fontSize, unsigned long delayTime) {
//   Serial.print("showing text: ");
//   Serial.println(text);
//   display.clearDisplay();
//   display.setTextSize(fontSize);
//   display.setTextColor(SSD1306_WHITE);
//   display.setCursor(0,0);
//   display.print(text);
//   display.setCursor(0,0);
//   display.display();
//   if(delayTime > 0) {
//     delay(delayTime);
//   }
// }

// void showTemperatures(float dsTemp, float amTemp, float humidity, bool heating) {
//   display.clearDisplay();
//   display.setTextSize(1);
//   display.setTextColor(SSD1306_WHITE);
//   display.setCursor(0,0);
//   display.print("Floor:    ");
//   display.println(dsTemp);  
//   display.print("Air:      ");
//   display.println(amTemp);  
//   display.print("Humidity: ");
//   display.println(humidity);
//   display.print("Heating:  ");
//   display.print(heating ? "ON" : "OFF");
//   display.setCursor(0,0);
//   display.display();
// }

// void showIp() {
//   Serial.println("clear display");
//   delay(10);
//   display.clearDisplay();
//   Serial.println("set text size");
//   delay(10);
//   display.setTextSize(2);
//   Serial.println("set text color");
//   delay(10);
//   display.setTextColor(SSD1306_WHITE);
//   Serial.println("set cursor");
//   delay(10);
//   display.setCursor(0,0);
//   Serial.print("show IP: ");
//   Serial.println(getLocalIp());
//   delay(10);
//   display.print(getLocalIp());
//   Serial.println("set cursor");
//   delay(10);
//   display.setCursor(0,0);
//   Serial.println("display");
//   delay(10);
//   display.display();
//   Serial.println("delay");  
//   delay(3000);
// }
