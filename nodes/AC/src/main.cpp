#include <MyPrivateConfig.h>

#define MY_NODE_ID 103

#include <SPI.h>
#include <MySensors.h>
#include <IRremote.h>
#include <ACNoblex.h>
#include <avr/power.h>

#define SKETCH_NAME "AC Controller "
#define SKETCH_VERSION "v1.0"

// Wait times
#define LONG_WAIT 500
#define SHORT_WAIT 50

#define SLEEP_TIME  0

// battery
#define BATTERY_SENSE_PIN   A0

// AC
#define CHILD_ID_AC_POWER   1
#define CHILD_ID_AC_LIGHT   2
#define CHILD_ID_AC_SWING   3
#define CHILD_ID_AC_FAN     4
#define CHILD_ID_AC_MODE    5
#define CHILD_ID_AC_TEMP    6
#define CHILD_ID_AC_STATE   7
#define CHILD_ID_BATTERY    8

#define MAGIC_ID            0
#define MAGIC_VALUE         0x71

/* ========= global variables ========= */
// battery level
int oldBatteryPcnt = 0;

// messages
MyMessage msgACPower(CHILD_ID_AC_POWER, V_LIGHT);
MyMessage msgACLight(CHILD_ID_AC_LIGHT, V_LIGHT);
MyMessage msgACSwing(CHILD_ID_AC_SWING, V_LIGHT);
MyMessage msgACFan(CHILD_ID_AC_FAN, V_VAR1);
MyMessage msgACMode(CHILD_ID_AC_MODE, V_VAR1);
MyMessage msgACTemp(CHILD_ID_AC_TEMP, V_TEMP);
MyMessage msgState(CHILD_ID_AC_STATE, V_VAR1);

// remote control
IRsend irsend;
ACNoblex ac(&irsend);

/* ========= function prototypes ========= */
void receive(const MyMessage &message);
void wakeUp();
void updateBatteryLevel();

void setup() {
  //clock_prescale_set(clock_div_8);

  // 1.1 V internal reference
  analogReference(INTERNAL);

  uint8_t magic = loadState(MAGIC_ID);
  if (magic != MAGIC_VALUE) {
    Serial.print("Saved sate not found, burning default values...");
    saveState(CHILD_ID_AC_POWER, ac.isOn());
    saveState(CHILD_ID_AC_LIGHT, ac.getLight());
    saveState(CHILD_ID_AC_SWING, ac.getSwing());
    saveState(CHILD_ID_AC_FAN, ac.getFanSpeed());
    saveState(CHILD_ID_AC_MODE, ac.getMode());
    saveState(CHILD_ID_AC_TEMP, ac.getTemp());
    saveState(MAGIC_ID, MAGIC_VALUE);
  } else {
    Serial.println("Restoring saved state");
    if (loadState(CHILD_ID_AC_POWER)) {
      ac.on();
    }
    ac.setLight(loadState(CHILD_ID_AC_LIGHT));
    ac.setSwing(loadState(CHILD_ID_AC_SWING));
    ac.setFanSpeed(loadState(CHILD_ID_AC_FAN));
    ac.setMode(loadState(CHILD_ID_AC_MODE));
    ac.setTemp(loadState(CHILD_ID_AC_TEMP));
  }

  Serial.println("AC Controller Started");
}

void presentation()  {
  sendSketchInfo(SKETCH_NAME, SKETCH_VERSION);
  wait(LONG_WAIT);

  // AC presentations
  present(CHILD_ID_BATTERY,  S_POWER,  "battery");
  wait(SHORT_WAIT);
  present(CHILD_ID_AC_POWER,  S_BINARY,  "ac-power");
  wait(SHORT_WAIT);
  present(CHILD_ID_AC_LIGHT,  S_LIGHT,   "ac-light");
  wait(SHORT_WAIT);
  present(CHILD_ID_AC_SWING,  S_BINARY,  "ac-swing");
  wait(SHORT_WAIT);
  present(CHILD_ID_AC_FAN,    S_CUSTOM,  "ac-fan");
  wait(SHORT_WAIT);
  present(CHILD_ID_AC_MODE,   S_CUSTOM,  "ac-mode");
  wait(SHORT_WAIT);
  present(CHILD_ID_AC_TEMP,   S_BINARY,  "ac-temp");
  wait(SHORT_WAIT);
  present(CHILD_ID_AC_STATE,  S_CUSTOM,  "ac-state");
  wait(SHORT_WAIT);

}

