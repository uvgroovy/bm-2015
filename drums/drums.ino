#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include "Adafruit_Soundboard.h"
#include "buttons.h"
#include <CapPin.h>
//#define USE_SOUNDSBOARD 1
#define USE_LED 1




// Choose any two pins that can be used with SoftwareSerial to RX & TX
#define SFX_TX 5
#define SFX_RX 6

// Connect to the RST pin on the Sound Board
#define SFX_RST 4

// You can also monitor the ACT pin for when audio is playing!

// we'll be using software serial
SoftwareSerial ss = SoftwareSerial(SFX_TX, SFX_RX);

#if USE_SOUNDSBOARD
// pass the software serial to Adafruit_soundboard, the second
// argument is the debug port (not used really) and the third 
// arg is the reset pin
Adafruit_Soundboard sfx = Adafruit_Soundboard(&ss, NULL, SFX_RST);
// can also try hardware serial with
// Adafruit_Soundboard sfx = Adafruit_Soundboard(&Serial1, NULL, SFX_RST);

#endif

#define ROWS 3
#define PIXELS_PER_ROW 16
#define LED_STRIP_PIN 7
#define NUM_PIXELS PIXELS_PER_ROW*ROWS

#if USE_LED
// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, LED_STRIP_PIN);
#endif


#define THRESH 100
#define SAMPLES 1000
#define LEDPIN 13

boolean isPressed(CapPin* pin) {
  long total = pin->readPin(SAMPLES);
  if (total > THRESH) {
    Serial.print("Total: ");
    Serial.println(total);
  }
  return total > THRESH;
  
}

#define DEBUG

#define CASSERT(X) typedef byte STATIC_ASSERT[X?1:-1]

#ifdef DEBUG
char rawPin[][3] = {"2","3","8","9", "10", "11", "12", "A0", "A1", "A2", "A3"};
#endif
CapPin pins[] = { CapPin(2), CapPin(3 ), CapPin(8), CapPin(9), CapPin(10), CapPin(11), CapPin(12), CapPin(A0), CapPin(A1), CapPin(A2), CapPin(A3)};
 
//CapPin pins[] = { CapPin(2), CapPin(3)};

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0])) 
const int NUM_PINS = ARRAY_SIZE(pins);

CASSERT(NUM_PINS == 11);

void setup() {
  Serial.begin(115200);
  Serial.println("Adafruit Sound Board!");
  
  // softwareserial at 9600 baud
  ss.begin(9600);
  // can also do Serial1.begin(9600)

#if USE_SOUNDSBOARD
  if (!sfx.reset()) {
    Serial.println("Not found");
    while (1);
  }
  Serial.println("SFX board found");

  uint8_t files = sfx.listFiles();
  if (files == 0) {
    Serial.println("Found 0 files. Check GND<->UG");
    while (1);
  }

  Serial.print("Found ");
  Serial.print(files);
  Serial.println(" files");
#endif
  for (int i = 0; i < NUM_PINS; ++i) {
#ifdef DEBUG
  Serial.print("Calibrating pin: ");
  Serial.println(i);
#endif
    pins[i].calibratePin(SAMPLES);
  }

  // This magic is super important for some reason.
  // Merging this will the calibration makes some buttons go out of whack
  for (int i = 0; i < NUM_PINS; ++i) {
#ifdef DEBUG
  Serial.print("Read pin: ");
  Serial.println(i);
#endif
    
    pins[i].readPin(SAMPLES);
  }
  
  
  Serial.println("Setup done");

#if USE_LED
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  testLEDs();

#endif
}

#if USE_LED
void testLEDs() {
  Serial.println("Testing LEDs");
  for (int row = 0; row < ROWS; row++) {
    strip.begin();
    for (int col = 0; col < PIXELS_PER_ROW; col++) {
      uint32_t color = 0xFF;
      color <<= 8*row;
      strip.setPixelColor(row * PIXELS_PER_ROW + col,color);
    }

    strip.show();
    delay(1000);
    clearToColor(0);
  }

  clearToColor(0);
}

void clearToColor(uint32_t color) {
  strip.begin();
  for (int row = 0; row < ROWS; row++) {
    for (int col = 0; col < PIXELS_PER_ROW; col++) {
      strip.setPixelColor(row * PIXELS_PER_ROW + col,color);
    }
  }
  strip.show();
}
void setColor(uint32_t color) {
  for(int i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();
}


void setColorForButton(BUTTONS button, int color) {
  byte start,end;
  indexesForButton(button, start, end);

  strip.begin();
  for (int i = start; i <= end; i++) {
    strip.setPixelColor(i, color);      
  }
  strip.show();
}
#else
void setColor(uint32_t color) {
}

#endif

struct LEDAnimation {
  BUTTONS _currentButton;
  byte _startIndex;
  byte _endIndex;

  LEDAnimation() {
    _currentButton = BUTTON_NONE;
  }
  
  void start(BUTTONS button) {
    if (_currentButton) {
      setColorForButton(_currentButton, 0);
    }

    _currentButton = button;
   setColorForButton(button, 0xFF);
  };

};
bool currentPressed = false;

void loop() {
  int i;
  for (i = 0; i < NUM_PINS; ++i) {
    if (isPressed(pins + i)){
      break;
    }
  }

  boolean isButtonPressed = (i < NUM_PINS);

  
  if (isButtonPressed && (!currentPressed)) {
    currentPressed = true;
    digitalWrite(LEDPIN, HIGH);
    // TODO play something
    
    work((BUTTONS)(i+1));
    
    #ifdef DEBUG
    Serial.print("Button pressed: ");
    Serial.print(i);
    Serial.print(" Pin: ");
    Serial.println(rawPin[i]);
    #endif
      
  } else if (!isButtonPressed && (currentPressed)) {
    currentPressed = false;
    digitalWrite(LEDPIN, LOW);
    
       Serial.println("stop");
  }
  
  animate();
}

void work(BUTTONS index) {

 static LEDAnimation d;
 d.start(index);
 switch(index) {
  case  TOP_RIGHT:
  break;
  case  TOP_LEFT:
  break;
  
  case  TOP_MIDV:
  break;  
  case  MID_LEFT:
  break;
  case  MID_MID:
  break;
  case  MID_RIGHT:
  break;
  case BUTT_LEFT:
  case  BUTT_RIGHT:
  break;
  case  ARR_DOWN:
  break;
  case ARR_UP:
  break;
  case  BUTT_MID:
  break;
  default:
       Serial.println("un known command");
  
 }   
}

void animate() {
  
}



void indexesForButton(BUTTONS button, byte& start, byte& end) {
  switch(button) {
    case TOP_RIGHT:
      start = 0;
      end = 3;
      break;
    case TOP_MIDV:
      start = 4;
      end = 7;
      break;
    case TOP_LEFT:
      start=8;
      end=12;
      break;
    case MID_LEFT:
      start=18;
      end=21;
      break;
    case MID_MID:
      start=23;
      end=25;
      break;
    case MID_RIGHT:
      start=27;
      end=30;
      break;
    case BUTT_RIGHT:
      start=34;
      end=36;
      break;
    case BUTT_MID:
      start=37;
      end=40;
      break;
    case BUTT_LEFT:
      start=42;
      end=45;
      break;

  }
};





