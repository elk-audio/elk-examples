#pragma once

#include "../../elk_juce_example_common/DemoUtilities.h"
#include "SynthOSCReceiver.h"
#include "SynthEditor.h"
#include "sample.h"

class SynthProcessor : public AudioProcessor
{
public:
    SynthProcessor()
        : AudioProcessor(BusesProperties().withOutput ("Output", AudioChannelSet::stereo(), true)),
          _currentProgram (0)
    {
        // initialize parameters:
        addParameter(_roomSizeParam = new AudioParameterFloat ("roomSize", "Room Size", 0.0f, 1.0f, 0.5f));
        addParameter(_dampingParam = new AudioParameterFloat ("damping", "Damping", 0.0f, 1.0f, 0.5f));

        _formatManager.registerBasicFormats();

        for (auto i = 0; i < _maxNumVoices; ++i)
            _synth.addVoice (new SamplerVoice());

        auto stream = new MemoryInputStream(sample::sample_ogg, sample::sample_oggSize, false);
        loadNewSample (stream, "ogg");
    }

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override
    {
        return (layouts.getMainOutputChannels() <= 2);
    }

    void prepareToPlay(double sampleRate, int estimatedMaxSizeOfBuffer) override
    {
        ignoreUnused(estimatedMaxSizeOfBuffer);

        _lastSampleRate = sampleRate;

        _synth.setCurrentPlaybackSampleRate (_lastSampleRate);
        _reverb.setSampleRate (_lastSampleRate);
    }

    void processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override
    {
        Reverb::Parameters reverbParameters;
        reverbParameters.roomSize = _roomSizeParam->get();
        reverbParameters.damping = _dampingParam->get();

        _reverb.setParameters(reverbParameters);
        _synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

        if (getMainBusNumOutputChannels() == 1)
            _reverb.processMono(buffer.getWritePointer (0), buffer.getNumSamples());
        else if (getMainBusNumOutputChannels() == 2)
            _reverb.processStereo(buffer.getWritePointer (0), buffer.getWritePointer (1), buffer.getNumSamples());
    }

    void releaseResources() override { }

    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool silenceInProducesSilenceOut() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    AudioProcessorEditor* createEditor() override { return new SynthEditor (*this); }
    bool hasEditor() const override { return true; }

    const String getName() const override { return "Elk Example JUCE Synth"; }
    int getNumPrograms() override { return 4; }
    int getCurrentProgram() override { return _currentProgram; }
    void setCurrentProgram (int index) override { _currentProgram = index; }

    const String getProgramName(int index) override
    {
        switch (index)
        {
            case 0:  return "Piano";
            case 1:  return "Singing";
            case 2:  return "Pinched Balloon";
            case 3:  return "Gazeebo";
        }

        return "<Unknown>";
    }

    void changeProgramName(int /*index*/, const String& /*name*/) override {}

    void getStateInformation(MemoryBlock& destData) override
    {
        MemoryOutputStream stream(destData, true);

        stream.writeFloat(*_roomSizeParam);
        stream.writeFloat(*_dampingParam);
    }

    void setStateInformation(const void* data, int sizeInBytes) override
    {
        MemoryInputStream stream(data, static_cast<size_t> (sizeInBytes), false);

        _roomSizeParam->setValueNotifyingHost(stream.readFloat());
        _dampingParam->setValueNotifyingHost(stream.readFloat());
    }

private:
    void loadNewSample(InputStream* soundBuffer, const char* format)
    {
        std::unique_ptr<AudioFormatReader> formatReader(_formatManager.findFormatForFileExtension(format)->createReaderFor(soundBuffer, true));

        BigInteger midiNotes;
        midiNotes.setRange(0, 126, true);
        SynthesiserSound::Ptr newSound = new SamplerSound("Voice", *formatReader, midiNotes, 0x40, 0.0, 0.0, 10.0);
        _synth.removeSound(0);
        _sound = newSound;
        _synth.addSound(_sound);
    }

    static constexpr int _maxNumVoices = 5;

    AudioFormatManager _formatManager;

    double _lastSampleRate;

    Reverb _reverb;
    Synthesiser _synth;
    SynthesiserSound::Ptr _sound;

    AudioParameterFloat* _roomSizeParam;
    AudioParameterFloat* _dampingParam;

    int _currentProgram;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthProcessor)
};