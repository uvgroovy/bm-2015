#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include "Adafruit_Soundboard.h"

#include <CapPin.h>


// Choose any two pins that can be used with SoftwareSerial to RX & TX
#define SFX_TX 5
#define SFX_RX 6

// Connect to the RST pin on the Sound Board
#define SFX_RST 4

// You can also monitor the ACT pin for when audio is playing!

// we'll be using software serial
SoftwareSerial ss = SoftwareSerial(SFX_TX, SFX_RX);

// pass the software serial to Adafruit_soundboard, the second
// argument is the debug port (not used really) and the third 
// arg is the reset pin
Adafruit_Soundboard sfx = Adafruit_Soundboard(&ss, NULL, SFX_RST);
// can also try hardware serial with
// Adafruit_Soundboard sfx = Adafruit_Soundboard(&Serial1, NULL, SFX_RST);



#define NUM_PIXELS 8
#define LED_STRIP_PIN 7

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, LED_STRIP_PIN);



#define THRESH 10
#define SAMPLES 1000


boolean isPressed(CapPin* pin) {
  long total = pin->readPin(SAMPLES);
  return total > THRESH;
}


CapPin cPin_C = CapPin(2);
CapPin* pins[] = { &cPin_C}; 
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0])) 
const int NUM_PINS = ARRAY_SIZE(pins);


void setup() {
  Serial.begin(115200);
  Serial.println("Adafruit Sound Board!");
  
  // softwareserial at 9600 baud
  ss.begin(9600);
  // can also do Serial1.begin(9600)

  if (!sfx.reset()) {
    Serial.println("Not found");
    while (1);
  }
  Serial.println("SFX board found");
  
}


bool currentPressed = false;


void loop() {
  int i;
  for (i = 0; i < NUM_PINS; ++i) {
    if (isPressed(pins[i])){
      break;
    }
  }

  boolean isButtonPressed = (i < NUM_PINS);

  
  if (isButtonPressed && (!currentPressed)) {
    currentPressed = true;
    // TODO play something
    
    s
        if (! sfx.playTrack("T00     OGG") ) {
        Serial.println("Failed to play track?");
      } else {
       Serial.println("good track");
       
      } 
      
  } else {
    currentPressed = false;
    
       Serial.println("stop");
  }
  
  animate();
}


void animate() {
  
}
