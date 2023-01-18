#ifndef EUTERPESND_DSP_ADSRENV_H
#define EUTERPESND_DSP_ADSRENV_H

#include "Fsm.h"
#include "AudioBufferTools.h"
#include "AudioBuffer.h"
#include <string>

namespace dsp{

struct Attack {
    std::string_view getName()const{
      return _name;
    }
  private:
    static constexpr std::string_view _name = "attack";
};
struct Decay {

  std::string_view getName()const{
      return _name;
    }
  private:
    static constexpr std::string_view _name = "attack";
};
struct Sustain {
    std::string_view getName()const{
      return _name;
    }
  private:
    static constexpr std::string_view _name = "sustain";
};
struct Release {
      std::string_view getName()const{
      return _name;
    }
  private:
    static constexpr std::string_view _name = "release";
};
struct Idle {
      std::string_view getName()const{
      return _name;
    }
  private:
    static constexpr std::string_view _name = "idle";
};

using State = std::variant<Idle, Attack, Decay, Sustain, Release>;

// Events
// TODO : Switch counter expired for level reached?
struct TargetLevelReached {};
struct NoteOnEvt {};
struct NoteOffEvt {};

template<typename T>
struct ExpSection{
    T overshoot;
    T offset;
    T multiplier;
};

class AdsrStm : public utilities::Fsm<AdsrStm, State> {
public:
  template <typename CurrentState, typename Event>
  auto processEvent(CurrentState &, const Event &e) {
    return std::nullopt;
  }
  // Idle state
  auto processEvent(Idle &, const NoteOnEvt &e) { return Attack(); }
  // Attack state
  auto processEvent(Attack &, const TargetLevelReached &e) { return Decay(); }
  auto processEvent(Attack &, const NoteOffEvt &e) { return Release(); }
  // Decay
  auto processEvent(Decay&, const TargetLevelReached &e) { return Sustain(); }
  // Sustain
  auto processEvent(Sustain &, const NoteOffEvt &e) { return Release(); }
  // Release
  auto processEvent(Release &, const TargetLevelReached &e) { return Idle(); }
};

class AdsrEnv : public utilities::AudioProcessor<float> {
public:
  struct Parameters {
    float attackTimeSec = 0.2f;
    float decayTimeSec = 0.2f;
    float sustainLevel = 0.2f;
    float releaseTimeSec = 0.2f;
  };

  AdsrEnv() = default;
  void configure(const Parameters& param);
  void setSampleRate(float sampleRate);
  void setAttackTime(float attackTimeSec);
  void setReleaseTime(float releaseTimeSec);
  void setDecayTime(float decayTimeSec);
  bool isActive() const;
  void trigger();
  void release();
  void reset();
  float tick();
  void applyToBuffer(float* outBuffer, int numSamples);
  void process(utilities::AudioBuffer<float>& buffer) override;
private:
  void updateRates();
  float getRateMult(float startLevel, float endLevel, uint32_t lengthInSamples);
  Parameters _parameters;
  ExpSection<float> _attackSection = {0.0,0.3,0.0};
  ExpSection<float> _decaySection = {0.0,0.3,0.0};;
  ExpSection<float> _releaseSection =  {0.0,0.3,0.0};;
  float _envelopeValue = 0.0f;
  float _sampleRate = 0.0f;
  AdsrStm _stm;
};

}

#endif
