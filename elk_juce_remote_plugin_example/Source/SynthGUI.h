#pragma once

#include "MaterialLookAndFeel.h"
#include "ElkLogo.h"

class SynthGUI :
        public juce::Component
{
public:
    SynthGUI()
        : roomSizeSlider (Slider::LinearHorizontal, Slider::TextBoxLeft),
          dampingSlider (Slider::LinearHorizontal, Slider::TextBoxLeft),
          cutoffSlider (Slider::LinearHorizontal, Slider::TextBoxLeft),
          resonanceSlider (Slider::LinearHorizontal, Slider::TextBoxLeft)
    {
        elkGreen = Colour::fromString ("FF0EED82");
        elkYellow = Colour::fromString ("FFFFED47");
        elkPink = Colour::fromString ("FFFF5CBE");
        elkBlue = Colour::fromString ("FF0E1C95");

        LookAndFeel::setDefaultLookAndFeel (&materialLookAndFeel);

        // Reverb:

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

        // Filter:

        addAndMakeVisible (cutoffLabel);
        cutoffLabel.attachToComponent (&cutoffSlider, false);

        cutoffSlider.setColour (Slider::trackColourId, elkGreen);
        cutoffSlider.setColour (Slider::thumbColourId, elkYellow);

        addAndMakeVisible (cutoffSlider);

        addAndMakeVisible (resonanceLabel);
        resonanceLabel.attachToComponent (&resonanceSlider, false);

        resonanceSlider.setColour (Slider::trackColourId, elkGreen);
        resonanceSlider.setColour (Slider::thumbColourId, elkPink);

        addAndMakeVisible (resonanceSlider);

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
        ipField.setEditable (true, true, true);
        addAndMakeVisible (ipLabel);
        addAndMakeVisible (ipField);

        // Port controls:
        addAndMakeVisible (outPortNumberLabel);
        outPortNumberField.setEditable (true, true, true);
        addAndMakeVisible (outPortNumberField);
        addAndMakeVisible (outConnectedLabel);

        addAndMakeVisible (inPortNumberLabel);
        inPortNumberField.setEditable (true, true, true);
        addAndMakeVisible (inPortNumberField);
        addAndMakeVisible (inConnectedLabel);
    }

    void paint (Graphics& g) override
    {
        g.fillAll (findColour(ResizableWindow::backgroundColourId));
    }

    void resized() override
    {
        auto r = getLocalBounds();
        auto logoHeight = r.getHeight() / 4;
        auto margin = r.getHeight() / 15;
        auto sliderHeight = r.getHeight() / 12;
        int labelWidth = 160;

        logoIcon->setTransformToFit (r.removeFromLeft (proportionOfWidth (0.33f))
                                         .withSizeKeepingCentre (logoHeight, logoHeight)
                                         .toFloat(),
                                     RectanglePlacement::fillDestination);

        r.removeFromTop(sliderHeight*2);

        auto labelsRow = r.removeFromTop (sliderHeight).withSizeKeepingCentre (r.getWidth(), sliderHeight*2);

        const int spacing = 15;
        synthNameLabel.setBounds (labelsRow.getX(), labelsRow.getCentreY() / 2 - 30 - spacing, labelWidth, 25);
        synthNameField.setBounds (labelsRow.getX() + labelWidth, labelsRow.getCentreY() / 2 - 30 - spacing, 250, 25);

        ipLabel.setBounds (labelsRow.getX(), labelsRow.getCentreY() / 2 - spacing, labelWidth, 25);
        ipField.setBounds (labelsRow.getX() + labelWidth, labelsRow.getCentreY() / 2 - spacing, 150, 25);

        outPortNumberLabel.setBounds (labelsRow.getX(), labelsRow.getCentreY() / 2 + spacing, labelWidth, 25);
        outPortNumberField.setBounds (labelsRow.getX() + labelWidth, labelsRow.getCentreY() / 2 + spacing, 50, 25);
        outConnectedLabel.setBounds (labelsRow.getX() + labelWidth + 50, labelsRow.getCentreY() / 2 + spacing, 150, 25);

        inPortNumberLabel.setBounds (labelsRow.getX(), labelsRow.getCentreY() / 2 + spacing*3, labelWidth, 25);
        inPortNumberField.setBounds (labelsRow.getX() + labelWidth, labelsRow.getCentreY() / 2 + spacing*3, 50, 25);
        inConnectedLabel.setBounds (labelsRow.getX() + labelWidth + 50, labelsRow.getCentreY() / 2 + spacing*3, 150, 25);

        r.removeFromTop (margin);

        cutoffSlider.setBounds (r.removeFromTop (sliderHeight));

        r.removeFromTop (margin);

        resonanceSlider.setBounds (r.removeFromTop (sliderHeight));

        r.removeFromTop (margin);

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

    Slider& getCutoffSlider()
    {
        return cutoffSlider;
    }

    Slider& getResonanceSlider()
    {
        return resonanceSlider;
    }

    Label& getOutPortNumberField()
    {
        return outPortNumberField;
    }

    Label& getInPortNumberField()
    {
        return inPortNumberField;
    }

    Label& getIPField()
    {
        return ipField;
    }

    Label& getSynthNameField()
    {
        return synthNameField;
    }

    Label& getOutConnectedLabel()
    {
        return outConnectedLabel;
    }

    Label& getInConnectedLabel()
    {
        return inConnectedLabel;
    }

private:
    MaterialLookAndFeel materialLookAndFeel;

    Slider roomSizeSlider;
    Slider dampingSlider;

    Slider cutoffSlider;
    Slider resonanceSlider;

    std::unique_ptr <Drawable> logoIcon;

    Label synthNameLabel {{}, "Synth Name (no spaces): " };
    Label synthNameField {{}, "Elk_JUCE_Example_Synth" };

    Label ipLabel {{}, "Target IP Address: " };
    Label ipField {{}, "127.0.0.1" };

    Label outPortNumberLabel {{}, "Outgoing OSC Port: " };
    Label outPortNumberField {{}, "24024" };

    Label inPortNumberLabel {{}, "Incoming OSC Port: " };
    Label inPortNumberField {{}, "24025" };

    Label roomSizeLabel {{}, "Room Size:" };
    Label dampingLabel {{}, "Damping:" };

    Label cutoffLabel {{}, "Cutoff:" };
    Label resonanceLabel {{}, "Resonance:" };

    Label inConnectedLabel {"(connected)"};
    Label outConnectedLabel {"(connected)"};

    Colour elkGreen;
    Colour elkYellow;
    Colour elkPink;
    Colour elkBlue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthGUI)
};