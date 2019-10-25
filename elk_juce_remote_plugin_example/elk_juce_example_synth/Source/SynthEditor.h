#pragma once

#include "../../elk_juce_example_common/SynthGUI.h"
#include "SynthOSCReceiver.h"

/*
 * This class instantiates the OSC Receiver.
 * So, if the plugin runs hosted by Sushi, since this gui is there never instantiated,
 * instead the plugin relies on Sushi to receive OSC messages, and control the plugin parameters.
 *
 * But if the plugin is run as standalone, the GUI is instantiated, and, it will listen to OSC.
 */

class SynthEditor : public AudioProcessorEditor,
        private Value::Listener,
        private Label::Listener,
        private Timer
{
public:
    SynthEditor(AudioProcessor& processor)
        : AudioProcessorEditor(processor),
          _synthGUI(SynthGUI::FieldOptions::HideIPField)
    {
        addAndMakeVisible(_synthGUI);

        auto roomSizeValue = getParameterValue("roomSize");

        Slider& roomSizeSlider = _synthGUI.getRoomSizeSlider();

        roomSizeSlider.setValue(roomSizeValue, NotificationType::dontSendNotification);
        roomSizeSlider.onValueChange = [this]
        {
            setParameterValue("roomSize", static_cast<float>(_synthGUI.getRoomSizeSlider().getValue()));
        };
        roomSizeSlider.setRange(0.0, 1.0);

        Slider& dampingSlider = _synthGUI.getDampingSlider();

        auto dampingValue = getParameterValue("damping");
        dampingSlider.setValue(dampingValue, NotificationType::dontSendNotification);
        dampingSlider.onValueChange = [this]
        {
            setParameterValue("damping", (float) _synthGUI.getDampingSlider().getValue());
        };
        dampingSlider.setRange(0.0, 1.0);

        _synthGUI.getPortNumberField().addListener(this);

        _synthGUI.getSynthNameField().addListener(this);

        _OSC_receiver.getRoomSizeValue().addListener(this);
        _OSC_receiver.getDampingValue().addListener(this);

        setSize(700, 400);
        startTimer(100);
    }

    void paint(Graphics& g) override
    {
        g.fillAll(findColour(ResizableWindow::backgroundColourId));
    }

    void resized() override
    {
        _synthGUI.setBounds(getLocalBounds());
    }

private:
    void labelTextChanged(Label* labelThatHasChanged) override
    {
        if(labelThatHasChanged == &_synthGUI.getPortNumberField())
        {
            auto newPort = _synthGUI.getPortNumberField().getTextValue().toString().getIntValue();
            _OSC_receiver.changePort(newPort);
        }
        else if(labelThatHasChanged == &_synthGUI.getSynthNameField())
        {
            _OSC_receiver.updateSynthName(_synthGUI.getSynthNameField().getTextValue().toString());
        }
    }

    void valueChanged(Value& value) override
    {
        if(value.refersToSameSourceAs(_OSC_receiver.getRoomSizeValue()))
            _synthGUI.getRoomSizeSlider().setValue(static_cast<double>(value.getValue()), NotificationType::sendNotification);
        else if(value.refersToSameSourceAs(_OSC_receiver.getDampingValue()))
            _synthGUI.getDampingSlider().setValue(static_cast<double>(value.getValue()), NotificationType::sendNotification);
    }

    void timerCallback() override
    {
        _synthGUI.getRoomSizeSlider().setValue(getParameterValue ("roomSize"), NotificationType::dontSendNotification);
        _synthGUI.getDampingSlider().setValue(getParameterValue ("damping"), NotificationType::dontSendNotification);
    }

    AudioProcessorParameter* getParameter(const String& paramId)
    {
        if (auto* processor = getAudioProcessor())
        {
            auto& params = processor->getParameters();

            for (auto p : params)
            {
                if (auto* param = dynamic_cast<AudioProcessorParameterWithID*> (p))
                {
                    if (param->paramID == paramId)
                        return param;
                }
            }
        }

        return nullptr;
    }

    float getParameterValue(const String& paramId)
    {
        if (auto* param = getParameter (paramId))
            return param->getValue();

        return 0.0f;
    }

    void setParameterValue(const String& paramId, float value)
    {
        if (auto* param = getParameter (paramId))
            param->setValueNotifyingHost (value);
    }

    SynthGUI _synthGUI;
    SynthOSCReceiver _OSC_receiver;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthEditor)
};