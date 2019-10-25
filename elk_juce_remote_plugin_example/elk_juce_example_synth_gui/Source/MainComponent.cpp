#include "MainComponent.h"

MainComponent::MainComponent():
        _synthGUI(SynthGUI::FieldOptions::ShowIPField)
{
    addAndMakeVisible(_synthGUI);

    Slider& roomSizeSlider = _synthGUI.getRoomSizeSlider();

    roomSizeSlider.setRange(0.0, 1.0);
    roomSizeSlider.setValue(0.5f);
    roomSizeSlider.onValueChange = [this]
    {
        // create and send an OSC message with an address and a float value:
        float value = static_cast<float>(_synthGUI.getRoomSizeSlider().getValue());
        if (! _OSC_sender.send(_roomSizeAddressPattern, value))
        {
            showConnectionErrorMessage("Error: could not send OSC message.");
        }
        else
        {
            DBG("Sent value " + String(value) + " to AP " + _roomSizeAddressPattern);
        }
    };

    Slider& dampingSlider = _synthGUI.getDampingSlider();

    dampingSlider.setRange(0.0, 1.0);
    dampingSlider.setValue(0.5f);
    dampingSlider.onValueChange = [this]
    {
        // create and send an OSC message with an address and a float value:
        float value = static_cast<float>(_synthGUI.getDampingSlider().getValue());
        if (! _OSC_sender.send(_dampingAddressPattern, value))
        {
            showConnectionErrorMessage("Error: could not send OSC message.");
        }
        else
        {
            DBG("Sent value " + String(value) + " to AP " + _dampingAddressPattern);
        }
    };

    _synthGUI.getPortNumberField().addListener(this);
    _synthGUI.getIPField().addListener(this);
    _synthGUI.getSynthNameField().addListener(this);

    connect();

    setSize(700, 400);
}

MainComponent::~MainComponent() {}

void MainComponent::connect()
{
    // specify here where to send OSC messages to: host URL and UDP port number
    if (! _OSC_sender.connect(_ip, _port))
    {
        showConnectionErrorMessage("Error: could not connect to UDP port " + String(_port) + ".");
    }
}

void MainComponent::updateIP(String ip)
{
    _ip = ip;
    connect();
}

void MainComponent::updatePort(int port)
{
    _port = port;
    connect();
}

void MainComponent::buildAddressPatterns()
{
    _roomSizeAddressPattern = "/parameter/" + _synthName + "/Room_Size";
    _dampingAddressPattern = "/parameter/" + _synthName + "/Damping";
}

void MainComponent::showConnectionErrorMessage(const String& messageText)
{
    AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
                                     "UDP connection error",
                                     messageText,
                                     "OK");
}

void MainComponent::paint(Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    _synthGUI.setBounds(getLocalBounds());
}

void MainComponent::labelTextChanged(Label* labelThatHasChanged)
{
    if(labelThatHasChanged == &_synthGUI.getPortNumberField())
    {
        const int newPort = _synthGUI.getPortNumberField().getTextValue().toString().getIntValue();
        updatePort(newPort);
    }
    else if(labelThatHasChanged == &_synthGUI.getIPField())
    {
        const String newIP = _synthGUI.getIPField().getTextValue().toString();
        updateIP(newIP);
    }
    else if(labelThatHasChanged == &_synthGUI.getSynthNameField())
    {
        _synthName = _synthGUI.getSynthNameField().getTextValue().toString();
        buildAddressPatterns();
    }
}