#include "MainComponent.h"

MainComponent::MainComponent():
        synthGUI (SynthGUI::FieldOptions::ShowIPField)
{
    addAndMakeVisible (synthGUI);

    Slider& roomSizeSlider = synthGUI.getRoomSizeSlider();

    roomSizeSlider.setRange (0.0, 1.0);
    roomSizeSlider.setValue (0.5f);

    roomSizeSlider.onValueChange = [this]
    {
        // create and send an OSC message with an address and a float value:
        float value = static_cast <float> (synthGUI.getRoomSizeSlider().getValue());

        if (! oscSender.send (roomSizeAddressPattern, value))
        {
            showConnectionErrorMessage ("Error: could not send OSC message.");
        }
        else
        {
            DBG ("Sent value " + String (value) + " to AP " + roomSizeAddressPattern);
        }
    };

    Slider& dampingSlider = synthGUI.getDampingSlider();

    dampingSlider.setRange (0.0, 1.0);
    dampingSlider.setValue (0.5f);

    dampingSlider.onValueChange = [this]
    {
        // create and send an OSC message with an address and a float value:
        float value = static_cast <float> (synthGUI.getDampingSlider().getValue());

        if (! oscSender.send (dampingAddressPattern, value))
        {
            showConnectionErrorMessage ("Error: could not send OSC message.");
        }
        else
        {
            DBG ("Sent value " + String (value) + " to AP " + dampingAddressPattern);
        }
    };

    synthGUI.getPortNumberField().addListener (this);
    synthGUI.getIPField().addListener (this);
    synthGUI.getSynthNameField().addListener (this);

    connect();

    setSize (700, 400);
}

MainComponent::~MainComponent() {}

void MainComponent::connect()
{
    // specify here where to send OSC messages to: host URL and UDP port number
    if (! oscSender.connect (targetIP, targetPort))
    {
        showConnectionErrorMessage ("Error: could not connect to UDP port " + String(targetPort) + ".");
    }
}

void MainComponent::updateIP (String ip)
{
    targetIP = ip;
    connect();
}

void MainComponent::updatePort (int port)
{
    targetPort = port;
    connect();
}

void MainComponent::buildAddressPatterns()
{
    roomSizeAddressPattern = "/parameter/" + synthName + "/Room_Size";
    dampingAddressPattern = "/parameter/" + synthName + "/Damping";
}

void MainComponent::showConnectionErrorMessage (const String& messageText)
{
    AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
                                     "UDP connection error",
                                     messageText,
                                     "OK");
}

void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    synthGUI.setBounds (getLocalBounds());
}

void MainComponent::labelTextChanged (Label* labelThatHasChanged)
{
    if (labelThatHasChanged == &synthGUI.getPortNumberField())
    {
        const int newPort = synthGUI.getPortNumberField().getTextValue().toString().getIntValue();
        updatePort (newPort);
    }
    else if (labelThatHasChanged == &synthGUI.getIPField())
    {
        const String newIP = synthGUI.getIPField().getTextValue().toString();
        updateIP(newIP);
    }
    else if (labelThatHasChanged == &synthGUI.getSynthNameField())
    {
        synthName = synthGUI.getSynthNameField().getTextValue().toString();
        buildAddressPatterns();
    }
}