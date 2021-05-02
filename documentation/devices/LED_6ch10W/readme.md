# multi channel high power led driver

## brief description

LED driver with PWM input like the PT4115 often used in build in drivers of led bulbs.
You have to either hack the DIM Pin or buy a driver module with broken out PWM pin

* This is a basic implementation with fixed fading duration and frequency
* 100hz PWM Frequency produces a flicker free dimming. PWM on a ESP8266 is software generated! Default 1Khz produces some random flicker (WiFi?)
* [Datasheet PT4115](datasheet.lcsc.com/szlcsc/1811061811_PowTech-CR-PowTech-Shanghai-PT4115B89E-B_C15425.pdf) 

**Be aware!** The PT4115 has no reverse polarity protection! It burst in flames if you mess things up. A full bridge rectifier can prevent this:

## hardware

* ESP8266 (i.e. Wemos D2 mini)
* 1 or more PT4115 or similar LED drivers
* 5V step down converter
* 6-30V power supply (suitable for your LED chips)
* high power LED COB modules
* heat sink for LEDs (recommended)

## plugins

* a_PWM

## code

### custom functions

*none*

### callback functions 

*none*

### setup

```C++
bool deviceSetup(void) {

    myDevice.init({"LED-6ch10W-01","LED Dimmer"});

    // node attached to the hardware by property plugins using the default input handler
    myDevice.addNode({"DIMMERS","6ch PWM LED Dimmer", "LED Dimmer"})
        ->addProperty({"ch1","Channel 1", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,1,10,600,0}, PWM_ID, D3)    // GPIO00
        ->addProperty({"ch2","Channel 2", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,1,10,600,0}, PWM_ID, D4)    // GPIO02
        ->addProperty({"ch3","Channel 3", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,1,10,600,0}, PWM_ID, D5)    // GPIO14
        ->addProperty({"ch4","Channel 4", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,1,10,600,0}, PWM_ID, D6)    // GPIO12
        ->addProperty({"ch5","Channel 5", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,1,10,600,0}, PWM_ID, D7)    // GPIO13
        ->addProperty({"ch6","Channel 6", "%", DATATYPE_FLOAT, RETAINED, "0:100",SETTABLE,1,10,600,0}, PWM_ID, D8);   // GPIO15

    myDevice.setOption("DIMMERS","ch*",a_PWMdimmer::PWM_FREQUENCY , 100); // 100hz PWM Frequency as PWM on a ESP8266 is software generated. 1Khz produces some random flicker (WiFi?)
    myDevice.setOption("DIMMERS","ch*",a_PWMdimmer::PWM_FADE_DURATION_MS , 100); // 100ms transition time for soft start/stop
    return true;
}
```

### loop

*none*