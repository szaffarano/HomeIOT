#!/usr/bin/env bash

MOS_HOST="localhost"
MOS_TOP_IN="mygateway1-in"

if [ $# -ne 5 ]; then
	echo "Usage $0 <NODE> <SENSOR> <CMD> <VTYPE> <PAYLOAD>"
	echo
	printf "\tCMD = [SET | REQ]"
	printf "\tVTYPE = [V_TEMP | V_HUM | V_STATUS | V_LIGHT | ... ]"
	exit 1
fi

NODE="$1"
SENSOR="$2"
CMD="$3"
VTYPE="$4"
PAYLOAD="$5"

case "$CMD" in
  "SET")
    CMD="1"
    ;;
  "REQ")
    CMD="2"
    ;;
  *)
	  echo "$CMD: unknown command"
	  exit 2
esac

case "$VTYPE" in
  "V_TEMP")
	  VTYPE="0"
    ;;
  "V_HUM")
	  VTYPE="1"
    ;;
  "V_STATUS")
	  VTYPE="2"
    ;;
  "V_LIGHT")
	  VTYPE="2"
    ;;
  *)
	  echo "$VTYPE: unknown VTYPE"
	  exit 3
esac


# Topic structure: MY_MQTT_PUBLISH_TOPIC_PREFIX/NODE-ID/SENSOR-ID/CMD-TYPE/ACK-FLAG/SUB-TYPE
mosquitto_pub -h $MOS_HOST \
	-t "$MOS_TOP_IN/$NODE/$SENSOR/$CMD/0/$VTYPE" \
	-m "$PAYLOAD"
