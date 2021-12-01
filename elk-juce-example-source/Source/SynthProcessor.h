#pragma once

#include "SynthOSCReceiver.h"
#include "SynthEditor.h"
#include "cello.h"

using namespace dsp;

class SynthProcessor : public AudioProcessor
{
public:
    SynthProcessor()
        : AudioProcessor (BusesProperties().withOutput ("Output", AudioChannelSet::stereo(), true)),
          currentProgram (0)
    {
        // initialize parameters:
        addParameter (roomSizeParam = new AudioParameterFloat ("roomSize", "Room Size", 0.0f, 1.0f, 0.5f));
        addParameter (dampingParam = new AudioParameterFloat ("damping", "Damping", 0.0f, 1.0f, 0.5f));

        addParameter (cutoffParam = new AudioParameterFloat ("cutoff", "Cutoff", 0.0f, 1.0f, 0.5f));
        addParameter (resonanceParam = new AudioParameterFloat ("resonance", "Resonance", 0.0f, 1.0f, 0.5f));

        formatManager.registerBasicFormats();

        for (auto i = 0; i < maxNumVoices; ++i)
            synth.addVoice (new SamplerVoice());

        auto stream = new MemoryInputStream (cello::cello_wav, cello::cello_wavSize, false);
        loadNewSample (stream, "wav");
    }

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override
    {
        return (layouts.getMainOutputChannels() <= 2);
    }

    void prepareToPlay (double sampleRate, int estimatedMaxSizeOfBuffer) override
    {
        ignoreUnused (estimatedMaxSizeOfBuffer);

        lastSampleRate = sampleRate;

        synth.setCurrentPlaybackSampleRate (lastSampleRate);
        reverb.setSampleRate (lastSampleRate);

        filter.state = new StateVariableFilter::Parameters<float>;
        filter.prepare ({lastSampleRate, static_cast<uint32> (estimatedMaxSizeOfBuffer), 2 });
    }

    void processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override
    {
        juce::Reverb::Parameters reverbParameters;
        reverbParameters.roomSize = roomSizeParam->get();
        reverbParameters.damping = dampingParam->get();
        reverb.setParameters (reverbParameters);

        auto cutoff = static_cast<float> (cutoffParam->get());
        auto resonance = static_cast<float> (resonanceParam->get());
        auto type = static_cast<StateVariableFilter::Parameters<float>::Type> (0);

        cutoff = lerp (0.0f, 1.0f, 20.0f, 20000.0f, cutoff);
        resonance = lerp (0.0f, 1.0f, 0.3f, 20.0f, resonance);

        filter.state->type = type;
        filter.state->setCutOffFrequency (lastSampleRate, cutoff, resonance);

        synth.renderNextBlock (buffer, midiMessages, 0, buffer.getNumSamples());
		
        dsp::AudioBlock<float> block (buffer);
        filter.process (dsp::ProcessContextReplacing<float>(block));

        if (getMainBusNumOutputChannels() == 1)
        {
            reverb.processMono (buffer.getWritePointer (0), buffer.getNumSamples());
        }
        else if (getMainBusNumOutputChannels() == 2)
        {
            reverb.processStereo (buffer.getWritePointer (0), buffer.getWritePointer (1), buffer.getNumSamples());
        }
    }

    void releaseResources() override { }

    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    
    double getTailLengthSeconds() const override { return 0.0; }

    AudioProcessorEditor* createEditor() override { return new SynthEditor (*this); }
    bool hasEditor() const override { return true; }

    const String getName() const override { return "Elk Example JUCE Synth"; }
    int getNumPrograms() override { return 4; }
    int getCurrentProgram() override { return currentProgram; }
    void setCurrentProgram (int index) override { currentProgram = index; }

    const String getProgramName (int index) override
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

    void changeProgramName (int /*index*/, const String& /*name*/) override {}

    void getStateInformation (MemoryBlock& destData) override
    {
        MemoryOutputStream stream (destData, true);

        stream.writeFloat (*roomSizeParam);
        stream.writeFloat (*dampingParam);
        stream.writeFloat (*cutoffParam);
        stream.writeFloat (*resonanceParam);
    }

    void setStateInformation (const void* data, int sizeInBytes) override
    {
        MemoryInputStream stream (data, static_cast<size_t> (sizeInBytes), false);

        roomSizeParam->setValueNotifyingHost (stream.readFloat());
        dampingParam->setValueNotifyingHost (stream.readFloat());
        cutoffParam->setValueNotifyingHost (stream.readFloat());
        resonanceParam->setValueNotifyingHost (stream.readFloat());
    }

    bool supportsDoublePrecisionProcessing() const override
    {
        return false;
    }

private:
    void loadNewSample (InputStream* soundBuffer, const char* format)
    {
        std::unique_ptr <AudioFormatReader> formatReader (formatManager.findFormatForFileExtension (format)->createReaderFor (soundBuffer, true));

        BigInteger midiNotes;
        midiNotes.setRange (0, 126, true);
        SynthesiserSound::Ptr newSound = new SamplerSound ("Voice", *formatReader, midiNotes, 0x40, 0.1, 0.3, 1.0);

        synth.removeSound (0);
        sound = newSound;
        synth.addSound (sound);
    }

    float lerp (float x1, float x2, float y1, float y2, float xInt)
    {
        return y1 + (y2 - y1) / (x2 - x1) * (xInt - x1);
    }

    static constexpr int maxNumVoices = 20;

    AudioFormatManager formatManager;

    double lastSampleRate;

    juce::Reverb reverb;
    juce::Synthesiser synth;

    SynthesiserSound::Ptr sound;

    ProcessorDuplicator<StateVariableFilter::Filter<float>, StateVariableFilter::Parameters<float>> filter;

    AudioParameterFloat* roomSizeParam;
    AudioParameterFloat* dampingParam;

    AudioParameterFloat* cutoffParam;
    AudioParameterFloat* resonanceParam;

    int currentProgram;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthProcessor)
};