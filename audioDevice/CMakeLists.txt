find_package(RtAudio REQUIRED)

setup_library(NAME audioDevice DEPENDS utilities EXTERNAL_DEPENDS RtAudio::rtaudio UNIT_TEST)
