#ifndef EUTERPESND_DSP_ADSRENV_H
#define EUTERPESND_DSP_ADSRENV_H

#include "AudioBuffer.h"
#include "AudioBufferTools.h"
#include "Conversions.h"
#include "Fsm.h"
#include <chrono>
#include <iostream>
#include <string>

using namespace std::chrono_literals;

constexpr double MINIMUM_LEVEL_DB = -80.0;
const double MINIMUM_LEVEL = utilities::fromDecibelsToGain(MINIMUM_LEVEL_DB);
const double ATTACK_TARGET_VALUE = 1.0;
const double ATTACK_OVERSHOOT_DB = -10.45;       // 0.3 in gain.
const double DECAY_RELEASE_OVERSHOOT_DB = -60.0; // 0.001 in gain.

namespace dsp {

struct Attack {
  std::string_view getName() const { return _name; }

private:
  static constexpr std::string_view _name = "attack";
};

struct Decay {

  std::string_view getName() const { return _name; }

private:
  static constexpr std::string_view _name = "decay";
};

struct Sustain {
  std::string_view getName() const { return _name; }

private:
  static constexpr std::string_view _name = "sustain";
};

struct Release {
  std::string_view getName() const { return _name; }

private:
  static constexpr std::string_view _name = "release";
};

struct Idle {
  std::string_view getName() const { return _name; }

private:
  static constexpr std::string_view _name = "idle";
};

using State = std::variant<Idle, Attack, Decay, Sustain, Release>;

// Events
// TODO : Switch counter expired for level reached?
struct TargetLevelReached {};
struct NoteOnEvt {};
struct NoteOffEvt {};

template <typename T> struct ExpSection {
  T advanceMultiplier;
  T baseValue;
  T overShoot;
};

class AdsrStm : public utilities::Fsm<AdsrStm, State> {
public:
  template <typename CurrentState, typename Event>
  auto processEvent(CurrentState &, const Event &e) {
    return std::nullopt;
  }

  template <typename CurrentState>
  auto processEvent(CurrentState &, const NoteOnEvt &e) {
    return Attack();
  }

  // Attack state
  auto processEvent(Attack &, const TargetLevelReached &e) { return Decay(); }

  auto processEvent(Attack &, const NoteOffEvt &e) { return Release(); }

  // Decay
  auto processEvent(Decay &, const TargetLevelReached &e) { return Sustain(); }

  auto processEvent(Decay &, const NoteOffEvt &e) { return Release(); }

  // Sustain
  auto processEvent(Sustain &, const NoteOffEvt &e) { return Release(); }

  // Release
  auto processEvent(Release &, const TargetLevelReached &e) { return Idle(); }
};

class AdsrEnv : public utilities::AudioProcessor<float> {
public:
  using SecondsDur = std::chrono::duration<double>;
  using Dur_ms = std::chrono::duration<double, std::milli>;
  struct Parameters {
    std::chrono::duration<double, std::milli> attackTime = 200ms;
    std::chrono::duration<double, std::milli> decayTime = 200ms;
    double sustainLevel = 1.0f;
    std::chrono::duration<double, std::milli> releaseTime = 200ms;
  };

  AdsrEnv() = default;

  void configure(const Parameters &param);

  void setSampleRate(float sampleRate);

  void setAttackTime(std::chrono::duration<double, std::milli> attackTime);

  void setReleaseTime(std::chrono::duration<double, std::milli> releaseTimeSec);

  void setDecayTime(std::chrono::duration<double, std::milli> decayTimeSec);

  template <typename StateType> bool isInState() const {
    return std::holds_alternative<StateType>(_stm.getState());
  }

  bool isActive() const;

  void trigger();

  void release();

  void reset();

  float tick();

  void process(utilities::AudioBuffer<float> &buffer) override;

private:
  void updateRates();

  double getRateMult(double startLevel, double endLevel,
                     SecondsDur interlvalTimeSec);

  Parameters _parameters;
  ExpSection<double> _attackSection;
  ExpSection<double> _decaySection;
  ExpSection<double> _releaseSection;
  double _envelopeValue = 0.0;
  double _sampleRate;
  AdsrStm _stm;
};
} // namespace dsp

#endif
