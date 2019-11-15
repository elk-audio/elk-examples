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

    Value& getCutoffValue()
    {
        return cutoffValue;
    }

    Value& getResonanceValue()
    {
        return resonanceValue;
    }

    void changePort (int port)
    {
        if (! connect (port))
        {
            connected = false;
            DBG ("Connection Failed");
        }
        else
        {
            connected = true;
            DBG("Connection Succeeded");
        }
    }

    void updateSynthName (String name)
    {
        synthName = name;
        buildAddressPatterns();
    }

    bool isConnected()
    {
        return connected;
    }

private:
    void buildAddressPatterns()
    {
        roomSizeAddressPattern = "/parameter/" + synthName + "/Room_Size";
        dampingAddressPattern = "/parameter/" + synthName + "/Damping";
        cutoffAddressPattern = "/parameter/" + synthName + "/Cutoff";
        resonanceAddressPattern = "/parameter/" + synthName + "/Resonance";
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
            else if (message.getAddressPattern().matches (cutoffAddressPattern))
            {
                cutoffValue.setValue (jlimit (0.0f, 1.0f, message[0].getFloat32()));
            }
            else if (message.getAddressPattern().matches (resonanceAddressPattern))
            {
                resonanceValue.setValue (jlimit (0.0f, 1.0f, message[0].getFloat32()));
            }
        }
    }

    int defaultPort {25024};

    String synthName {"elk_juce_example"};
    String roomSizeAddressPattern {"/parameter/elk_juce_example/Room_Size"};
    String dampingAddressPattern {"/parameter/elk_juce_example/Damping"};
    String cutoffAddressPattern {"/parameter/elk_juce_example/Cutoff"};
    String resonanceAddressPattern {"/parameter/elk_juce_example/Resonance"};

    Value roomSizeValue {0.5f};
    Value dampingValue {0.5f};

    Value cutoffValue {0.5f};
    Value resonanceValue {0.5f};

    bool connected = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthOSCReceiver)
};
