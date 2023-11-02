- Test the ADSR module sustain level works. Maybe add it as a percentage?
 
- Idea for testing the filters based on Chow implementation[StateVariableFilterTest.cpp](https://github.com/Chowdhury-DSP/chowdsp_utils/blob/master/tests/dsp_tests/chowdsp_filters_test/StateVariableFilterTest.cpp)

- Build a templated IIR filter class where the order is template parameter?
- How to use MIDI in RtAudio -> RtMidi
  - How to integrate the midi data into audioCallback?s
    - 
- Try to open stream in non-interleaved form to make the library juce-compatible.
  [Rt Audio Stream Options](https://ccrma.stanford.edu/software/stk/structRtAudio_1_1StreamOptions.html)
  It's possible (tested on testRtAudio) but I'm unsure of the benefit of it.

- Investigate how ProcessorChain and ProcessContext works in Juce.
  [juce::dsp::ProcessorChain](https://docs.juce.com/master/classdsp_1_1ProcessorChain.html)

- Investigate Juce's idea for AudioContext
  [juce::dsp::processors](https://docs.juce.com/master/group__juce__dsp-processors.html)

- Add support for auto clang-format from the github workflows.
  [auto-format.yml](https://github.com/jatinchowdhury18/plugin-ci-example/blob/main/.github/workflows/auto-format.yml)