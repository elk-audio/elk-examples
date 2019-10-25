#pragma once

#include "MaterialLookAndFeel.h"
#include "ElkLogo.h"

class SynthGUI :
        public juce::Component
{
public:
    enum class FieldOptions {
        ShowIPField,
        HideIPField
    };

    SynthGUI(FieldOptions options = FieldOptions::HideIPField)
        : _roomSizeSlider(Slider::LinearHorizontal, Slider::NoTextBox),
          _dampingSlider(Slider::LinearHorizontal, Slider::NoTextBox)
    {
        _fieldOptions = options;

        _elkGreen = Colour::fromString("FF0EED82");
        _elkYellow = Colour::fromString("FFFFED47");
        _elkPink = Colour::fromString("FFFF5CBE");
        _elkBlue = Colour::fromString("FF0E1C95");

        LookAndFeel::setDefaultLookAndFeel(&_materialLookAndFeel);

        addAndMakeVisible(_roomSizeLabel);
        _roomSizeLabel.attachToComponent (&_roomSizeSlider, false);

        _roomSizeSlider.setColour(Slider::trackColourId, _elkGreen);
        _roomSizeSlider.setColour(Slider::thumbColourId, _elkYellow);

        addAndMakeVisible(_roomSizeSlider);

        addAndMakeVisible(_dampingLabel);
        _dampingLabel.attachToComponent(&_dampingSlider, false);

        _dampingSlider.setColour(Slider::trackColourId, _elkGreen);
        _dampingSlider.setColour(Slider::thumbColourId, _elkPink);

        addAndMakeVisible(_dampingSlider);

        MemoryInputStream in(ElkLogo::Elk_audio_logo_svg, ElkLogo::Elk_audio_logo_svgSize, false);

        _LogoIcon = Drawable::createFromImageDataStream(in);

        if (_LogoIcon != nullptr)
            _LogoIcon->replaceColour(Colours::black, _elkGreen);

        addAndMakeVisible(_LogoIcon.get());

        // Name controls:
        addAndMakeVisible(_synthNameLabel);
        _synthNameField.setEditable(true, true, true);
        addAndMakeVisible(_synthNameField);

        // IP controls:
        if(options == FieldOptions::ShowIPField)
        {
            _portNumberLabel.setText("Outgoing OSC Port: ", NotificationType::dontSendNotification);
            _ipField.setEditable(true, true, true);
            addAndMakeVisible(_ipLabel);
            addAndMakeVisible(_ipField);
        }

        // Port controls:
        addAndMakeVisible(_portNumberLabel);
        _portNumberField.setEditable(true, true, true);
        addAndMakeVisible(_portNumberField);

        setSize(700, 400);
    }

    void paint(Graphics& g) override
    {
        g.fillAll(findColour(ResizableWindow::backgroundColourId));
    }

    void resized() override
    {
        auto r = getLocalBounds();
        auto logoHeight = r.getHeight() / 3;
        auto margin = r.getHeight() / 10;
        auto sliderHeight = r.getHeight() / 6;
        int labelWidth = 160;

        _LogoIcon->setTransformToFit(r.removeFromLeft(proportionOfWidth(0.33))
                                         .withSizeKeepingCentre(logoHeight, logoHeight)
                                         .toFloat(),
                                     RectanglePlacement::fillDestination);

        r.removeFromTop(sliderHeight);

        auto labelsRow = r.removeFromTop(sliderHeight).withSizeKeepingCentre(r.getWidth(), sliderHeight);

        int spacing = 0;

        if(_fieldOptions == FieldOptions::ShowIPField)
            spacing = 15;

        _synthNameLabel.setBounds(labelsRow.getX(), labelsRow.getCentreY()/2-30-spacing, labelWidth, 25);
        _synthNameField.setBounds(labelsRow.getX() + labelWidth, labelsRow.getCentreY()/2-30-spacing, 250, 25);

        _ipLabel.setBounds(labelsRow.getX(), labelsRow.getCentreY()/2-spacing, labelWidth, 25);
        _ipField.setBounds(labelsRow.getX() + labelWidth, labelsRow.getCentreY()/2-spacing, 150, 25);

        _portNumberLabel.setBounds(labelsRow.getX(), labelsRow.getCentreY()/2+spacing, labelWidth, 25);
        _portNumberField.setBounds(labelsRow.getX() + labelWidth, labelsRow.getCentreY()/2+spacing, 50, 25);

        _roomSizeSlider.setBounds(r.removeFromTop (sliderHeight));

        r.removeFromTop(margin);

        _dampingSlider.setBounds(r.removeFromTop (sliderHeight));
    }

    Slider& getRoomSizeSlider()
    {
        return _roomSizeSlider;
    }

    Slider& getDampingSlider()
    {
        return _dampingSlider;
    }

    Label& getPortNumberField()
    {
        return _portNumberField;
    }

    Label& getIPField()
    {
        return _ipField;
    }

    Label& getSynthNameField()
    {
        return _synthNameField;
    }

private:
    MaterialLookAndFeel _materialLookAndFeel;

    Slider _roomSizeSlider;
    Slider _dampingSlider;

    std::unique_ptr<Drawable> _LogoIcon;

    FieldOptions _fieldOptions {FieldOptions::HideIPField};

    Label _synthNameLabel {{}, "Synth Name (no spaces): " };
    Label _synthNameField {{}, "Elk_JUCE_Example_Synth" };

    Label _ipLabel {{}, "Target IP Address: " };
    Label _ipField {{}, "127.0.0.1" };

    Label _portNumberLabel {{}, "Incoming OSC Port: " };
    Label _portNumberField {{}, "24024" };

    Label _roomSizeLabel {{}, "Room Size:" };
    Label _dampingLabel {{}, "Damping:" };

    Colour _elkGreen;
    Colour _elkYellow;
    Colour _elkPink;
    Colour _elkBlue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthGUI)
};