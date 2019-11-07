#include "../JuceLibraryCode/JuceHeader.h"
#include "SynthProcessor.h"

//==============================================================================
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SynthProcessor();
}
