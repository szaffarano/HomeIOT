/**
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2015 Sensnology AB
 * Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 *******************************
 *
 * REVISION HISTORY
 * Version 1.0 - Henrik Ekblad
 *
 * DESCRIPTION
 * The ESP8266 MQTT gateway sends radio network (or locally attached sensors) data to your MQTT broker.
 * The node also listens to MY_MQTT_TOPIC_PREFIX and sends out those messages to the radio network
 *
 * LED purposes:
 * - To use the feature, uncomment WITH_LEDS_BLINKING in MyConfig.h
 * - RX (green) - blink fast on radio message recieved. In inclusion mode will blink fast only on presentation recieved
 * - TX (yellow) - blink fast on radio message transmitted. In inclusion mode will blink slowly
 * - ERR (red) - fast blink on error during transmission error or recieve crc error
 *
 * See http://www.mysensors.org/build/esp8266_gateway for wiring instructions.
 * nRF24L01+  ESP8266
 * VCC        VCC
 * CE         GPIO4
 * CSN/CS     GPIO15
 * SCK        GPIO14
 * MISO       GPIO12
 * MOSI       GPIO13
 *
 * Not all ESP8266 modules have all pins available on their external interface.
 * This code has been tested on an ESP-12 module.
 * The ESP8266 requires a certain pin configuration to download code, and another one to run code:
 * - Connect REST (reset) via 10K pullup resistor to VCC, and via switch to GND ('reset switch')
 * - Connect GPIO15 via 10K pulldown resistor to GND
 * - Connect CH_PD via 10K resistor to VCC
 * - Connect GPIO2 via 10K resistor to VCC
 * - Connect GPIO0 via 10K resistor to VCC, and via switch to GND ('bootload switch')
 *
  * Inclusion mode button:
 * - Connect GPIO5 via switch to GND ('inclusion switch')
 *
 * Hardware SHA204 signing is currently not supported!
 *
 * Make sure to fill in your ssid and WiFi password below for ssid & pass.
 */

/*
 *	typedef enum {
 *	C_PRESENTATION = 0, //!< Sent by a node when they present attached sensors. This is usually done in presentation() at startup.
 *	C_SET = 1, //!< This message is sent from or to a sensor when a sensor value should be updated.
 *	C_REQ = 2, //!< Requests a variable value (usually from an actuator destined for controller).
 *	C_INTERNAL = 3, //!< Internal MySensors messages (also include common messages provided/generated by the library).
 *	C_STREAM = 4 //!< For firmware and other larger chunks of data that need to be divided into pieces.
 * } mysensor_command;
 */
// Topic structure: MY_MQTT_PUBLISH_TOPIC_PREFIX/NODE-ID/SENSOR-ID/CMD-TYPE/ACK-FLAG/SUB-TYPE

#include <EEPROM.h>
#include <SPI.h>
#include <MyPrivateConfig.h>

#define MY_GATEWAY_ESP8266

#define MY_WITH_LEDS_BLINKING_INVERSE

#define MY_DEFAULT_ERR_LED_PIN 16
#define MY_DEFAULT_RX_LED_PIN  0
#define MY_DEFAULT_TX_LED_PIN  5

#if defined(MY_USE_UDP)
  #include <WiFiUDP.h>
#else
  #include <ESP8266WiFi.h>
#endif

#include <MySensor.h>

#include <ESP8266WiFi.h>
#include <MySensor.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

void setup() {
	WiFi.mode(WIFI_STA);

	ArduinoOTA.onStart([]() {
		Serial.println("Start");
	});

	ArduinoOTA.onEnd([]() {
		Serial.println("\nEnd");
	});
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
	});
	ArduinoOTA.onError([](ota_error_t error) {
		Serial.printf("Error[%u]: ", error);
		if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
		else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
		else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
		else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
		else if (error == OTA_END_ERROR) Serial.println("End Failed");
	});

	ArduinoOTA.begin();

	Serial.println("Ready");
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());

}

void presentation() {
}

void loop() {
	ArduinoOTA.handle();
}
