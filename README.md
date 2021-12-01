# elk-examples

For the example projects that complement the online [elk-docs](https://elk-audio.github.io/elk-docs/html/index.html), and the Development Kit release.

The binary files needed for them can be downloaded from the [repository's Releases section](https://github.com/elk-audio/elk-examples/releases/).

The contents of the folders are as follows:

**MDA JX10 VST 2**

This example demonstrates how to control the mda jx10 vst2 plugin using OSC messages, by means of the Open Stage Control GUI provided in the folder.

A configuration file with a path to the plugin appropriate for the Linux desktop is provided.

**MDA JX10 VST 3**

This example demonstrates how to control the mda jx10 vst3 plugin using OSC messages, by means of two alternative applications:

* Open Stage Control, using the GUI provided in the folder.
* TouchOSC, again with a .touchosc GUI file in the folder.

A configuration file with a path to the plugin appropriate for the Linux desktop is provided.

**MDA JX10 lv2vst**

A demonstration of how to load an lv2 plugin, using the lv2vst wrapper, along with example files for Linux desktop, and a corresponding Open Stage Control GUI.

**Elk JUCE Example**

The source code and CMake / **.jucer** file, for building our Elk JUCE example VST 3 plugin.

You can build it either with [JUCE 6's new CMake support, or Projucer](https://elk-audio.github.io/elk-docs/html/documents/building_plugins_for_elk.html).

The plugin is accompanied by a python program for integrating it with the Blackboard controls, and with an instance of the plugin running on a desktop computer, synchronising the two plugin instance's (Elk Pi and desktop) state over OSC.

**Multi-FX**

This example demonstrates using Sushi as a host of multiple effects applied on the audio input signal. It includes an Open Stage Control GUI example, and configuration files for running on the desktop, and on the Elk Pi. All the plugins used are already installed on the Elk Pi image. When running on the Elk Pi with a blackboard, it can also be used with the full "general controller" python glue app, for which there is a batch script in the "general-controller" folder.

**Blackboard**

The examples under the **blackboard** folder, are specifically made to run on the Elk Pi, using the  Blackboard controller board. They are compatible both with the Raspberry Pi 3 b+ and Pi 4, but you will need to separately download the plugin binaries specific to each platform.

These examples are more involved, so ensure you read the [online documentation](https://elk-audio.github.io/elk-docs/html/documents/elk_examples_overview.html#complete-blackboard-examples) closely to get started.

**Benchmark Synth**

This is a simple tool that uses Sushi's internal timing mechanism and its gRPC API to accurate measure the CPU load of a synthesizer at NoteON events.

See the [README.md in the benchmark-synth subfolder](benchmark-synth/README.md) for more details.
