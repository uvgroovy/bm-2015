#include <Wire.h>
#include <CapPin.h>

#define DEBUG
#define THRESH 100
#define SAMPLES 1000
#define LEDPIN 13

/**************************
 * Buttons
 **************************/

#ifdef DEBUG
char rawPin[][3] = {"2","3","8","9", "10", "11", "12", "A0", "A1", "A2", "A3"};
#endif
CapPin pins[] = { CapPin(2), CapPin(3 ), CapPin(8), CapPin(9), CapPin(10), CapPin(11), CapPin(12), CapPin(A0), CapPin(A1), CapPin(A2), CapPin(A3)};
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0])) 

const int NUM_PINS = ARRAY_SIZE(pins);
#define CASSERT(X) typedef byte STATIC_ASSERT[X?1:-1]
CASSERT(NUM_PINS == 11);

typedef enum 
 {  BUTTON_NONE,
    TOP_RIGHT,
    BUTT_RIGHT,
    MID_MID,
    ARR_DOWN,
    ARR_UP,
    MID_LEFT,
    BUTT_LEFT,
    TOP_MIDV,
    TOP_LEFT,
    MID_RIGHT,
    BUTT_MID  }
 BUTTONS;

 /*********************
  * logic
  *********************/
  
bool currentPressed = false;
int currentButton = 0;
int pendingButton = 0;

boolean isPressed(CapPin* pin) {
  long total = pin->readPin(SAMPLES);

#ifdef DEUBUG
  if (total > THRESH) {
    Serial.print("Total: ");
    Serial.println(total);
  }
#endif

  return total > THRESH;  
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Hello from Arduino!");
  
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
  Wire.begin(4);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
}


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
    
    currentButton = i+1;
    pendingButton = currentButton;
    
#ifdef DEBUG
    Serial.print("Button pressed: ");
    Serial.print(i);
    Serial.print(" Pin: ");
    Serial.println(rawPin[i]);
#endif
  } else if (!isButtonPressed && (currentPressed)) {
    currentPressed = false;
    currentButton = 0;
    digitalWrite(LEDPIN, LOW);
    Serial.println("stop");
  }
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent()
{
  Serial.println("request event!");
  int buttonToSend = pendingButton;
  pendingButton = 0;
  
  if (buttonToSend == 0) {
    buttonToSend = currentButton;
  }
  
  Wire.write(buttonToSend); // respond with message of 6 bytes
  
  // as expected by master
}
