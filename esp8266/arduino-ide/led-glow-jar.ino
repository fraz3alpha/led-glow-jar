#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 2

#define PIXEL_ROWS 4
#define PIXEL_COLS 6
#define PIXEL_BRIGHTNESS 255
#define PIXEL_COUNT 24

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIN, NEO_GRB + NEO_KHZ800);



#define PIXEL_BRIGHTNESS_FACTOR 2

uint8_t pixel_rows[] = {
          0, 1, 2, 3,
          3, 2, 1, 0,
          0, 1, 2, 3,
          3, 2, 1, 0,
          0, 1, 2, 3,
          3, 2, 1, 0,
        };

uint8_t pixel_cols[] = {
          0, 0, 0, 0,
          1, 1, 1, 1,
          2, 2, 2, 2,
          3, 3, 3, 3,
          4, 4, 4, 4,
          5, 5, 5, 5,
        };

uint8_t pixel_offset[PIXEL_COUNT];

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code

  Serial.begin(57600);

  // Create the randomness
  for (uint16_t i=0; i<PIXEL_COUNT; i++) {
    pixel_offset[i] = random(256);
    Serial.println(pixel_offset[i]);
  }

  strip.begin();
  strip.setBrightness(PIXEL_BRIGHTNESS);
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  // Some example procedures showing how to display to the pixels:
//  colorWipe(strip.Color(255, 0, 0), 50); // Red
//  colorWipe(strip.Color(0, 255, 0), 50); // Green
//  colorWipe(strip.Color(0, 0, 255), 50); // Blue
//colorWipe(strip.Color(0, 0, 0, 255), 50); // White RGBW
  // Send a theater pixel chase in...
//  theaterChase(strip.Color(127, 127, 127), 50); // White
//  theaterChase(strip.Color(127, 0, 0), 50); // Red
//  theaterChase(strip.Color(0, 0, 127), 50); // Blue
//
//  rainbow(20);
//  rainbowCycle(20);
//  rainbow_up(20);
    random_pixel2();
//    tardis(2);
//  theaterChaseRainbow(50);
}

void tardis(uint8_t wait) {
  uint16_t i, j, k;

  uint16_t previous_row;

  // All the way up
  for(i=0; i<PIXEL_ROWS; i++) {

    if (i == 0) {
      previous_row = PIXEL_ROWS - 1;
    } else {
      previous_row = i - 1;
    }

    for (j=0; j<256; j++) {
      // Decrease the one before, increase me
      for (k=0; k<PIXEL_COUNT; k++) {
        // Increase the brightness of pixels on this row


        if (pixel_rows[k] == i) {
//          strip.setPixelColor(k, white(j));
          strip.setPixelColor(k, white(j));
        } else if (pixel_rows[k] == previous_row) {
          strip.setPixelColor(k, white(255-j));
        } else {
          strip.setPixelColor(k, white(0));
        }
//        if (i > 0) {
//          // Decrease the brightness of the previous row, if we aren't at the bottom
//          if (pixel_rows[k] == i) {
//            strip.setPixelColor(k, white(256-j));
//          }
//        } else {
//          // Decrease the brightness of the top row, if we are at the bottom
//          if (pixel_rows[k] == PIXEL_ROWS-1) {
//            strip.setPixelColor(k, white(256-j));
//          }
//        }
      }
      strip.show();
      delay(wait);
    }
  }
}

// Fades up and down
// <random> -> <255> -> <0> -> <random> (then repeat)
void random_pixel2() {
  uint16_t intensity = 0;
  for (uint16_t t=0; t<512; t++){
    for (uint16_t i=0; i<PIXEL_COUNT; i++) {
      intensity = pixel_offset[i] + t;
      if (intensity > 255) {
        if (intensity > 511) {
          // We are coming back up to our starting value
          intensity -= 512;
        } else {
          // We are ramping back down
          intensity = 511 - intensity;
        }
      }

      strip.setPixelColor(i, white((intensity & 255) / PIXEL_BRIGHTNESS_FACTOR, 32));
    }
    strip.show();
    delay(5);
  }
}

// Fades up with an abrupt change to zero when it rolls over
void random_pixel() {
  for (uint16_t t=0; t<256; t++){
    for (uint16_t i=0; i<PIXEL_COUNT; i++) {
      strip.setPixelColor(i, white((t+pixel_offset[i]) & 255));
    }
    strip.show();
    delay(20);
  }
}


void rainbow_up(uint8_t wait) {
  uint16_t i, j, k;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
      for(i=0; i< strip.numPixels(); i++) {
        strip.setPixelColor(i, Wheel(((pixel_rows[i] * 256 / PIXEL_ROWS) + j) & 255));
      }
    strip.show();
    delay(wait);
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

uint32_t white(byte intensity) {
  return white(intensity, 0);
}

uint32_t white(byte intensity, byte minimum) {
  intensity = max(intensity, minimum);
  uint32_t white_value = intensity + (intensity << 8) + (intensity << 16) ;
  return white_value;
}
