#include <Arduino.h>
#include <U8g2lib.h>

#define OLED_CLOCK OLED_CLK
#define OLED_DATA OLED_SDA
#define OLED_RESET OLED_RST

U8G2_SSD1306_128X64_NONAME_F_HW_I2C OLED(U8G2_R2, OLED_RST, OLED_SCL, OLED_SDA);

void setup() {
  // put your setup code here, to run once:
  OLED.begin();
  OLED.clear();
  OLED.setFont(u8g2_font_profont12_tf);
  OLED.print("Hello xd");
}

void loop() {
  // put your main code here, to run repeatedly:
}