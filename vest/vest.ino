#include "Adafruit_NeoPixel.h"


#define  DEBUG 1
#define CASSERT(X) typedef byte STATIC_ASSERT[X?1:-1]

#include <CapPin.h>
#include <TimerOne.h>

#define PIEZO_PIN 9

#define LED_STRIP_PIN 5
#define NUM_PIXELS 8
#define LEDPIN 13

CapPin cPin_C = CapPin(8);
CapPin cPin_Cs = CapPin(7);
CapPin cPin_D = CapPin(6);

CapPin cPin_Ds = CapPin(4);
CapPin cPin_E = CapPin(3);
CapPin cPin_F = CapPin(2);

CapPin cPin_Fs = CapPin(11);  
CapPin cPin_G = CapPin(12);
CapPin cPin_Gs = CapPin(13);
CapPin cPin_A = CapPin(A3); // A0 wasn't working in my promini
CapPin cPin_As = CapPin(A1);
CapPin cPin_B = CapPin(A2);

CapPin* pins[] = { &cPin_C, &cPin_Cs, &cPin_D, &cPin_Ds, &cPin_E, &cPin_F, &cPin_Fs, &cPin_G, &cPin_Gs, &cPin_A, &cPin_As, &cPin_B}; 
byte names[] = {'c', 'C', 'd', 'D', 'e', 'f', 'F', 'g', 'G', 'a', 'A', 'b'};
// In Hz. x2 for a higher octave
int tones[]  = {261, 277, 294, 311, 329, 349, 370, 392, 415, 440, 466, 493};

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0])) 
const int NUM_PINS = ARRAY_SIZE(pins);
// static assert to make sure i didn't miss anything
CASSERT(NUM_PINS == ARRAY_SIZE(names) );
CASSERT(NUM_PINS == ARRAY_SIZE(tones) );


byte melody[] =                "dafcdadcfdacdafcdaagp";
byte times[sizeof(melody)] =   "221222222222221222228";
byte octaves[sizeof(melody)] = "555555555555555555555";

int tempo = 70;


#define THRESH 10

class Peizo {
  int m_pin;
  int m_isOn;
public:
  Peizo(int pin) : 
  m_pin(pin), m_isOn(false) {
  }

  void start(long frequencyHz) {
    long periodMicro = 1000000/frequencyHz;

    Timer1.setPeriod(periodMicro);
    if (!m_isOn) {
      Timer1.pwm(m_pin, 512);
      m_isOn = true;
    }
  }

  void stop() {
    m_isOn = false;
    Timer1.disablePwm(m_pin);  
  }


  void play(long frequency, long time){
    start(frequency);
    delay(time);
    stop();

  }
  bool isOn() {
    return m_isOn;
  }
};

Peizo peizo(PIEZO_PIN);



// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, LED_STRIP_PIN);


//https://learn.adafruit.com/led-ampli-tie/the-code
// Input a value 0 to 255 to get a color value.
// The colors are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } 
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

#define SAMPLES 1000

void setup() {
#ifdef DEBUG
  Serial.begin(9600);
#endif
    Timer1.initialize();
  strip.begin();

  pinMode(LEDPIN, OUTPUT);
  pinMode(PIEZO_PIN, OUTPUT);



  for (int i = 0; i < NUM_PINS; ++i) {
    
#ifdef DEBUG
  Serial.print("Calibrating pin: ");
  Serial.println(i);
#endif
    pins[i]->calibratePin(SAMPLES);
  }

  strip.show(); // Initialize all pixels to 'off'


  digitalWrite(LEDPIN,HIGH);
  peizo.play(329,200);
  delay(100);
  peizo.play(329,200);
  delay(100);  
  peizo.play(329,250);
  delay(150);
  digitalWrite(LEDPIN,LOW);    
#ifdef DEBUG
  Serial.println("Setup done");
#endif

}


// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

boolean isPressed(CapPin* pin) {
  long total = pin->readPin(SAMPLES);
  return total > THRESH;
}

int addNote(int noteIndex) {
   return 0;
}

int doLightShow(int special) {
   return 0;
}

bool currentPressed = false;
char currentNote = '\0';
int noteIndex = -1;
void loop() {
  /////////// get index of button
  int i;
  for (i = 0; i < NUM_PINS; ++i) {
    if (isPressed(pins[i])){
      currentNote = names[i];
      break;
    }
  }

  if (i == NUM_PINS) {
    currentNote = '\0';
  }

  /// play a note, or stop playing one. 
  if ((currentNote != '\0') && (!currentPressed)) {
#ifdef DEBUG
  Serial.print("Note index: ");
  Serial.println(i);
#endif

    currentPressed = true;
    noteIndex = i;
    digitalWrite(LEDPIN, HIGH);
    int special = addNote(noteIndex);
    
    if (special == 0) {
      peizo.start(tones[i]);
    } else {
      doLightShow(special);
    }
    
    
  } else if ((currentNote == '\0') && (currentPressed)) {
#ifdef DEBUG
  Serial.print("Note OFF index: ");
  Serial.println(i);
#endif
    // key was released (no touch, and it was pressed before)
    currentPressed = false;
    noteIndex = -1;
    peizo.stop();
    digitalWrite(LEDPIN, LOW);
  }

  animate();
}


#ifdef DEBUG  
const uint32_t red = strip.Color(255,0,0);
const uint32_t blu = strip.Color(0,0,255);
const uint32_t yellow = strip.Color(255,255, 0);
#endif

unsigned long lastDrawTime = 0;
unsigned long stepIndex = 0;
void animate() {
  /////////// calc frame
  unsigned long timePassed = millis();
  if ((timePassed- lastDrawTime) > 20) {
    // increase frame
    stepIndex++;
    stepIndex = stepIndex % 256;
    lastDrawTime = timePassed;
  }

  /////////// animate leds
  if (currentNote == '\0') {
    rainbow_step();
  } 
  else {
    
#ifdef DEBUG  
  boolean bit1 = noteIndex & 1;
  boolean bit2 = noteIndex & 2;
  boolean bit3 = noteIndex & 4;
  boolean bit4 = noteIndex & 8;
  
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, yellow);
  }
  
  strip.setPixelColor(3, bit1?red:blu);
  strip.setPixelColor(4, bit2?red:blu);
  strip.setPixelColor(5, bit3?red:blu);
  strip.setPixelColor(6, bit4?red:blu);
  
 #else
    uint32_t color = noteToColor(noteIndex);
    for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, color);
    }
 #endif
 
  }
  strip.show();
}

uint32_t noteToColor(int i) {
  
  return Wheel(map(i,0,NUM_PINS-1,0,255));
}

void rainbow_step() {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, Wheel((i + stepIndex) & 255));
  }
}


