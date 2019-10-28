#pragma once

#include "MaterialLookAndFeel.h"
#include "ElkLogo.h"

class SynthGUI :
        public juce::Component
{
public:
    enum class FieldOptions
    {
        ShowIPField,
        HideIPField
    };

    SynthGUI (FieldOptions options = FieldOptions::HideIPField)
        : roomSizeSlider (Slider::LinearHorizontal, Slider::NoTextBox),
          dampingSlider (Slider::LinearHorizontal, Slider::NoTextBox)
    {
        fieldOptions = options;

        elkGreen = Colour::fromString ("FF0EED82");
        elkYellow = Colour::fromString ("FFFFED47");
        elkPink = Colour::fromString ("FFFF5CBE");
        elkBlue = Colour::fromString ("FF0E1C95");

        LookAndFeel::setDefaultLookAndFeel (&materialLookAndFeel);

        addAndMakeVisible (roomSizeLabel);
        roomSizeLabel.attachToComponent (&roomSizeSlider, false);

        roomSizeSlider.setColour (Slider::trackColourId, elkGreen);
        roomSizeSlider.setColour (Slider::thumbColourId, elkYellow);

        addAndMakeVisible (roomSizeSlider);

        addAndMakeVisible (dampingLabel);
        dampingLabel.attachToComponent (&dampingSlider, false);

        dampingSlider.setColour (Slider::trackColourId, elkGreen);
        dampingSlider.setColour (Slider::thumbColourId, elkPink);

        addAndMakeVisible (dampingSlider);

        MemoryInputStream in (ElkLogo::Elk_audio_logo_svg, ElkLogo::Elk_audio_logo_svgSize, false);

        logoIcon = Drawable::createFromImageDataStream (in);

        if (logoIcon != nullptr)
        {
            logoIcon->replaceColour (Colours::black, elkGreen);
            addAndMakeVisible (logoIcon.get());
        }

        // Name controls:
        addAndMakeVisible (synthNameLabel);
        synthNameField.setEditable (true, true, true);
        addAndMakeVisible (synthNameField);

        // IP controls:
        if (options == FieldOptions::ShowIPField)
        {
            portNumberLabel.setText ("Outgoing OSC Port: ", NotificationType::dontSendNotification);
            ipField.setEditable (true, true, true);
            addAndMakeVisible (ipLabel);
            addAndMakeVisible (ipField);
        }

        // Port controls:
        addAndMakeVisible (portNumberLabel);
        portNumberField.setEditable (true, true, true);
        addAndMakeVisible (portNumberField);

        setSize (700, 400);
    }

    void paint (Graphics& g) override
    {
        g.fillAll (findColour(ResizableWindow::backgroundColourId));
    }

    void resized() override
    {
        auto r = getLocalBounds();
        auto logoHeight = r.getHeight() / 3;
        auto margin = r.getHeight() / 10;
        auto sliderHeight = r.getHeight() / 6;
        int labelWidth = 160;

        logoIcon->setTransformToFit (r.removeFromLeft (proportionOfWidth (0.33))
                                         .withSizeKeepingCentre (logoHeight, logoHeight)
                                         .toFloat(),
                                     RectanglePlacement::fillDestination);

        r.removeFromTop(sliderHeight);

        auto labelsRow = r.removeFromTop (sliderHeight).withSizeKeepingCentre (r.getWidth(), sliderHeight);

        int spacing = 0;

        if (fieldOptions == FieldOptions::ShowIPField)
            spacing = 15;

        synthNameLabel.setBounds (labelsRow.getX(), labelsRow.getCentreY() / 2 - 30 - spacing, labelWidth, 25);
        synthNameField.setBounds (labelsRow.getX() + labelWidth, labelsRow.getCentreY() / 2 - 30 - spacing, 250, 25);

        ipLabel.setBounds (labelsRow.getX(), labelsRow.getCentreY() / 2 - spacing, labelWidth, 25);
        ipField.setBounds (labelsRow.getX() + labelWidth, labelsRow.getCentreY() / 2 - spacing, 150, 25);

        portNumberLabel.setBounds (labelsRow.getX(), labelsRow.getCentreY() / 2 + spacing, labelWidth, 25);
        portNumberField.setBounds (labelsRow.getX() + labelWidth, labelsRow.getCentreY() / 2 + spacing, 50, 25);

        roomSizeSlider.setBounds (r.removeFromTop (sliderHeight));

        r.removeFromTop (margin);

        dampingSlider.setBounds (r.removeFromTop (sliderHeight));
    }

    Slider& getRoomSizeSlider()
    {
        return roomSizeSlider;
    }

    Slider& getDampingSlider()
    {
        return dampingSlider;
    }

    Label& getPortNumberField()
    {
        return portNumberField;
    }

    Label& getIPField()
    {
        return ipField;
    }

    Label& getSynthNameField()
    {
        return synthNameField;
    }

private:
    MaterialLookAndFeel materialLookAndFeel;

    Slider roomSizeSlider;
    Slider dampingSlider;

    std::unique_ptr <Drawable> logoIcon;

    FieldOptions fieldOptions {FieldOptions::HideIPField};

    Label synthNameLabel {{}, "Synth Name (no spaces): " };
    Label synthNameField {{}, "Elk_JUCE_Example_Synth" };

    Label ipLabel {{}, "Target IP Address: " };
    Label ipField {{}, "127.0.0.1" };

    Label portNumberLabel {{}, "Incoming OSC Port: " };
    Label portNumberField {{}, "24024" };

    Label roomSizeLabel {{}, "Room Size:" };
    Label dampingLabel {{}, "Damping:" };

    Colour elkGreen;
    Colour elkYellow;
    Colour elkPink;
    Colour elkBlue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthGUI)
};