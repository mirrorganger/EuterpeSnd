#include <cstdint>

namespace dsp {

class TrapezoidalEnv {

public:
  void init(const uint32_t fadeLength, bool autoRetrigger = false);
  void attack();
  void release();
  bool isActive();
  float advance();

private:
  enum class EnvState : uint8_t {
    IDLE = 0U,
    FADE_IN = 1U,
    SUSTAIN = 2U,
    FADE_OUT = 3U,
  };

  inline float getFromCounter() {
    return static_cast<float>(_counter) / static_cast<float>(_fadeLength);
  }

  uint32_t _counter = 0U;
  uint32_t _fadeLength = 0U;
  EnvState _state = EnvState::IDLE;
  bool _autoRetriggered = false;
};

} // namespace dsp