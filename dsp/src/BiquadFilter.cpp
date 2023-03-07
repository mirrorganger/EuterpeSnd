
#include "BiquadFilter.h"

#include <math.h>

namespace dsp {

/**
 * Computation based on the Transposed Canonical Biquad Form from
 * Designing Audio Effect Plugins in C++. For AAX, AU & VST3 & DSP Theory-Routledge (2019)
 * Chapter 10.22 Other Biquad Structures.
 */
    static inline NumericDataType
    processInternal(const float &inSample, double &z1, double &z2, const BiquadFilter::BiquadCoeff &biquadCoeff) {
        double out = biquadCoeff.b0 * inSample + z1;
        z1 = (inSample * biquadCoeff.b1) - (biquadCoeff.a1 * out) + z2;
        z2 = (inSample * biquadCoeff.b2) - (biquadCoeff.a2 * out);
        return static_cast<float>(out);
    }

    BiquadFilter::BiquadFilter() {
    }

    BiquadFilter::BiquadFilter(const FilterSettings &filterSettings) {
        setUp(filterSettings);
    }

    void BiquadFilter::setUp(const FilterSettings &filterSettings) {
        _filterSettings = filterSettings;
        _z1.resize(filterSettings.nChannels);
        _z2.resize(filterSettings.nChannels);
        clear();
        update();
    }

    void BiquadFilter::setQfactor(double qFactor) {
        _filterSettings.qFactor = qFactor;
        update();
    }

    void BiquadFilter::setType(Type filterType) {
        _filterSettings.filterType = filterType;
        update();
    }

    void BiquadFilter::setCentralFreq(double cutoffFreq) {
        _filterSettings.cutoffFreq = cutoffFreq;
        update();
    }

    void BiquadFilter::clear() {
        for (auto v: {_z1, _z2}) {
            std::fill(v.begin(), v.end(), 0.0);
        }
    }


    void BiquadFilter::process(utilities::AudioBuffer<float> &buffer) {

        for (size_t sample = 0; sample < buffer.getFramesPerBuffer(); sample++) {
            size_t sampleOffset = sample * buffer.getNumberOfChannels();
            for (size_t channel = 0; channel < buffer.getNumberOfChannels(); channel++) {
                float cached = buffer[sampleOffset + channel];
                buffer[sampleOffset + channel] = processInternal(cached, _z1[channel], _z2[channel], _biquadCoeff);
            }
        }

    }

    void BiquadFilter::update() {

        switch (_filterSettings.filterType) {
            case Type::LOWPASS: {
                auto k = 1.0 / std::tan(
                        static_cast<double>(M_PI) * _filterSettings.cutoffFreq / _filterSettings.samplingFreq);
                auto kSquared = k * k;
                auto qInv = 1.0 / _filterSettings.qFactor;
                auto norm = 1.0 / (1.0 + k * qInv + kSquared);
                _biquadCoeff.b0 = norm;
                _biquadCoeff.b1 = 2.0 * norm;
                _biquadCoeff.b2 = norm;
                _biquadCoeff.a1 = norm * 2.0 * (1.0 - kSquared);
                _biquadCoeff.a2 = norm * (1.0 - k * qInv + kSquared);
                break;
            }
            case Type::HIGHPASS: {
                auto k = std::tan(
                        static_cast<double>(M_PI) * _filterSettings.cutoffFreq / _filterSettings.samplingFreq);
                auto kSquared = k * k;
                auto qInv = 1.0 / _filterSettings.qFactor;
                auto norm = 1.0 / (1.0 + k * qInv + kSquared);
                _biquadCoeff.b0 = norm;
                _biquadCoeff.b1 = norm * -2.0;
                _biquadCoeff.b2 = norm;
                _biquadCoeff.a1 = norm * 2.0 * (kSquared - 1.0);
                _biquadCoeff.a2 = norm * (1.0 - k * qInv + kSquared);
                break;
            }
            case Type::BANDPASS:
                /* code */
                break;
            default:
                break;
        }
    }

} // namespace AudioUtilities
