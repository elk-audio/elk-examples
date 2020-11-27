# Benchmark Synth

This is a simple tool that uses SUSHI's internal timing mechanism and its gRPC API to accurate measure the CPU load of a synthesizer at NoteON events.
It requires [elkpy](https://github.com/elk-audio/elkpy) to run.

For a list of all the options, see:
```
$ python3 benchmark-synth.py

usage: benchmark-synth [-h] [-a ALSA_PORT] [-p PROCESSOR]
                       [-n NOTES [NOTES ...]] [-d DURATION]

optional arguments:
  -h, --help            show this help message and exit
  -a ALSA_PORT, --alsa-port ALSA_PORT
                        Specify ALSA port to connect to, default: Sushi
  -p PROCESSOR, --processor PROCESSOR
                        Specify Sushi's processor to benchmark
  -n NOTES [NOTES ...], --notes NOTES [NOTES ...]
                        List of MIDI note numbers to send, default: [60]
  -d DURATION, --duration DURATION
                        Note duration in seconds, default: 1.0

```

The script sends NoteON / NoteOFF events to SUSHI at regular timing intervals, and collects the CPU load of the specified processor with the option **-p**, which should match the plugin name in the SUSHI's JSON configuration file used.

For example, using the *config_obxd.json* included in the extra plugins directory, you will first need to run SUSHI:

```
$ sushi -r -c config_obxd.json
```

Then, in a separate shell, simply run:

```
$ python3 benchmark-synth.py -p obxd
```

which will emit a NoteON with Note Number 60 every one second. To benchmark a polyphonic usage, use the **-n** option and pass a list of note numbers, e.g.:

```
$ python3 benchmark-synth.py -p obxd -n 60 64 67 70
```

The numbers reported are in the same format as collected in SUSHI's log, i.e. they are expressed in percentage of "available CPU time for an audio interrupt". In other words, values above 100% mean that the plugin is overrunning the audio interrupts and thus causing audio dropouts. The statistics are collected over the desired duration, sometimes it is useful to watch at the **max** column to see if there are significant differences compared to the average; in that case, the plugin is performing CPU-intensive operations only during few callbacks.
