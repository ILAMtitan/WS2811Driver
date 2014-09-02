#include <WS2811Driver.h>

WS2811Driver ledStrip = WS2811Driver(60, NEO_GRB);

void setup(void)
{
  uint16_t i;
  
  for(i=0; i<ledStrip.numPixels(); i++)
  {
    ledStrip.setPixelColor(i, 0, 0, 0);
  }
  
  ledStrip.begin();        // configure P1.6 for output
}

void loop() {
  
  colorWipe(ledStrip.Color(255, 0, 0), 50); // Red
  colorWipe(ledStrip.Color(0, 255, 0), 50); // Green
  colorWipe(ledStrip.Color(0, 0, 255), 50); // Blue
  // Send a theater pixel chase in...
  theaterChase(ledStrip.Color(127, 127, 127), 50); // White
  theaterChase(ledStrip.Color(127,   0,   0), 50); // Red
  theaterChase(ledStrip.Color(  0,   0, 127), 50); // Blue

  rainbowCycle(20);
  theaterChaseRainbow(50);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<ledStrip.numPixels(); i++) {
      ledStrip.setPixelColor(i, c);
      ledStrip.show();
      delay(wait);
  }
}

void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< ledStrip.numPixels(); i++) {
      ledStrip.setPixelColor(i, Wheel(((i * 256 / ledStrip.numPixels()) + j) & 255));
    }
    ledStrip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < ledStrip.numPixels(); i=i+3) {
        ledStrip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      ledStrip.show();
     
      delay(wait);
     
      for (int i=0; i < ledStrip.numPixels(); i=i+3) {
        ledStrip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < ledStrip.numPixels(); i=i+3) {
          ledStrip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        ledStrip.show();
       
        delay(wait);
       
        for (int i=0; i < ledStrip.numPixels(); i=i+3) {
          ledStrip.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
}

uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return ledStrip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return ledStrip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return ledStrip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
