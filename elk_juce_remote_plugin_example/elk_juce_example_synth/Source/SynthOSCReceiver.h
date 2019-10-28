#include "../JuceLibraryCode/JuceHeader.h"

#pragma once

class SynthOSCReceiver :
        private OSCReceiver,
        private OSCReceiver::Listener <OSCReceiver::MessageLoopCallback>
{
public:
    SynthOSCReceiver()
    {
        changePort (defaultPort);

        addListener (this);
    }

    Value& getRoomSizeValue()
    {
        return roomSizeValue;
    }

    Value& getDampingValue()
    {
        return dampingValue;
    }

    void changePort (int port)
    {
        if (! connect (port))
        {
            DBG ("Connection Failed");
            showConnectionErrorMessage ("Error: could not connect to UDP port " + String (port) + ".");
        }
        else
        {
            DBG("Connection Succeeded");
        }
    }

    void updateSynthName (String name)
    {
        synthName = name;
        buildAddressPatterns();
    }

private:
    void buildAddressPatterns()
    {
        roomSizeAddressPattern = "/parameter/" + synthName + "/Room_Size";
        dampingAddressPattern = "/parameter/" + synthName + "/Damping";
    }

    void oscMessageReceived (const OSCMessage& message) override
    {
        DBG ("Message Received: ");

        if (message.size() == 1 && message[0].isFloat32())
        {
            DBG ("    value " + String (message[0].getFloat32()) + " to AP " + message.getAddressPattern().toString());

            if (message.getAddressPattern().matches(roomSizeAddressPattern))
            {
                roomSizeValue.setValue (jlimit (0.0f, 1.0f, message[0].getFloat32()));
            }
            else if (message.getAddressPattern().matches (dampingAddressPattern))
            {
                dampingValue.setValue (jlimit (0.0f, 1.0f, message[0].getFloat32()));
            }
        }
    }

    void showConnectionErrorMessage(const String& messageText)
    {
        AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
                                          "Connection error",
                                          messageText,
                                          "OK");
    }

    int defaultPort {24024};

    String synthName {"Elk_JUCE_Example_Synth"};
    String roomSizeAddressPattern {"/parameter/Elk_JUCE_Example_Synth/Room_Size"};
    String dampingAddressPattern {"/parameter/Elk_JUCE_Example_Synth/Damping"};

    Value roomSizeValue {0.5f};
    Value dampingValue {0.5f};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthOSCReceiver)
};
