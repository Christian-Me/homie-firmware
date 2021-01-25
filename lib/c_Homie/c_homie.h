#ifndef c_HOMIE_H__
#define c_HOMIE_H__
#include <Homie.h>
// #include "homieSyslog.h"
#include "../../include/datatypes.h"
#include "homie_property.h"
#include "homie_node.h"
#include "homie_device.h"

bool globalInputHandler(const HomieNode& node, const HomieRange& range, const String& property, const String& value);
void onHomieEvent(const HomieEvent& event);
void homieParameterSet(const __FlashStringHelper *nodeId, const __FlashStringHelper *parameterId, bool state);

#endif