void loop() {
  hwSleep(digitalPinToInterrupt(2), LOW, SLEEP_TIME);
  //smartSleep(5 * 1000);
  //send( msgACTemp.set(ac.getTemp()));

  //smartSleep(5000);
}

void receive(const MyMessage &message) {
  switch (message.sensor) {
  case CHILD_ID_AC_POWER:
    if (message.getCommand() == C_SET) {
      if (message.getBool()) {
        Serial.println("power on");
        ac.on();
      } else {
        Serial.println("power off");
        ac.off();
      }
      ac.sendCommand();
    } else if (message.getCommand() == C_REQ) {
      send( msgACPower.set(ac.isOn()));
    }
    break;
  case CHILD_ID_AC_LIGHT:
    if (message.getCommand() == C_SET) {
      Serial.print("light ");
      Serial.println(message.getBool());
      ac.setLight(message.getBool());
      saveState(CHILD_ID_AC_LIGHT, ac.getLight());
      ac.sendCommand();
    } else if (message.getCommand() == C_REQ) {
      send( msgACLight.set(ac.getLight()));
    }
    break;
  case CHILD_ID_AC_SWING:
    if (message.getCommand() == C_SET) {
      ac.setSwing(message.getBool());
      saveState(CHILD_ID_AC_SWING, ac.getSwing());
      ac.sendCommand();
    } else if (message.getCommand() == C_REQ) {
      send( msgACSwing.set(ac.getSwing()));
    }
    break;
  case CHILD_ID_AC_FAN:
    if (message.getCommand() == C_SET) {
      ac.setFanSpeed(message.getInt());
      saveState(CHILD_ID_AC_FAN, ac.getFanSpeed());
      ac.sendCommand();
    } else if (message.getCommand() == C_REQ) {
      send( msgACFan.set(ac.getFanSpeed()));
    }
    break;
  case CHILD_ID_AC_MODE:
    if (message.getCommand() == C_SET) {
      ac.setMode(message.getInt());
      saveState(CHILD_ID_AC_MODE, ac.getMode());
      ac.sendCommand();
    } else if (message.getCommand() == C_REQ) {
      send( msgACMode.set(ac.getMode()));
    }
    break;
  case CHILD_ID_AC_TEMP:
    if (message.getCommand() == C_SET) {
      if (strlen(message.data) > 0 && ac.getTemp() != message.getInt()) {
        ac.setTemp(message.getInt());
        saveState(CHILD_ID_AC_TEMP, ac.getTemp());
      }
      ac.sendCommand();
    } else if (message.getCommand() == C_REQ) {
      send( msgACTemp.set(ac.getTemp()));
    }
    break;
  case CHILD_ID_BATTERY:
    switch (message.getCommand()) {
    case C_REQ:
      updateBatteryLevel();
      sendBatteryLevel(oldBatteryPcnt);
      break;
    case C_PRESENTATION:
    case C_SET:
    case C_INTERNAL:
    case C_STREAM:
    case C_RESERVED_5:
    case C_RESERVED_6:
    case C_INVALID_7:
      Serial.println("Unimplemented'");
      break;
    }
    break;
 default:
    Serial.println("Unknown'");
    break;
  }
}


void updateBatteryLevel() {
  // get the battery Voltage
  int sensorValue = analogRead(BATTERY_SENSE_PIN);

  // 1M, 470K divider across battery and using internal ADC ref of 1.1V
  // Sense point is bypassed with 0.1 uF cap to reduce noise at that point
  // ((1e6+470e3)/470e3)*1.1 = Vmax = 3.44 Volts
  // 3.44/1023 = Volts per bit = 0.003363075
  //float batteryV  = sensorValue * 0.003363075;
  int batteryPcnt = sensorValue / 10;

  if (oldBatteryPcnt != batteryPcnt) {
    // Power up radio after sleep
    oldBatteryPcnt = batteryPcnt;
  }
}
