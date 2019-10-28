#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "../../elk_juce_example_common/SynthGUI.h"

class MainComponent : public Component,
        private Label::Listener
{
public:
    MainComponent();
    ~MainComponent();

    void paint (Graphics&) override;
    void resized() override;

private:
    void connect();

    void updateIP (String ip);

    void updatePort (int port);

    void buildAddressPatterns();

    void labelTextChanged (Label* labelThatHasChanged) override;

    void showConnectionErrorMessage (const String& messageText);

    SynthGUI synthGUI;

    OSCSender oscSender;

    String targetIP {"127.0.0.1"};
    int targetPort {24024};

    String synthName {"Elk_JUCE_Example_Synth"};
    String roomSizeAddressPattern {"/parameter/Elk_JUCE_Example_Synth/Room_Size"};
    String dampingAddressPattern {"/parameter/Elk_JUCE_Example_Synth/Damping"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
