#include <MyPrivateConfig.h>
#include <MySensors.h>
#include <Bounce2.h>

#define SKETCH_NAME    "Light automation"
#define SKETCH_VERSION "1.0.0"

#define RELAY_PIN        4
#define RELAY_CHILD_ID   1
#define RELAY_CHILD_DESC "LED"

// Time in millis holding the button to consider it a state witch
#define CLICK_TIME_MILLIS 300

#define BUTTON_PIN 3

#define BOUNCE_INTERVAL_MILLIS 20

MyMessage msgRelay(RELAY_CHILD_ID, V_LIGHT);
Bounce debouncer = Bounce();

bool initialValueSent = false;
bool            isOn = false;

bool clicked;
unsigned long buttonPressTimeStamp;

void updateLEDState(bool);

void before() {
  // nothing
}

void setup() {
   // set relay pin mode
   pinMode(RELAY_PIN, OUTPUT);

   // set button pin mode and internal pull-up
   pinMode(BUTTON_PIN, INPUT);
   digitalWrite(BUTTON_PIN, HIGH);
  
   // configure button debouncer
   debouncer.attach(BUTTON_PIN);
   debouncer.interval(BOUNCE_INTERVAL_MILLIS);

   // get relay state from eeprom and write it to the pin
   isOn = loadState(RELAY_CHILD_ID);
   digitalWrite(RELAY_PIN, isOn ? HIGH : LOW);
}

void presentation() {
   wait(200);
   sendSketchInfo(SKETCH_NAME, SKETCH_VERSION);

   wait(200);
   present(RELAY_CHILD_ID, S_BINARY, RELAY_CHILD_DESC);

   wait(200);
}


void loop() {
   bool changed = debouncer.update();

   if (!initialValueSent) {
      send(msgRelay.set(isOn ? HIGH : LOW));
      request(RELAY_CHILD_ID, V_STATUS);
      wait(1000, C_SET, V_STATUS);
   }

   if (changed) {
      int value = debouncer.read();
      if (value == LOW) {
         // button released
         clicked = (millis() - buttonPressTimeStamp) >= CLICK_TIME_MILLIS;
      } else {
         // button pressed
         clicked = false;
         buttonPressTimeStamp = millis();
      }
   }
  
   if  (clicked) {
      clicked = false;
      updateLEDState(!isOn);
   }

}

void receive(const MyMessage &message) {
   if (message.type == V_STATUS) {
      initialValueSent = true;

      updateLEDState(message.getBool());
   }
}

void updateLEDState(bool newState) {
   isOn = newState;
   uint8_t ledState = isOn ? HIGH : LOW;
   digitalWrite(RELAY_PIN, ledState);
   send(msgRelay.set(ledState));
   saveState(RELAY_CHILD_ID, isOn);
} 
