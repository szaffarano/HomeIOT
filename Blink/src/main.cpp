#include <MyPrivateConfig.h>
#include <MySensors.h>

#define SKETCH_NAME    "Light automation"
#define SKETCH_VERSION "1.0.0"

#define LED_PIN        4
#define LED_CHILD_ID   1
#define LED_CHILD_DESC "LED"

MyMessage msgLED(LED_CHILD_ID, V_LIGHT);

bool initialValueSent = false;
bool            isOn = false;

void before() {
}

void setup() {
   pinMode(LED_PIN, OUTPUT);

   isOn = loadState(LED_CHILD_ID);

   digitalWrite(LED_PIN, isOn ? HIGH : LOW);
}

void presentation() {
   wait(200);
   sendSketchInfo(SKETCH_NAME, SKETCH_VERSION);

   wait(200);
   present(LED_CHILD_ID, S_BINARY, LED_CHILD_DESC);

   wait(200);
}


void loop() {
   if (!initialValueSent) {
      send(msgLED.set(isOn ? HIGH : LOW));
      request(LED_CHILD_ID, V_STATUS);
      wait(2000, C_SET, V_STATUS);
   }
}

void receive(const MyMessage &message) {
    if (message.type == V_STATUS) {
       initialValueSent = true;

       isOn = message.getBool();
       digitalWrite(LED_PIN, isOn ? HIGH : LOW);
       send(msgLED.set(isOn ? HIGH : LOW));
       saveState(LED_CHILD_ID, isOn);
    }
}
