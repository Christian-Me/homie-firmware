#ifndef c_HOMIE_H__
#define c_HOMIE_H__
#include <Homie.h>
#include "homieSyslog.h"
#include "../include/datatypes.h"

bool normalOperation = false;

String getDatatype(DataTypes dataType) {
  switch(dataType) {
    case StringT: return "string";
    case IntegerT: return "integer";
    case FloatT: return "float";
    case BooleanT: return "boolean";
    case EnumT: return "enum";
    case ColorT: return "color";
    case DateTimeT: return "datetime";
    case DurationT: return "duration";
  }
  return "";
}

void onHomieEvent(const HomieEvent& event) {
  mySysLog_setAppName("CORE");
  switch(event.type) {
      case HomieEventType::MQTT_READY:
        myLog(LOG_INFO, F("MQTT ready!"));
        normalOperation = true;
      break;
      case HomieEventType::MQTT_DISCONNECTED:
        myLogf(LOG_INFO, "MQTT disconnected reason: %d", event.mqttReason);
        normalOperation = false;
      break;
      case HomieEventType::WIFI_CONNECTED:
        mSysLog_start();
        mySysLog_setDeviceName(Homie.getConfiguration().deviceId);
        myLogf(LOG_INFO, "WiFi ready ip: %s", event.ip.toString().c_str());
      break;
      case HomieEventType::WIFI_DISCONNECTED:
        normalOperation = false;
      break;    
      case HomieEventType::SENDING_STATISTICS:
        myLog(LOG_DEBUG, F("Sending stats"));
      break;
      case HomieEventType::OTA_STARTED:
        myLog(LOG_INFO, F("OTA started"));
        normalOperation = false;
      break;
      case HomieEventType::OTA_PROGRESS:
      break;
      case HomieEventType::OTA_SUCCESSFUL:
        myLog(LOG_INFO, F("OTA success"));
      break;
      case HomieEventType::OTA_FAILED:
        myLog(LOG_ERR, F("OTA failed"));
        normalOperation = true;
      break;
      case HomieEventType::ABOUT_TO_RESET:
        myLog(LOG_WARNING, F("OTA success"));
        normalOperation = false;
      break;
      case HomieEventType::MQTT_PACKET_ACKNOWLEDGED:
      break;
      case HomieEventType::NORMAL_MODE:
        myLog(LOG_INFO, F("Normal Mode"));
        normalOperation = true;
      break;
      case HomieEventType::READY_TO_SLEEP:
        myLog(LOG_WARNING, F("Ready to Sleep"));
      break;
      case HomieEventType::STANDALONE_MODE:
        myLog(LOG_INFO, F("Standalaone Mode"));
        normalOperation = true;
      break;
      case HomieEventType::CONFIGURATION_MODE:
        myLog(LOG_INFO, F("Configuration Mode"));
        normalOperation = false;
      break;
  }
}

void homieParameterSet(const __FlashStringHelper *nodeId, const __FlashStringHelper *parameterId, bool state) {
  String topic = "";
  topic = Homie.getConfiguration().mqtt.baseTopic;
  topic+=Homie.getConfiguration().deviceId;
  topic+="/";
  topic+=nodeId;
  topic+="/";
  topic+=parameterId;
  topic+="/set";
  Homie.getMqttClient().publish(topic.c_str(),2,true,(state ? "true" : "false"));
}

HomieNode* createHomieSensorNode(const SensorNode* sensorNode) {
  myLogf(LOG_INFO,F("deviceID : %s"),sensorNode->id);
  myLogf(LOG_INFO,F("Name     : %s"),sensorNode->name);
  myLogf(LOG_INFO,F("Type     : %s"),sensorNode->type);

  HomieNode static homieNode = HomieNode(sensorNode->id,sensorNode->name, sensorNode->type);

  for (uint8_t i=0; i<sensorNode->datapoints; i++) {
    myLogf(LOG_INFO,F("Advertize #%d id=%s name=%s unit=%s datatype=%s format=%s"),i,sensorNode->dataPoint[i].id,sensorNode->dataPoint[i].name,sensorNode->dataPoint[i].unit,sensorNode->dataPoint[i].datatype,sensorNode->dataPoint[i].format);

    homieNode.advertise(sensorNode->dataPoint[i].id)
      .setName(sensorNode->dataPoint[i].name)
      .setUnit(sensorNode->dataPoint[i].unit)
      .setRetained(sensorNode->dataPoint[i].retained)
      .setDatatype(sensorNode->dataPoint[i].datatype)
      .setFormat(sensorNode->dataPoint[i].format);
  }

  return &homieNode;
}

bool nodeInputHandler(const String& property, const HomieRange& range, const String& value) {
  myLogf(LOG_INFO,F("[Node] Received property: %s = %s"),property.c_str(),value.c_str());
  return true;
}

HomieNode* createHomieActuatorNode(const ActuatorNode* actuatorNode) {
  myLogf(LOG_INFO,F("deviceID : %s"),actuatorNode->id);
  myLogf(LOG_INFO,F("Name     : %s"),actuatorNode->name);
  myLogf(LOG_INFO,F("Type     : %s"),actuatorNode->type);

  HomieNode static homieNode = HomieNode(actuatorNode->id,actuatorNode->name, actuatorNode->type);

  for (uint8_t i=0; i<actuatorNode->actuators; i++) {
    myLogf(LOG_INFO,F("Advertize #%d id=%s name=%s unit=%s datatype=%s format=%s"),i,actuatorNode->actuator[i].id,actuatorNode->actuator[i].name,actuatorNode->actuator[i].unit,actuatorNode->actuator[i].datatype,actuatorNode->actuator[i].format);

    homieNode.advertise(actuatorNode->actuator[i].id)
      .setName(actuatorNode->actuator[i].name)
      .setUnit(actuatorNode->actuator[i].unit)
      .setRetained(actuatorNode->actuator[i].retained)
      .setDatatype(actuatorNode->actuator[i].datatype)
      .setFormat(actuatorNode->actuator[i].format)
      .settable();
  }

  return &homieNode;
}

#endif