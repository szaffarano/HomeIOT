#include <MyPrivateConfig.h>

#define MY_NODE_ID 105

#define MY_RF24_CS_PIN  7
#define MY_RF24_CE_PIN  8

#include <SPI.h>
#include <MySensors.h>
#include <IRremote.h>
#include <ACNoblex.h>
#include <avr/power.h>

#define SKETCH_NAME "DevDuino "
#define SKETCH_VERSION "v1.0"

#define LED_PIN   9
#define TEMP_PIN  A2

#define SLEEP_TIME (60L * 1000)

#define CHILD_ID_LED        1
#define CHILD_ID_BATTERY    2
#define CHILD_ID_TEMP       3

/* ========= global variables ========= */
// led state
boolean on = false;

// battery level
long lastBattery = -100;
float lastTemp = -1;

// messages
MyMessage msgLED(CHILD_ID_LED, V_LIGHT);
MyMessage msgBatt(CHILD_ID_BATTERY, V_VOLTAGE);
MyMessage msgTemp(CHILD_ID_TEMP, V_TEMP);

/* ========= function prototypes ========= */
void receive(const MyMessage &message);
void sendBattLevel();
void sendTemp();
float readMCP9700(int pin, float offset);

void setup() {
  clock_prescale_set(clock_div_8);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, on ? HIGH : LOW);
}

void presentation()  {
  sendSketchInfo(SKETCH_NAME, SKETCH_VERSION);

  // AC presentations
  present(CHILD_ID_LED, S_BINARY, "led");
  present(CHILD_ID_BATTERY, S_POWER, "battery");
  present(CHILD_ID_TEMP, S_TEMP, "temperature");
}

void loop() {
  /*
  sleep(SLEEP_TIME);
  sendBattLevel();
  sendTemp();
  */
  //hwSleep(digitalPinToInterrupt(2), LOW, 500);
  smartSleep(30000);
}

void receive(const MyMessage &message) {
  switch (message.sensor) {
  case CHILD_ID_LED:
    if (message.getCommand() == C_SET && strlen(message.data) ) {
      on = message.getBool();
      send(msgLED.set(on));
    }
    digitalWrite(LED_PIN, on ? HIGH : LOW);
    break;
  case CHILD_ID_BATTERY:
    if (message.getCommand() == C_REQ) {
      sendBattLevel();
    }
    break;
  case CHILD_ID_TEMP:
    if (message.getCommand() == C_REQ) {
      sendTemp();
    }
  default:
    Serial.print("Unknown sensor: ");
    Serial.println(message.sensor);
    break;
  }

}

void sendBattLevel() {
  uint16_t vcc = hwCPUVoltage();
  if (vcc != lastBattery) {
    lastBattery = vcc;

    send(msgBatt.set(vcc));

    // Calculate percentage

    vcc = vcc - 1900; // subtract 1.9V from vcc, as this is the lowest voltage we will operate at

    long percent = vcc / 14.0;
    sendBatteryLevel(percent);
  }
}

void sendTemp() {
  static float TEMP_SENSE_OFFSET = -0.01;

  float temp = readMCP9700(TEMP_PIN,TEMP_SENSE_OFFSET); //temp pin and offset for calibration
  if (temp != lastTemp) {
    lastTemp = temp;
    send(msgTemp.set(temp, 2));
  }
}

float readMCP9700(int pin, float offset) {
  analogReference(INTERNAL);

  analogRead(A0); //perform a dummy read to clear the adc
  delay(20);

  for (int n = 0; n < 5; n++) {
    analogRead(pin);
  }

  int adc = analogRead(pin);
  float tSensor = ((adc * (1.1 / 1024.0)) - 0.5 + offset) * 100;
  float error = 244e-6 * (125 - tSensor) * (tSensor - -40.0) + 2E-12 * (tSensor - -40.0) - 2.0;
  float temp = tSensor - error;

  return temp;
}
