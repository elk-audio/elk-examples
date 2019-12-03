#pragma once

#include "SynthGUI.h"
#include "SynthOSCReceiver.h"

/*
 * This class instantiates the OSC Receiver and OSC Sender.
 * So, if the plugin runs hosted by Sushi, since this gui is there never instantiated,
 * instead the plugin relies on Sushi to receive OSC messages, and control the plugin parameters.
 *
 * But if the plugin is run as standalone, or in a desktop VST host,
 * the GUI can be instantiated, and, it will send and receive OSC.
 */

class SynthEditor : public AudioProcessorEditor,
        private Value::Listener,
        private Label::Listener,
        private Timer
{
public:
    SynthEditor (AudioProcessor& processor)
        : AudioProcessorEditor (processor),
          synthGUI()
    {
        addAndMakeVisible (synthGUI);

        // Room Size Slider:
        auto roomSizeValue = getParameterValue (roomSizeName);
        Slider& roomSizeSlider = synthGUI.getRoomSizeSlider();
        roomSizeSlider.setValue (roomSizeValue, NotificationType::dontSendNotification);

        roomSizeSlider.onValueChange = [this]
        {
            const float sliderValue = static_cast<float> (synthGUI.getRoomSizeSlider().getValue());
            const float roomSizeValue = getParameterValue (roomSizeName);

            if (! approximatelyEqual (roomSizeValue, sliderValue))
            {
                setParameterValue (roomSizeName, sliderValue);

                // create and send an OSC message with an address and a float value:
                float value = static_cast<float> (synthGUI.getRoomSizeSlider().getValue());

                if (! oscSender.send (roomSizeAddressPattern, value))
                {
                    updateOutConnectedLabel (false);
                }
                else
                {
                    DBG ("Sent value " + String (value) + " to AP " + roomSizeAddressPattern);
                }
            }
        };

        roomSizeSlider.setRange (0.0, 1.0);

        // Damping Slider:
        auto dampingValue = getParameterValue (dampingName);
        Slider& dampingSlider = synthGUI.getDampingSlider();
        dampingSlider.setValue (dampingValue, NotificationType::dontSendNotification);

        dampingSlider.onValueChange = [this]
        {
            const float sliderValue = static_cast<float> (synthGUI.getDampingSlider().getValue());
            const float dampingValue = getParameterValue (dampingName);

            if (! approximatelyEqual (dampingValue, sliderValue))
            {
                setParameterValue (dampingName, static_cast<float> (synthGUI.getDampingSlider().getValue()));

                // create and send an OSC message with an address and a float value:
                float value = static_cast<float> (synthGUI.getDampingSlider().getValue());

                if (! oscSender.send (dampingAddressPattern, value))
                {
                    updateOutConnectedLabel (false);
                }
                else
                {
                    DBG ("Sent value " + String (value) + " to AP " + dampingAddressPattern);
                }
            }
        };

        dampingSlider.setRange (0.0, 1.0);

        // Cutoff Slider:
        auto cutoffValue = getParameterValue (cutoffName);
        Slider& cutoffSlider = synthGUI.getCutoffSlider();
        cutoffSlider.setValue (cutoffValue, NotificationType::dontSendNotification);

		cutoffSlider.setSkewFactor(0.5);

        cutoffSlider.onValueChange = [this]
        {
            const float sliderValue = static_cast<float> (synthGUI.getCutoffSlider().getValue());
            const float cutoffValue = getParameterValue (cutoffName);

            if (! approximatelyEqual (cutoffValue, sliderValue))
            {
                setParameterValue (cutoffName, static_cast<float> (synthGUI.getCutoffSlider().getValue()));

                // create and send an OSC message with an address and a float value:
                float value = static_cast<float> (synthGUI.getCutoffSlider().getValue());

                if (! oscSender.send (cutoffAddressPattern, value))
                {
                    updateOutConnectedLabel (false);
                }
                else
                {
                    DBG ("Sent value " + String (value) + " to AP " + cutoffAddressPattern);
                }
            }
        };

        cutoffSlider.setRange (0.0, 1.0);

        // Resonance Slider:
        auto resonanceValue = getParameterValue (resonanceName);
        Slider& resonanceSlider = synthGUI.getResonanceSlider();
        resonanceSlider.setValue (resonanceValue, NotificationType::dontSendNotification);

        resonanceSlider.onValueChange = [this]
        {
            const float sliderValue = static_cast<float> (synthGUI.getResonanceSlider().getValue());
            const float resonanceValue = getParameterValue (resonanceName);

            if (! approximatelyEqual (resonanceValue, sliderValue))
            {
                setParameterValue (resonanceName, static_cast<float> (synthGUI.getResonanceSlider().getValue()));

                // create and send an OSC message with an address and a float value:
                float value = static_cast<float> (synthGUI.getResonanceSlider().getValue());

                if (! oscSender.send (resonanceAddressPattern, value))
                {
                    updateOutConnectedLabel (false);
                }
                else
                {
                    DBG ("Sent value " + String (value) + " to AP " + resonanceAddressPattern);
                }
            }
        };

        resonanceSlider.setRange (0.0, 1.0);

        synthGUI.getInPortNumberField().addListener (this);
        synthGUI.getSynthNameField().addListener (this);
        synthGUI.getOutPortNumberField().addListener (this);
        synthGUI.getIPField().addListener (this);

        oscRreceiver.getRoomSizeValue().addListener (this);
        oscRreceiver.getDampingValue().addListener (this);

        oscRreceiver.getCutoffValue().addListener (this);
        oscRreceiver.getResonanceValue().addListener (this);

        updateInConnectedLabel();

        connectSender();

        setSize (700, 600);
        startTimer (100);
    }

    void paint (Graphics& g) override
    {
        g.fillAll (findColour (ResizableWindow::backgroundColourId));
    }

    void resized() override
    {
        synthGUI.setBounds (getLocalBounds());
    }

private:
    void buildAddressPatterns()
    {
        roomSizeAddressPattern = "/parameter/" + synthName + "/Room_Size";
        dampingAddressPattern = "/parameter/" + synthName + "/Damping";
        cutoffAddressPattern = "/parameter/" + synthName + "/Cutoff";
        resonanceAddressPattern = "/parameter/" + synthName + "/Resonance";
    }

    void connectSender()
    {
        // specify here where to send OSC messages to: host URL and UDP port number
        if (! oscSender.connect (outgoingIP, outgoingPort))
        {
            updateOutConnectedLabel (false);
        }
        else
        {
            updateOutConnectedLabel (true);
        }
    }

    void updateOutgoingIP (String ip)
    {
        outgoingIP = ip;
        connectSender();
    }

    void updateOutgoingPort (int port)
    {
        outgoingPort = port;
        connectSender();
    }

    void labelTextChanged (Label* labelThatHasChanged) override
    {
        if (labelThatHasChanged == &synthGUI.getInPortNumberField())
        {
            const int newPort = synthGUI.getInPortNumberField().getTextValue().toString().getIntValue();
            oscRreceiver.changePort (newPort);
            updateInConnectedLabel();
        }
        else if (labelThatHasChanged == &synthGUI.getOutPortNumberField())
        {
            const int newPort = synthGUI.getOutPortNumberField().getTextValue().toString().getIntValue();
            updateOutgoingPort (newPort);
        }
        else if (labelThatHasChanged == &synthGUI.getIPField())
        {
            const String newIP = synthGUI.getIPField().getTextValue().toString();
            updateOutgoingIP (newIP);
        }
        else if (labelThatHasChanged == &synthGUI.getSynthNameField())
        {
            synthName = synthGUI.getSynthNameField().getTextValue().toString();
            buildAddressPatterns();
            oscRreceiver.updateSynthName (synthGUI.getSynthNameField().getTextValue().toString());
        }
    }

    void updateInConnectedLabel()
    {
        const bool connected = oscRreceiver.isConnected();
        if (connected)
        {
            synthGUI.getInConnectedLabel().setText ("(Connected)", dontSendNotification);
        }
        else
        {
            synthGUI.getInConnectedLabel().setText ("(Disconnected!)", dontSendNotification);
        }
    }

    void updateOutConnectedLabel (bool connected)
    {
        if (connected)
        {
            synthGUI.getOutConnectedLabel().setText ("(Connected)", dontSendNotification);
        }
        else
        {
            synthGUI.getOutConnectedLabel().setText ("(Disconnected!)", dontSendNotification);
        }
    }

    // This callback is invoked if an OSC message has been received setting either value.
    void valueChanged (Value& value) override
    {
        if (value.refersToSameSourceAs (oscRreceiver.getRoomSizeValue()))
        {
            if (! approximatelyEqual (static_cast<double> (value.getValue()), synthGUI.getRoomSizeSlider().getValue()))
            {
                synthGUI.getRoomSizeSlider().setValue(static_cast<double> (value.getValue()),
                                                      NotificationType::sendNotification);
            }
        }
        else if (value.refersToSameSourceAs (oscRreceiver.getDampingValue()))
        {
            if (! approximatelyEqual (static_cast<double> (value.getValue()), synthGUI.getDampingSlider().getValue()))
            {
                synthGUI.getDampingSlider().setValue (static_cast<double> (value.getValue()),
                                                      NotificationType::sendNotification);
            }
        }
        else if (value.refersToSameSourceAs (oscRreceiver.getCutoffValue()))
        {
            if (! approximatelyEqual (static_cast<double> (value.getValue()), synthGUI.getCutoffSlider().getValue()))
            {
                synthGUI.getCutoffSlider().setValue (static_cast<double> (value.getValue()),
                                                      NotificationType::sendNotification);
            }
        }
        else if (value.refersToSameSourceAs (oscRreceiver.getResonanceValue()))
        {
            if (! approximatelyEqual (static_cast<double> (value.getValue()), synthGUI.getResonanceSlider().getValue()))
            {
                synthGUI.getResonanceSlider().setValue (static_cast<double> (value.getValue()),
                                                     NotificationType::sendNotification);
            }
        }
    }

    void timerCallback() override
    {
        synthGUI.getRoomSizeSlider().setValue (getParameterValue (roomSizeName), NotificationType::dontSendNotification);
        synthGUI.getDampingSlider().setValue (getParameterValue (dampingName), NotificationType::dontSendNotification);
        synthGUI.getCutoffSlider().setValue (getParameterValue (cutoffName), NotificationType::dontSendNotification);
        synthGUI.getResonanceSlider().setValue (getParameterValue (resonanceName), NotificationType::dontSendNotification);
    }

    AudioProcessorParameter* getParameter (const String& paramId)
    {
        if (auto* proc = getAudioProcessor())
        {
            auto& params = proc->getParameters();

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

    float getParameterValue (const String& paramId)
    {
        if (auto* param = getParameter (paramId))
            return param->getValue();

        return 0.0f;
    }

    void setParameterValue (const String& paramId, float value)
    {
        if (auto* param = getParameter (paramId))
            param->setValueNotifyingHost (value);
    }

    SynthGUI synthGUI;
    SynthOSCReceiver oscRreceiver;
    OSCSender oscSender;

    String outgoingIP {"127.0.0.1"};
    int outgoingPort {24024};
    int incomingPort {25024};

    String synthName {"elk_juce_example"};
    String roomSizeAddressPattern {"/parameter/elk_juce_example/Room_Size"};
    String dampingAddressPattern {"/parameter/elk_juce_example/Damping"};

    String cutoffAddressPattern {"/parameter/elk_juce_example/Cutoff"};
    String resonanceAddressPattern {"/parameter/elk_juce_example/Resonance"};

    const String dampingName {"damping"};
    const String roomSizeName {"roomSize"};

    const String cutoffName {"cutoff"};
    const String resonanceName {"resonance"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthEditor)
};