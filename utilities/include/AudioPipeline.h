#include <tuple>
#include <vector>
#include <concepts>

#include "AudioBuffer.h"

namespace utilities
{

template<typename T, typename NumericType>
concept isAudioProcessor = requires(T t, AudioBuffer<NumericType>& buffer){
  {t.process(buffer)} ;
};

template<typename NumericType, isAudioProcessor<NumericType>... Processors>
class AudioPipeline
{
public:
    using Buffer = AudioBuffer<NumericType>;
    void processAll(Buffer& buffer){
      std::apply([&](auto&&... args){(args.process(buffer), ...);},_processors);
    }

    template<int Index>
    void processOne(Buffer& buffer){
       std::get<Index>(_processors).process(buffer);
    }

    template<typename ProcessorType>
    void processOne(Buffer& buffer){
      std::get<ProcessorType>(_processors).process(buffer);
    }

    template<int Index>
    auto& getProcessor(){return std::get<Index>(_processors);}

    template<typename ProcessorType>
    ProcessorType& getProcessor(){return std::get<ProcessorType>(_processors);}

private:
  std::tuple<Processors...> _processors;
};





}

