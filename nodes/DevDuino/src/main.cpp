#define DEVDUINO_NODE

#include <MyPrivateConfig.h>

#include <MySensors.h>

#define SKETCH_NAME         "DevDuino"
#define SKETCH_VERSION      "v1.3"

#define LED_CHILD_ID        1
#define LED_CHILD_DESC      "LED"

#define TEMP_CHILD_ID       2
#define TEMP_CHILD_DESC     "Temperatura"

#define LED_PIN             9
#define TEMP_PIN            A2

#define ONE_SEC             1000L
#define ONE_MIN             (60L * ONE_SEC)

#define SMART_SLEEP_DELAY   (10 * ONE_MIN)

boolean is_on = false;
float oldBatteryPcnt = 0;
float oldTemp = 0;

MyMessage msgLED(LED_CHILD_ID, V_LIGHT);
MyMessage msgTemp(TEMP_CHILD_ID, V_TEMP);

bool initialValueSent = false;

long readVcc();

void setup() {
        clock_prescale_set(clock_div_8);

        pinMode(LED_PIN, OUTPUT);
        pinMode(TEMP_PIN, INPUT);

        is_on = loadState(LED_CHILD_ID);
        digitalWrite(LED_PIN, is_on ? HIGH : LOW);
}

void presentation()  {
        wait(200);
        sendSketchInfo(SKETCH_NAME, SKETCH_VERSION);
        wait(200);
        present(LED_CHILD_ID, S_BINARY, LED_CHILD_DESC);
        wait(500);
        present(TEMP_CHILD_ID, S_TEMP, TEMP_CHILD_DESC);
        wait(500);
}

void loop() {
        float vcc = readVcc() / 1000.0;

        float pcnt = ((float)(vcc / 3.44)) * 100;
        float temp = (((float)analogRead(TEMP_PIN) * vcc / 1024.0) - 0.5)/0.01;

        if (!initialValueSent) {
                send(msgLED.set(is_on ? HIGH : LOW));
                request(LED_CHILD_ID, V_STATUS);
                wait(3000, C_SET, V_STATUS);

                send(msgTemp.set(temp, 2));
                request(TEMP_CHILD_ID, V_STATUS);
                wait(3000, C_SET, V_STATUS);
        }

        if (oldBatteryPcnt != pcnt) {
                sendBatteryLevel(pcnt);
                oldBatteryPcnt = pcnt;
        }
        if (oldTemp != temp) {
                send(msgTemp.set(temp, 2));
                oldTemp = temp;
        }

        smartSleep(SMART_SLEEP_DELAY);
}

void receive(const MyMessage &message) {
        if (message.isAck()) {
                // ?
        }

        if (message.type == V_STATUS && message.sensor == LED_CHILD_ID) {
                initialValueSent = true;

                is_on = (bool)message.getInt();
                digitalWrite(LED_PIN, is_on ? HIGH : LOW);
                send(msgLED.set(is_on ? HIGH : LOW));
                saveState(LED_CHILD_ID, is_on);
        }
}

long readVcc() {
        // Read 1.1V reference against AVcc
        // set the reference to Vcc and the measurement to the internal 1.1V reference
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
        ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
        ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
        ADMUX = _BV(MUX3) | _BV(MUX2);
  #else
        ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif

        delay(75); // Wait for Vref to settle
        ADCSRA |= _BV(ADSC); // Start conversion
        while (bit_is_set(ADCSRA,ADSC)) ; // measuring

        uint8_t low  = ADCL;// must read ADCL first - it then locks ADCH
        uint8_t high = ADCH; // unlocks both

        long result = (high<<8) | low;

        result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
        return result; // Vcc in millivolts
}
