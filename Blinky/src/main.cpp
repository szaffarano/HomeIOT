#define MY_DEBUG
#define MY_RADIO_NRF24

#include <MyPrivateConfig.h>

#define MY_NODE_ID 103

#include <SPI.h>
#include <MySensor.h>
#include <IRremote.h>
#include <ACNoblex.h>
#include <avr/power.h>

#define SKETCH_NAME "Blinky "
#define SKETCH_VERSION "v1.0"

// Wait times
#define LONG_WAIT         500
#define SHORT_WAIT        50

// blinky led
#define LED_PIN           3

// deep sleep time -> 0 = forever
#define SLEEP_TIME        0

// sensors's ids
#define CHILD_ID_LED      1
#define CHILD_ID_BATTERY  2

// battery
#define BATTERY_SENSE_PIN A0

/* ========= global variables ========= */
// led state
boolean on = true;

// battery level
int oldBatteryPcnt = 0;

// messages
MyMessage msgLED(CHILD_ID_LED,      V_LIGHT);

/* ========= function prototypes ========= */
void receive(const MyMessage &message);
void wakeUp();
void updateBatteryLevel();

void setup() {
  clock_prescale_set(clock_div_8);

  // 1.1 V internal reference
  analogReference(INTERNAL);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, on ? HIGH : LOW);

  Serial.println("Blinky Started");
}

void presentation()  {
  sendSketchInfo(SKETCH_NAME, SKETCH_VERSION);
  wait(LONG_WAIT);

  // AC presentations
  present(CHILD_ID_LED,     S_BINARY, "led");
  present(CHILD_ID_BATTERY, S_POWER,  "battery");

  wait(SHORT_WAIT);

  request(CHILD_ID_LED, V_STATUS);
}

void loop() {
  hwSleep(digitalPinToInterrupt(2), LOW, SLEEP_TIME);
}

void receive(const MyMessage &message) {
  switch (message.sensor) {
  case CHILD_ID_LED:
    switch (message.getCommand()) {
    case C_SET:
      on = message.getBool();
      digitalWrite(LED_PIN, on ? HIGH : LOW);
      break;
    case C_REQ:
      send(msgLED.set(on));
      break;
    }
    break;
  case CHILD_ID_BATTERY:
    switch (message.getCommand()) {
    case C_REQ:
      updateBatteryLevel();
      sendBatteryLevel(oldBatteryPcnt);
      break;
    }
    break;
  default:
    Serial.print("Unknown sensor: ");
    Serial.println(message.sensor);
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