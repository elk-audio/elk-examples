#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
class MaterialLookAndFeel : public LookAndFeel_V4
{
public:
    //==============================================================================
    MaterialLookAndFeel()
    {
        setColour (ResizableWindow::backgroundColourId, windowBackgroundColour);
        setColour (TextButton::buttonOnColourId,        brightButtonColour);
        setColour (TextButton::buttonColourId,          disabledButtonColour);
    }

    //==============================================================================
    void drawButtonBackground (Graphics& g,
                               Button& button,
                               const Colour& /*backgroundColour*/,
                               bool /*isMouseOverButton*/,
                               bool isButtonDown) override
    {
        auto buttonRect = button.getLocalBounds().toFloat();

        if (isButtonDown)
            g.setColour (brightButtonColour.withAlpha (0.7f));
        else if (! button.isEnabled())
            g.setColour (disabledButtonColour);
        else
            g.setColour (brightButtonColour);

        g.fillRoundedRectangle (buttonRect, 5.0f);
    }

    //==============================================================================
    void drawButtonText (Graphics& g, TextButton& button, bool isMouseOverButton, bool isButtonDown) override
    {
        ignoreUnused (isMouseOverButton, isButtonDown);

        Font font (getTextButtonFont (button, button.getHeight()));
        g.setFont (font);

        if (button.isEnabled())
            g.setColour (Colours::white);
        else
            g.setColour (backgroundColour);

        g.drawFittedText (button.getButtonText(), 0, 0,
                          button.getWidth(),
                          button.getHeight(),
                          Justification::centred, 2);
    }

    //==============================================================================
    void drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           const Slider::SliderStyle style, Slider& slider) override
    {
        ignoreUnused (style, minSliderPos, maxSliderPos);

        auto r = Rectangle<int> (x + haloRadius, y, width - (haloRadius * 2), height);
        auto backgroundBar = r.withSizeKeepingCentre(r.getWidth(), 2);

        sliderPos = (sliderPos - minSliderPos) / static_cast<float> (width);

        auto knobPos = static_cast<int> (sliderPos * r.getWidth());

        g.setColour (slider.findColour (Slider::trackColourId));
        g.fillRect (backgroundBar.removeFromLeft (knobPos));

        g.setColour (sliderInactivePart);
        g.fillRect (backgroundBar);

        if (slider.isMouseOverOrDragging())
        {
            auto haloBounds = r.withTrimmedLeft (knobPos - haloRadius)
                    .withWidth (haloRadius * 2)
                    .withSizeKeepingCentre (haloRadius * 2, haloRadius * 2);

            g.setColour (sliderActivePart.withAlpha (0.5f));
            g.fillEllipse (haloBounds.toFloat());
        }

        auto knobRadius = slider.isMouseOverOrDragging() ? knobActiveRadius : knobInActiveRadius;
        auto knobBounds = r.withTrimmedLeft (knobPos - knobRadius)
                .withWidth (knobRadius * 2)
                .withSizeKeepingCentre (knobRadius * 2, knobRadius * 2);

        g.setColour (slider.findColour (Slider::thumbColourId));
        g.fillEllipse (knobBounds.toFloat());
    }

    //==============================================================================
    Font getTextButtonFont (TextButton& button, int buttonHeight) override
    {
        return LookAndFeel_V3::getTextButtonFont (button, buttonHeight).withHeight (buttonFontSize);
    }

    Font getLabelFont (Label& label) override
    {
        return LookAndFeel_V3::getLabelFont (label).withHeight (labelFontSize);
    }

    //==============================================================================
    enum
    {
        labelFontSize  = 15,
        buttonFontSize = 15
    };

    //==============================================================================
    enum
    {
        knobActiveRadius   = 12,
        knobInActiveRadius = 8,
        haloRadius         = 18
    };

    //==============================================================================
    const Colour windowBackgroundColour = Colour (0xff262328);
    const Colour backgroundColour       = Colour (0xff4d4d4d);
    const Colour brightButtonColour     = Colour (0xff80cbc4);
    const Colour disabledButtonColour   = Colour (0xffe4e4e4);
    const Colour sliderInactivePart     = Colour (0xff545d62);
    const Colour sliderActivePart       = Colour (0xff80cbc4);
};