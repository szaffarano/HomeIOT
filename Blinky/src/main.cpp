#include <MyPrivateConfig.h>

#include <SPI.h>
#include <MySensors.h>

#define CHILD_ID_LIGHT      1

#define EPROM_LIGHT_STATE   1
#define EPROM_DIMMER_LEVEL  2
#define EPROM_WATT_LEVEL    3

#define LIGHT_OFF           0
#define LIGHT_ON            1

#define NODE_NAME           "Dimable Light"
#define NODE_VERSION        "1.0"
#define NODE_ACK            true

#define LED_PIN             3

int lastlightState = LIGHT_OFF;
int lastDimValue = 100;
int lastWattValue = 0;

MyMessage lightMsg(CHILD_ID_LIGHT, V_STATUS);
MyMessage dimmerMsg(CHILD_ID_LIGHT, V_PERCENTAGE);

void setCurrentState2Hardware();
void sendCurrentState2Controller();

void setup() {
  int lightState = loadState(EPROM_LIGHT_STATE);
  int dimValue = loadState(EPROM_DIMMER_LEVEL);
  int wattValue = loadState(EPROM_WATT_LEVEL);

  if (lightState == LIGHT_OFF || lightState == LIGHT_ON) {
    Serial.print("Updating lightState : "); Serial.println(lightState);
    lastlightState = lightState;
  }

  if (dimValue > 0 && dimValue <= 100) {
      Serial.print("Updating dimValue : "); Serial.println(dimValue);
      lastDimValue = dimValue;
  } else if (dimValue == 0) {
      lastlightState = LIGHT_OFF;
  }

  if (wattValue > 0) {
    Serial.print("Updating wattValue : "); Serial.println(wattValue);
    lastWattValue = wattValue;
  }

  pinMode(LED_PIN, OUTPUT);

  setCurrentState2Hardware();

  Serial.print("Node (id: "); Serial.print(getNodeId());
  Serial.println(") ready to receive messages..." );
}

void presentation() {
  sendSketchInfo(NODE_NAME, NODE_VERSION, NODE_ACK);
  present(CHILD_ID_LIGHT, S_DIMMER, "LED1");
  requestTime();
  sendCurrentState2Controller();
}

void loop() {
  wait(1000);
  //smartSleep(1000);
  //Serial.println("awaken!");
}

void receive(const MyMessage &message) {
  Serial.print("Incoming message, command: "); Serial.print(message.getCommand());
  Serial.print(", type: "); Serial.println(message.type);
  int value= atoi(message.data);

  switch(message.getCommand()) {
    case C_INTERNAL:
      switch(message.type) {
        case I_SKETCH_NAME:
          Serial.println("sending sketch name");
          //sendSketchInfo(NODE_NAME, NODE_VERSION, NODE_ACK);
          break;
        case I_SKETCH_VERSION:
          Serial.println("sending sketch version");
          //sendSketchInfo(NODE_NAME, NODE_VERSION, NODE_ACK);
          break;
        default:
          Serial.print("Unimplemented C_INTERNAL type: ");
          Serial.println(message.type);
      }
      break;
    case C_SET:
      switch (message.type) {
        case V_LIGHT:
          Serial.println( "V_LIGHT command received..." );
          if (value == LIGHT_OFF || value == LIGHT_ON) {
            lastlightState = value;
            saveState(EPROM_LIGHT_STATE, lastlightState);
          } else {
            Serial.print( "V_LIGHT data invalid (should be 0/1): ");
            Serial.println(value);
          }
        break;
        case V_DIMMER:
          Serial.println( "V_DIMMER command received..." );
          if (value >= 0 || value <= 100) {
            if (value == 0) {
              lastlightState = LIGHT_OFF;
            } else {
              lastlightState = LIGHT_ON;
            }
            lastDimValue = value;
            saveState(EPROM_DIMMER_LEVEL, lastDimValue);
          } else {
            Serial.print( "V_DIMMER data invalid (should be 0..100): ");
            Serial.println(value);
          }
        break;
        case V_WATT:
          Serial.println( "V_WATT command received..." );
          if (value > 0) {
            lastWattValue = value;
            saveState(EPROM_WATT_LEVEL, value);
          } else {
            Serial.print( "V_WATT data invalid (should be greater than 0): ");
            Serial.println(value);
          }
        break;
      }
      setCurrentState2Hardware();
      break;
    case C_REQ:
        Serial.println("Unimplemented C_REQ");
      break;
    default:
      Serial.print( "Invalid command received: "); Serial.println(message.type);
  }

}

void setCurrentState2Hardware() {
  if (lastlightState == LIGHT_OFF) {
     Serial.println("Light state: OFF");
     analogWrite(LED_PIN, 0);
  } else {
     Serial.print("Light state: ON, Level: ");Serial.print(lastDimValue);
     Serial.print(", watts: "); Serial.println(lastWattValue);

     analogWrite(LED_PIN, lastDimValue);
  }

  //sendCurrentState2Controller();
}

void sendCurrentState2Controller() {
  if (lastlightState == LIGHT_OFF || lastDimValue == 0) {
    send(dimmerMsg.set(0), NODE_ACK);
  } else {
    send(dimmerMsg.set(lastDimValue), NODE_ACK);
  }
}

void receiveTime(unsigned long ts) {
  Serial.print("Time received: "); Serial.println(ts);
}
