#ifndef MyPrivateConfig_h
#define MyPrivateConfig_h

//#define MY_DEBUG

//#define MY_USE_UDP
//#define MY_DEBUG_VERBOSE

#define MY_HOSTNAME "some.fqdn"

#define MY_RADIO_RF24

#define MY_BAUD_RATE            nnnnnnnn

#define MY_WIFI_SSID     "xxxx"
#define MY_WIFI_PASSWORD "xxxx"

#define MY_DEFAULT_ERR_LED_PIN 0
#define MY_DEFAULT_RX_LED_PIN  0
#define MY_DEFAULT_TX_LED_PIN  0

#define MY_RF24_PA_LEVEL        XXX
#define MY_RF24_DATARATE        XXX
#define MY_RF24_CHANNEL         N
#define MY_RF24_BASE_RADIO_ID   a,b,c,d

#define MY_WITH_LEDS_BLINKING_INVERSE

#ifdef DEVDUINO_NODE
#define MY_RF24_CS_PIN  0
#define MY_RF24_CE_PIN  0
#endif

/** gateway stuff */
#if defined(MQTT_GW)

#define MY_GATEWAY_MQTT_CLIENT

#define MY_MQTT_PUBLISH_TOPIC_PREFIX   "xxxx"
#define MY_MQTT_SUBSCRIBE_TOPIC_PREFIX "xxxx"
#define MY_MQTT_USER                   "xxxx"
#define MY_MQTT_PASSWORD               "xxxx"
#define MY_MQTT_CLIENT_ID              "xxxx"

#define MY_CONTROLLER_URL_ADDRESS   "xxxx"
#define MY_PORT       0

#else

#define MY_PORT                0
#define MY_GATEWAY_MAX_CLIENTS 0

#endif // GW type

#endif // lib
