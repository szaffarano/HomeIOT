#include <MyPrivateConfig.h>
#include <MySensors.h>

#define SKETCH_NAME    "Dual Relay"
#define SKETCH_VERSION "1.0.0"

#define RELAY_1_PIN        3
#define RELAY_1_CHILD_ID   1
#define RELAY_1_CHILD_DESC "Relay 1"

#define RELAY_2_PIN        4
#define RELAY_2_CHILD_ID   2
#define RELAY_2_CHILD_DESC "Relay 2"

// messages to send back the state to the controller
MyMessage msgRelayOne(RELAY_1_CHILD_ID, V_LIGHT);
MyMessage msgRelayTwo(RELAY_2_CHILD_ID, V_LIGHT);

// initialization flag
bool initialValueSent = false;

// Relays state
bool stateRelayOne     = false;
bool stateRelayTwo     = false;

void before() {
  // nothing
}

inline uint8_t stateToLogicValue(bool);

void setup() {
   // set relays pin mode
   pinMode(RELAY_1_PIN, OUTPUT);
   pinMode(RELAY_2_PIN, OUTPUT);

   // get relay state from eeprom and write it to the pin
   stateRelayOne = loadState(RELAY_1_CHILD_ID);
   stateRelayTwo = loadState(RELAY_2_CHILD_ID);

   digitalWrite(RELAY_1_PIN, stateToLogicValue(stateRelayOne));
   digitalWrite(RELAY_2_PIN, stateToLogicValue(stateRelayTwo));
}

void presentation() {
   wait(200);
   sendSketchInfo(SKETCH_NAME, SKETCH_VERSION);

   wait(200);
   present(RELAY_1_CHILD_ID, S_BINARY, RELAY_1_CHILD_DESC);

   wait(200);
   present(RELAY_2_CHILD_ID, S_BINARY, RELAY_2_CHILD_DESC);
}


void loop() {
   if (!initialValueSent) {
      send(msgRelayOne.set(stateRelayOne));
      request(RELAY_1_CHILD_ID, V_STATUS);
      wait(200, C_SET, V_STATUS);

      send(msgRelayTwo.set(stateRelayTwo));
      request(RELAY_2_CHILD_ID, V_STATUS);
      wait(200, C_SET, V_STATUS);
   }

}

void receive(const MyMessage &message) {
   if (message.type == V_STATUS) {
      initialValueSent = true;

      switch(message.sensor) {
        case RELAY_1_CHILD_ID:
          stateRelayOne = message.getBool();

          digitalWrite(RELAY_1_PIN, stateToLogicValue(stateRelayOne));
          send(msgRelayOne.set(stateRelayOne));
          saveState(RELAY_1_CHILD_ID, stateRelayOne);
          break;
        case RELAY_2_CHILD_ID:
          stateRelayTwo = message.getBool();

          digitalWrite(RELAY_2_PIN, stateToLogicValue(stateRelayTwo));
          send(msgRelayTwo.set(stateRelayTwo));
          saveState(RELAY_2_CHILD_ID, stateRelayTwo);
          break;
      }
   }
}

inline uint8_t stateToLogicValue(bool s) {
  return s ? LOW : HIGH;
}
