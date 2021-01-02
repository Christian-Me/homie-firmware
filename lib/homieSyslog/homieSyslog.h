#ifndef HOMIE_SYSLOG_H__
#define HOMIE_SYSLOG_H__

#include <ESP8266WiFi.h>
#include <Syslog.h>
#include <WiFiUdp.h>
#include <Homie.h>

HomieSetting<long> syslogProtocol("syslogProtocol", "Syslog Protocol");
HomieSetting<const char*> syslogServer("syslogServer", "Syslog Server");
HomieSetting<long> syslogPort("syslogProtocol", "Syslog Port");
HomieSetting<long> syslogLevel("syslogLevel", "Syslog Level");

// Syslog server connection info
#define SYSLOG_SERVER "192.168.2.14"
#define SYSLOG_PORT 514

WiFiUDP udpClient;

// Create a new empty syslog instance
Syslog * syslog;


void mSyslog_setup(uint8_t protocol = SYSLOG_PROTO_IETF) {

  syslogProtocol.setDefaultValue(0).setValidator([] (long candidate) {
    return (candidate >= 0) && (candidate <= 1);
  });
  syslogServer.setDefaultValue(SYSLOG_SERVER).setValidator([] (const char *candidate) {
    return true; // should be OK ;)
  });
  syslogPort.setDefaultValue(SYSLOG_PORT).setValidator([] (long candidate) {
    return (candidate >= 1) && (candidate <= 65535);
  });
  syslogLevel.setDefaultValue(LOG_DEBUG).setValidator([] (long candidate) {
    return (candidate >= LOG_EMERG) && (candidate <= LOG_DEBUG);
  });
  syslog = new Syslog(udpClient, protocol);
}

void mSysLog_start() {

  // prepare syslog configuration here (can be anywhere before first call of 
	// log/logf method)
  syslog->server(syslogServer.get(), syslogPort.get());
  syslog->deviceHostname(Homie.getConfiguration().deviceId);
  syslog->appName(Homie.getConfiguration().name);
  syslog->defaultPriority(LOG_KERN);
  syslog->logMask(LOG_UPTO(syslogLevel.get()));
  syslog->logf(LOG_DEBUG, "syslog: %s:%d",syslogServer.get(),syslogPort.get());
}

void mSyslog_loop() {
  // nothing to do here
}

#endif