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

        auto roomSizeValue = getParameterValue (roomSizeName);
        Slider& roomSizeSlider = synthGUI.getRoomSizeSlider();
        roomSizeSlider.setValue (roomSizeValue, NotificationType::dontSendNotification);

        roomSizeSlider.onValueChange = [this]
        {
            const float sliderValue = static_cast <float> (synthGUI.getRoomSizeSlider().getValue());
            const float roomSizeValue = getParameterValue (roomSizeName);

            if (! approximatelyEqual (roomSizeValue, sliderValue))
            {
                setParameterValue (roomSizeName, sliderValue);

                // create and send an OSC message with an address and a float value:
                float value = static_cast <float> (synthGUI.getRoomSizeSlider().getValue());

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

        auto dampingValue = getParameterValue (dampingName);
        Slider& dampingSlider = synthGUI.getDampingSlider();
        dampingSlider.setValue (dampingValue, NotificationType::dontSendNotification);

        dampingSlider.onValueChange = [this]
        {
            const float sliderValue = static_cast <float> (synthGUI.getDampingSlider().getValue());
            const float dampingValue = getParameterValue (dampingName);

            if (! approximatelyEqual (dampingValue, sliderValue))
            {
                setParameterValue (dampingName, (float) synthGUI.getDampingSlider().getValue());

                // create and send an OSC message with an address and a float value:
                float value = static_cast <float> (synthGUI.getDampingSlider().getValue());

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

        synthGUI.getInPortNumberField().addListener (this);
        synthGUI.getSynthNameField().addListener (this);
        synthGUI.getOutPortNumberField().addListener (this);
        synthGUI.getIPField().addListener (this);

        oscRreceiver.getRoomSizeValue().addListener (this);
        oscRreceiver.getDampingValue().addListener (this);

        updateInConnectedLabel();

        connectSender();

        setSize (700, 400);
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
            if (! approximatelyEqual (static_cast <double> (value.getValue()), synthGUI.getRoomSizeSlider().getValue()))
            {
                synthGUI.getRoomSizeSlider().setValue(static_cast <double> (value.getValue()),
                                                      NotificationType::sendNotification);
            }
        }
        else if (value.refersToSameSourceAs (oscRreceiver.getDampingValue()))
        {
            if (! approximatelyEqual (static_cast <double> (value.getValue()), synthGUI.getDampingSlider().getValue()))
            {
                synthGUI.getDampingSlider().setValue (static_cast <double> (value.getValue()),
                                                      NotificationType::sendNotification);
            }
        }
    }

    void timerCallback() override
    {
        synthGUI.getRoomSizeSlider().setValue (getParameterValue (roomSizeName), NotificationType::dontSendNotification);
        synthGUI.getDampingSlider().setValue (getParameterValue (dampingName), NotificationType::dontSendNotification);
    }

    AudioProcessorParameter* getParameter (const String& paramId)
    {
        if (auto* processor = getAudioProcessor())
        {
            auto& params = processor->getParameters();

            for (auto p : params)
            {
                if (auto* param = dynamic_cast <AudioProcessorParameterWithID*> (p))
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
    int incomingPort {24025};

    String synthName {"Elk_JUCE_Example_Synth"};
    String roomSizeAddressPattern {"/parameter/Elk_JUCE_Example_Synth/Room_Size"};
    String dampingAddressPattern {"/parameter/Elk_JUCE_Example_Synth/Damping"};

    const String dampingName {"damping"};
    const String roomSizeName {"roomSize"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthEditor)
};