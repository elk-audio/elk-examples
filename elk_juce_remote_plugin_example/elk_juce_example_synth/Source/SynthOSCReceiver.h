#include "../JuceLibraryCode/JuceHeader.h"

#pragma once

class SynthOSCReceiver :
        private OSCReceiver,
        private OSCReceiver::Listener<OSCReceiver::MessageLoopCallback>
{
public:
    SynthOSCReceiver()
    {
        changePort(_defaultPort);

        addListener(this);
    }

    Value& getRoomSizeValue()
    {
        return _roomSizeValue;
    }

    Value& getDampingValue()
    {
        return _dampingValue;
    }

    void changePort(int port)
    {
        if (! connect(port))
        {
            DBG("Connection Failed");
            showConnectionErrorMessage("Error: could not connect to UDP port " + String(port) + ".");
        }
        else
        {
            DBG("Connection Succeeded");
        }
    }

    void updateSynthName(String name)
    {
        _synthName = name;
        buildAddressPatterns();
    }

private:
    void buildAddressPatterns()
    {
        _roomSizeAddressPattern = "/parameter/" + _synthName + "/Room_Size";
        _dampingAddressPattern = "/parameter/" + _synthName + "/Damping";
    }

    void oscMessageReceived(const OSCMessage& message) override
    {
        DBG("Message Received: ");
        if (message.size() == 1 && message[0].isFloat32())
        {
            DBG("    value " + String(message[0].getFloat32()) + " to AP " + message.getAddressPattern().toString());

            if(message.getAddressPattern().matches(_roomSizeAddressPattern))
                _roomSizeValue.setValue (jlimit (0.0f, 1.0f, message[0].getFloat32()));
            else if(message.getAddressPattern().matches(_dampingAddressPattern))
                _dampingValue.setValue (jlimit (0.0f, 1.0f, message[0].getFloat32()));
        }
    }

    void showConnectionErrorMessage(const String& messageText)
    {
        AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
                                          "Connection error",
                                          messageText,
                                          "OK");
    }

    int _defaultPort {24024};

    String _synthName {"Elk_JUCE_Example_Synth"};
    String _roomSizeAddressPattern {"/parameter/Elk_JUCE_Example_Synth/Room_Size"};
    String _dampingAddressPattern {"/parameter/Elk_JUCE_Example_Synth/Damping"};

    Value _roomSizeValue {0.5f};
    Value _dampingValue {0.5f};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthOSCReceiver)
};
