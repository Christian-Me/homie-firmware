#include "c_homie.h"

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