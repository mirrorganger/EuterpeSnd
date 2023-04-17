#ifndef EUTERPESND_DSP_ADSRENV_H
#define EUTERPESND_DSP_ADSRENV_H

#include "Fsm.h"
#include "AudioBufferTools.h"
#include "AudioBuffer.h"
#include <string>
#include <iostream>
#include <chrono>
using namespace std::chrono_literals;

namespace dsp {

    struct Attack {
        std::string_view getName() const {
            return _name;
        }

    private:
        static constexpr std::string_view _name = "attack";
    };

    struct Decay {

        std::string_view getName() const {
            return _name;
        }

    private:
        static constexpr std::string_view _name = "decay";
    };

    struct Sustain {
        std::string_view getName() const {
            return _name;
        }

    private:
        static constexpr std::string_view _name = "sustain";
    };

    struct Release {
        std::string_view getName() const {
            return _name;
        }

    private:
        static constexpr std::string_view _name = "release";
    };

    struct Idle {
        std::string_view getName() const {
            return _name;
        }

    private:
        static constexpr std::string_view _name = "idle";
    };

    using State = std::variant<Idle, Attack, Decay, Sustain, Release>;

// Events
// TODO : Switch counter expired for level reached?
    struct TargetLevelReached {
    };
    struct NoteOnEvt {
    };
    struct NoteOffEvt {
    };

    template<typename T>
    struct ExpSection {
        T advanceMultiplier;
        uint64_t length = 0U;
    };

    class AdsrStm : public utilities::Fsm<AdsrStm, State> {
    public:
        template<typename CurrentState, typename Event>
        auto processEvent(CurrentState &, const Event &e) {
            return std::nullopt;
        }

        // Idle state
        auto processEvent(Idle &, const NoteOnEvt &e) { 
            return Attack(); }

        // Attack state
        auto processEvent(Attack &, const TargetLevelReached &e) {
             return Decay(); }

        auto processEvent(Attack &, const NoteOffEvt &e) { 
            return Release(); }

        // Decay
        auto processEvent(Decay &, const TargetLevelReached &e) {
             return Sustain(); }

        // Sustain
        auto processEvent(Sustain &, const NoteOffEvt &e) {
             return Release(); }

        // Release
        auto processEvent(Release &, const TargetLevelReached &e) {
             return Idle(); }
    };

    class AdsrEnv : public utilities::AudioProcessor<float> {
    public:
        using SecondsDur = std::chrono::duration<double>;
        struct Parameters {
            std::chrono::duration<double,std::milli>  attackTime = 200ms;
            std::chrono::duration<double,std::milli>  decayTime  = 200ms;
            double  sustainLevel = 1.0f;
            std::chrono::duration<double,std::milli>  releaseTime = 200ms;
        };

        AdsrEnv() = default;

        void configure(const Parameters &param);

        void setSampleRate(float sampleRate);

        void setAttackTime(std::chrono::duration<double, std::milli> attackTime);

        void setReleaseTime(std::chrono::duration<double, std::milli> releaseTimeSec);

        void setDecayTime(std::chrono::duration<double, std::milli> decayTimeSec);

        template<typename StateType>
        bool isInState() const{
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

        double getRateMult(double startLevel, double endLevel, SecondsDur interlvalTimeSec);

        Parameters _parameters;
        ExpSection<double> _attackSection;
        ExpSection<double> _decaySection;
        ExpSection<double> _releaseSection;
        double _envelopeValue = 0.0;
        double _sampleRate = 0.0;
        double _minimumLevel = 0.0001;
        uint64_t _currentSectionSample = 0U;
        AdsrStm _stm;
    };
}

#endif
