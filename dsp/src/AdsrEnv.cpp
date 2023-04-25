//
// Created by cesar on 8/13/22.
//

#include "AdsrEnv.h"
#include <cmath>
#include <cstring>

namespace dsp {

    template<typename... Ts> // (7)
    struct Overloaded : Ts ... {
        using Ts::operator()...;
    };
    template<class... Ts> Overloaded(Ts...) -> Overloaded<Ts...>;

    void AdsrEnv::configure(const AdsrEnv::Parameters &param) {
        _parameters = param;
        _attackSection.overShoot = utilities::fromDecibelsToGain(ATTACK_OVERSHOOT_DB);
        _releaseSection.overShoot = utilities::fromDecibelsToGain(DECAY_RELEASE_OVERSHOOT_DB);
        _decaySection.overShoot = utilities::fromDecibelsToGain(DECAY_RELEASE_OVERSHOOT_DB);
        updateRates();
    }

    void AdsrEnv::setSampleRate(float sampleRate) {
        _sampleRate = sampleRate;
        updateRates();
    }

    void AdsrEnv::setAttackTime(std::chrono::duration<double, std::milli> attackTime) {
        _parameters.attackTime = attackTime;
        updateRates();
    }

    void AdsrEnv::setReleaseTime(std::chrono::duration<double, std::milli> releaseTime) {
        _parameters.releaseTime = releaseTime;
        updateRates();
    }

    void AdsrEnv::setDecayTime(std::chrono::duration<double, std::milli> decayTime) {
        _parameters.decayTime = decayTime;
        updateRates();
    }

    bool AdsrEnv::isActive() const {
        return !std::holds_alternative<Idle>(_stm.getState());
    }

    void AdsrEnv::trigger() {
        if (_envelopeValue < MINIMUM_LEVEL) _envelopeValue = MINIMUM_LEVEL;
        _stm.transition(NoteOnEvt());
    }

    void AdsrEnv::release() {
        _stm.transition(NoteOffEvt{});
    }

    void AdsrEnv::reset() {
        _envelopeValue = MINIMUM_LEVEL;
    }

/**
 * TODO: Think about when to set it to the target values in each step.
 * A_db = 20 log_10 (A_liner)
 *
 * The time of attack left (in samples) could be computed from the current value
 * and the multiplier.
 * @return
 */
    float AdsrEnv::tick() {
        auto visitor = Overloaded{[&](Idle) { _envelopeValue = 0.0f; },
                                  [&](Attack) {
                                      _envelopeValue = _attackSection.baseValue +
                                              _attackSection.advanceMultiplier * _envelopeValue;
                                      if (_envelopeValue >= (1.0)) {
                                          _envelopeValue = 1.0;
                                          _stm.transition(TargetLevelReached{});
                                      }
                                  },
                                  [&](Decay) {
                                      _envelopeValue = _decaySection.baseValue +
                                                        _decaySection.advanceMultiplier * _envelopeValue;
                                      if (_envelopeValue <= (_parameters.sustainLevel))
                                      {
                                          _envelopeValue = _parameters.sustainLevel;
                                          _stm.transition(TargetLevelReached{});
                                      }
                                  },
                                  [&](Sustain) {},
                                  [&](Release) {
                                      _envelopeValue = _releaseSection.baseValue +
                                                        _releaseSection.advanceMultiplier * _envelopeValue;
                                      if (_envelopeValue <= MINIMUM_LEVEL) {
                                          _stm.transition(TargetLevelReached{});
                                      }
                                  }};
        std::visit(visitor, _stm.getState());
        return static_cast<float>(_envelopeValue);
    }

    void AdsrEnv::process(utilities::AudioBuffer<float> &buffer) {
        auto multVisitor = Overloaded{[&](Idle) {
            utilities::AudioBufferTools::clearRawBuffer(buffer.getWritePointer(),
                                                        buffer.getNumberOfChannels() * buffer.getFramesPerBuffer());
        },
                                      [&](Sustain) {
                                          utilities::AudioBufferTools::multiplyRawBuffer(buffer.getWritePointer(),
                                                                                         buffer.getNumberOfChannels() *
                                                                                         buffer.getFramesPerBuffer(),
                                                                                         _envelopeValue);
                                      },
                                      [&](auto otherState) {
                                          auto wrtPtr = buffer.getWritePointer();
                                          for (int sample_i = 0; sample_i < buffer.getFramesPerBuffer(); ++sample_i) {
                                              auto multCached = tick();
                                              for (int channel_i = 0;
                                                   channel_i < buffer.getNumberOfChannels(); ++channel_i) {
                                                  (*wrtPtr) *= multCached;
                                                  wrtPtr++;
                                              }
                                          }
                                      }};

        std::visit(multVisitor, _stm.getState());
    }


    void AdsrEnv::updateRates() {
        _attackSection.advanceMultiplier = getRateMult(_attackSection.overShoot, 1.0 + _attackSection.overShoot,
                                                       _parameters.attackTime);
        _attackSection.baseValue = (1.0 + _attackSection.overShoot) * (1.0 - _attackSection.advanceMultiplier);
        _decaySection.advanceMultiplier = getRateMult(_decaySection.overShoot, 1.0 + _decaySection.overShoot,
                                                      _parameters.decayTime);
        _decaySection.baseValue = (_parameters.sustainLevel-_decaySection.overShoot) * (1.0 - _decaySection.advanceMultiplier);
        _releaseSection.advanceMultiplier = getRateMult(_releaseSection.overShoot, 1.0 + _releaseSection.overShoot,
                                                      _parameters.releaseTime);
        _releaseSection.baseValue = (-_releaseSection.overShoot) * (1.0 - _releaseSection.advanceMultiplier);
    }

    /*
    double AdsrEnv::getRateMult(double startLevel, double endLevel, SecondsDur intervalTime) {
        return 1.0 + (log(endLevel) - log(startLevel)) / (intervalTime.count() * _sampleRate);
    }
    */

    double AdsrEnv::getRateMult(double startLevel, double endLevel, dsp::AdsrEnv::SecondsDur intervalTime) {
        return exp(-log(endLevel / startLevel) / (intervalTime.count() * _sampleRate));
    }

}
