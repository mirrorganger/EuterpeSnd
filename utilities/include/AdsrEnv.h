#ifndef UTILITES_ADSRENV_H
#define UTILITES_ADSRENV_H

#include "Fsm.h"

// TODO :Check refs.
// http://www.martin-finke.de/blog/articles/audio-plugins-011-envelopes/
// https://www.musicdsp.org/en/latest/Synthesis/189-fast-exponential-envelope-generator.html
// https://www.earlevel.com/main/2013/06/02/envelope-generators-adsr-part-2/
// https://svn.linuxsampler.org/cgi-bin/viewvc.cgi/linuxsampler/trunk/src/engines/common/EG.h?revision=2055&view=markup&pathrev=2218
// https://github.com/linuxsampler/linuxsampler


namespace utilities{

struct Attack {};
struct Decay {};
struct Sustain {};
struct Release {};
struct Idle {};

using State = std::variant<Idle, Attack, Decay, Sustain, Release>;

// Events
struct CounterExpiredEvt {};
struct NoteOnEvt {};
struct NoteOffEvt {};

class AdsrStm : public Fsm<AdsrStm, State> {
public:
  template <typename CurrentState, typename Event>
  auto processEvent(CurrentState &, const Event &e) {
    return std::nullopt;
  }
  // Idle state
  auto processEvent(Idle &, const NoteOnEvt &e) { return Attack(); }
  // Attack state
  auto processEvent(Attack &, const CounterExpiredEvt &e) { return Decay(); }
  auto processEvent(Attack &, const NoteOffEvt &e) { return Release(); }
  // Decay
  auto processEvent(Decay&, const CounterExpiredEvt &e) { return Sustain(); }
  // Sustain
  auto processEvent(Sustain &, const NoteOffEvt &e) { return Release(); }
  // Release
  auto processEvent(Release &, const CounterExpiredEvt &e) { return Idle(); }
};

class AdsrEnv {
public:
  struct Parameters {
    float attackTimeSec = 0.001f;
    float decayTimeSec = 0.001f;
    float sustainLevel = 0.6f;
    float releaseTimeSec = 0.001f;
  };

  AdsrEnv();
  void configure(const Parameters& param);
  void setSampleRate(float sampleRate);
  void setAttackTime(float attackTimeSec);
  void setReleaseTime(float releaseTimeSec);
  bool isActive() const;
  void trigger();
  void release();
  void reset();
  float tick();

private:
  void updateRates();
  void updateCoeff(float startLevel, float endLevel, uint32_t lengthInSamples);
  Parameters _parameters;
  float _attackRate = 0.0f;
  float _decayRate = 0.0f;
  float _releaseRate = 0.0f;
  float _envelopeValue = 0.0f;
  float _sampleRate = 0.0f;
  float _currentTargetLevel = 0.0f;
  float _multCoeff = 0.0f;
  int _counter = 0;
  AdsrStm _stm;
};

}

#endif
