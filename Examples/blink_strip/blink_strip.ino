// blink_strip - ws2811 led strip blink example
// 11-15-2012 Initial Version 0.1

#include <WS2811Driver.h>

// 24 bits of color data stored GGRRBB
const uint8_t leds0[] = { 0x00,0xff,0x00, 0xff,0x00,0x00, 0x00,0x00,0xff, 0xff,0xff,0xff }; // R,G,B,W
const uint8_t leds1[] = { 0x00,0x00,0x00, 0x00,0x00,0x00, 0x00,0x00,0x00, 0x00,0x00,0x00 }; // 0,0,0,0,

WS2811Driver ledStrip; // uses P1_7 as datapin connect to DIN on strip

void setup(void)
{
    ledStrip.setLEDCount(4); // setup for 4 leds on a strip
    ledStrip.begin();        // configure P1.7 for output
}

void loop() {
    ledStrip.write(leds0);               // set leds to red,green,blue,white
    delay(500);
    ledStrip.write(leds1,sizeof(leds1)); // turn off all 4 leds
    delay(500);
}
