#ifndef SEQUENCER_H__
#define SEQUENCER_H__
#include <Arduino.h>

typedef struct {
  const char* node;             //!< homie node id
  const char* property;         //!< homie node id
  float value;                  //!< set value
  unsigned long delay;          //!< delay in ms
} SequenceStep;

typedef bool (*SequencerCallback)(const char*, const char*, float);
typedef void (*SequencerDoneCallback)();

class Sequencer {
    SequenceStep* _sequence = NULL;
    SequencerCallback _callback = NULL;
    SequencerDoneCallback _doneCallback = NULL;
    unsigned long _timer = 0;
    uint8_t _sequenceCounter = 0;
    unsigned long sequenceStep(uint8_t step);
  public:
    bool startSequence(SequenceStep* sequence, SequencerCallback callback); //!< start a sequence by passing a sequence struct and a callback function
    bool startSequence(SequenceStep* sequence, SequencerCallback callback, 
                       SequencerDoneCallback doneCallback);                 //!< start a sequence by passing a sequence struct and a callback function + done callback
    bool sequenceRunning();                                                 //!< retuns true if a sequence is running
    void stopSequence();                                                    //!< stops a sequence
    void loop();                                                            //!< tie this method into your big loop
};

#endif