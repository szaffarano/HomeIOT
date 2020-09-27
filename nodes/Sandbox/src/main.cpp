#include <MyPrivateConfig.h>

#include <MySensors.h>

#define LED_PIN             3

#define SKETCH_NAME         "Sandbox "
#define SKETCH_VERSION      "v1.0"

#define LED_CHILD_ID        1
#define LED_CHILD_DESC      "LED"

#define BATTERY_SENSE_PIN   A0

MyMessage msgLED(LED_CHILD_ID, V_LIGHT);
boolean is_on = false;
float oldBatteryPcnt = 0;
bool initialValueSent = false;

float getBatteryLevel();

void setup() {
        clock_prescale_set(clock_div_8);

        analogReference(INTERNAL);

        pinMode(LED_PIN, OUTPUT);

        is_on = loadState(LED_CHILD_ID);
        digitalWrite(LED_PIN, is_on ? HIGH : LOW);
}

void presentation()  {
	delay(200);
        sendSketchInfo(SKETCH_NAME, SKETCH_VERSION);
	delay(200);
        present(LED_CHILD_ID, S_BINARY, LED_CHILD_DESC);
}

void loop() {
        if (!initialValueSent) {
                send(msgLED.set(is_on ? HIGH : LOW));
                wait(500);
                request(LED_CHILD_ID, V_STATUS);

                wait(2000, C_SET, V_STATUS);
        }
        float bat = getBatteryLevel();
        if (bat != oldBatteryPcnt) {
                sendBatteryLevel(bat);
                oldBatteryPcnt = bat;
        }

        smartSleep(10L * 60 * 1000);
}

void receive(const MyMessage &message) {
        if (message.isAck()) {
                // ack?
        }

        if (message.type == V_STATUS && message.sensor == LED_CHILD_ID) {
                is_on = (bool)message.getInt();
                digitalWrite(LED_PIN, is_on ? HIGH : LOW);
                send(msgLED.set(is_on ? HIGH : LOW));
                saveState(LED_CHILD_ID, is_on);

                initialValueSent = true;
        }

}

float getBatteryLevel() {
        // get the battery Voltage
        int sensorValue = analogRead(BATTERY_SENSE_PIN);

        // 1M, 470K divider across battery and using internal ADC ref of 1.1V
        // Sense point is bypassed with 0.1 uF cap to reduce noise at that point
        // ((1e6+470e3)/470e3)*1.1 = Vmax = 3.44 Volts
        // 3.44/1023 = Volts per bit = 0.003363075
        //float batteryV  = sensorValue * 0.003363075;
        return (float)(sensorValue / 10);
}
