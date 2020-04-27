# elk-examples

For the example projects that complement the online [elk-docs](https://elk-audio.github.io/elk-docs/html/index.html), and the Development Kit release.

The binary files needed for them can be downloaded from the [repository's Releases section](https://github.com/elk-audio/elk-examples/releases/).

The contents of the folders are as follows:

**Blackboard**

The examples under the `blackboard` folder, are specifically made to run on the Elk Pi, using the  Blackboard controller board. They are compatible both with the Raspberry Pi 3 b+ and Pi 4, but you will need to separately download the plugin binaries specific for each platform.

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

The source code and JUCE `.jucer` file, for building our Elk JUCE example vst2 plugin.

The plugin is accompanied by a python program for integrating it with the Blackboard controls, and with an instance of the plugin running on a desktop computer, synchronizing the two plugin instance's (Elk Pi and desktop) state over OSC.

**Benchmark Synth**

This is a simple tool that uses Sushi's internal timing mechanism and its gRPC API to accurate measure the CPU load of a synthesizer at NoteON events.

See the [README.md in the benchmark-synth subfolder](benchmark-synth/README.md) for more details.