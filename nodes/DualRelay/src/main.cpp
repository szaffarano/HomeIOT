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

#define BATTERY_SENSE_PIN A0

#define DELAY_MILLIS      300
#define LONG_DELAY_MILLIS (DELAY_MILLIS * 5)

// messages to send back the state to the controller
MyMessage msgRelayOne(RELAY_1_CHILD_ID, V_LIGHT);
MyMessage msgRelayTwo(RELAY_2_CHILD_ID, V_LIGHT);

// initialization flag
bool initialValueSent = false;

// Relays state
bool stateRelayOne     = false;
bool stateRelayTwo     = false;

// Battery state
uint8_t batteryPcnt = 0;

void before() {
  // nothing
}

inline uint8_t stateToLogicValue(bool);
uint8_t getBatteryLevel();

void setup() {
   analogReference(INTERNAL);

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
   sendSketchInfo(SKETCH_NAME, SKETCH_VERSION);
   wait(DELAY_MILLIS);

   present(RELAY_1_CHILD_ID, S_BINARY, RELAY_1_CHILD_DESC);
   wait(DELAY_MILLIS);

   present(RELAY_2_CHILD_ID, S_BINARY, RELAY_2_CHILD_DESC);
   wait(DELAY_MILLIS);

   sendBatteryLevel(getBatteryLevel());
   wait(DELAY_MILLIS);
}


void loop() {
   if (!initialValueSent) {
      send(msgRelayOne.set(stateRelayOne));
      wait(DELAY_MILLIS);

      request(RELAY_1_CHILD_ID, V_STATUS);
      wait(DELAY_MILLIS * 5, C_SET, V_STATUS);

      send(msgRelayTwo.set(stateRelayTwo));
      wait(DELAY_MILLIS);Serial.flush();

      request(RELAY_2_CHILD_ID, V_STATUS);
      send(msgRelayTwo.set(stateRelayTwo));
      wait(DELAY_MILLIS * 5, C_SET, V_STATUS);
   } 

   float bat = getBatteryLevel();
   if (bat != batteryPcnt) {
      batteryPcnt = bat;
      sendBatteryLevel(batteryPcnt);
      wait(DELAY_MILLIS);
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
          wait(DELAY_MILLIS);

          saveState(RELAY_1_CHILD_ID, stateRelayOne);
          break;
        case RELAY_2_CHILD_ID:
          stateRelayTwo = message.getBool();

          digitalWrite(RELAY_2_PIN, stateToLogicValue(stateRelayTwo));

          send(msgRelayTwo.set(stateRelayTwo));
          wait(DELAY_MILLIS);

          saveState(RELAY_2_CHILD_ID, stateRelayTwo);
          break;
      }
   }
}

inline uint8_t stateToLogicValue(bool s) {
   return s ? LOW : HIGH;
}

uint8_t getBatteryLevel() {
   // get the battery Voltage
   int vcc = analogRead(BATTERY_SENSE_PIN);
   
   // 1M, 470K divider across battery and using internal ADC ref of 1.1V
   // Sense point is bypassed with 0.1 uF cap to reduce noise at that point
   // ((1e6+470e3)/470e3)*1.1 = Vmax = 3.44 Volts
   // 3.44/1023 = Volts per bit = 0.003363075

   return (vcc * 0.003363075 * 100);
}
