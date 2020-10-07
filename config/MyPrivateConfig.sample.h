#ifndef MyPrivateConfig_h
#define MyPrivateConfig_h

#define MY_HOSTNAME "some.name"

#define MY_RADIO_RF24

#define MY_BAUD_RATE            115200

#define MY_WIFI_SSID     "xxxx"
#define MY_WIFI_PASSWORD "xxxx"

#define MY_DEFAULT_ERR_LED_PIN 1
#define MY_DEFAULT_RX_LED_PIN  2
#define MY_DEFAULT_TX_LED_PIN  3

#define MY_RF24_PA_LEVEL        RF24_PA_HIGH
#define MY_RF24_DATARATE        RF24_1MBPS
#define MY_RF24_CHANNEL         9
#define MY_RF24_BASE_RADIO_ID   0x12,0x34,0x56,0x78,0x9A

#define MY_WITH_LEDS_BLINKING_INVERSE

#ifdef DEVDUINO_NODE
#define MY_RF24_CS_PIN  7
#define MY_RF24_CE_PIN  8
#endif

#if defined(MQTT_GW)

#define MY_GATEWAY_MQTT_CLIENT
#define MY_GATEWAY_ESP8266

#define MY_MQTT_PUBLISH_TOPIC_PREFIX   "xxx"
#define MY_MQTT_SUBSCRIBE_TOPIC_PREFIX "xxx"
#define MY_MQTT_CLIENT_ID              "xxx"
#define MY_PORT                        1234
#define MY_CONTROLLER_IP_ADDRESS      127, 0, 0, 1

#else

#define MY_PORT       4321
#define MY_GATEWAY_MAX_CLIENTS    4

#endif // GW type

#endif // lib
