#include "sequencer.h"

bool Sequencer::startSequence(SequenceStep* sequence, SequencerCallback callback){
  _sequenceCounter = 0;
  _sequence = sequence;
  _callback = callback;
  _doneCallback = NULL;
  _timer = sequenceStep(_sequenceCounter);
  return true;
};

bool Sequencer::startSequence(SequenceStep* sequence, SequencerCallback callback, SequencerDoneCallback doneCallback) {
  startSequence(sequence, callback);
  _doneCallback = doneCallback;
};

unsigned long Sequencer::sequenceStep(uint8_t step){
  if (!_callback(_sequence[step].node,_sequence[step].property,_sequence[step].value)) {
    return 0; // cancel sequence
  };
  return (_sequence[step].delay>0) ? millis()+_sequence[step].delay : 0;
};

bool Sequencer::sequenceRunning() {
  return (_timer>0);
};

void Sequencer::stopSequence() {
  _timer = 0;
  _sequenceCounter = 0;
};

void Sequencer::loop() {
  if (_timer>0 && millis()>_timer) {
    _sequenceCounter++;
    _timer = sequenceStep(_sequenceCounter);
    if (_timer==0 & _doneCallback!=NULL) {
      _doneCallback();
    } 
  }
};