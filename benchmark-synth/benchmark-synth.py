import subprocess
import re
import time
import argparse

import alsaseq
from elkpy import sushicontroller as sc

######################
#  Module constants  #
######################

PROC_NAME = "benchmark-synth"

DEFAULT_TARGET_ALSA_PORT = "Sushi"
DEFAULT_MIDI_NOTES = [60]
DEFAULT_NOTE_DURATION = 1.0
INITIAL_MEASUREMENT_DELAY = 0.5

DEFAULT_SUSHI_GRPC_PORT = 51051

SND_SEQ_QUEUE_DIRECT = 253


######################
#  Helper functions  #
######################

def noteonevent(ch, key, vel):
    'Returns an ALSA event tuple to be sent directly with alsaseq.output().'

    return (alsaseq.SND_SEQ_EVENT_NOTEON, alsaseq.SND_SEQ_TIME_STAMP_REAL,
            0, SND_SEQ_QUEUE_DIRECT, (0, 0),
            (0, 0), (0, 0), (ch, key, vel, 0, 0))


def noteoffevent(ch, key, vel):
    'Returns an ALSA event tuple to be sent directly with alsaseq.output().'

    return (alsaseq.SND_SEQ_EVENT_NOTEOFF, alsaseq.SND_SEQ_TIME_STAMP_REAL,
            0, SND_SEQ_QUEUE_DIRECT, (0, 0),
            (0, 0), (0, 0), (ch, key, vel, 0, 0))

def get_alsa_port_by_name(port_name):
    """Get ALSA port number querying aconnect."""
    aconnect_proc = subprocess.Popen(['aconnect', '-l'], stdout=subprocess.PIPE)
    out, dummy = aconnect_proc.communicate()

    FIND_ALSA_PORT = re.compile(r"""client \s (\d+): \s \'%s\'""" % port_name, re.VERBOSE|re.DOTALL|re.MULTILINE)
    alsa_port_match = FIND_ALSA_PORT.findall(str(out))
    if not alsa_port_match:
        raise ValueError("Couldn't find ALSA port %s" % port_name)

    return int(alsa_port_match[0])


if __name__ == "__main__":
    parser = argparse.ArgumentParser(PROC_NAME)
    parser.add_argument("-a", "--alsa-port", default=DEFAULT_TARGET_ALSA_PORT, type=str, help="Specify ALSA port to connect to, default: %s" % DEFAULT_TARGET_ALSA_PORT)
    parser.add_argument("-p", "--processor", type=str, help="Specify Sushi's processor to benchmark")
    parser.add_argument("-n", "--notes", type=int, nargs='+', default=[60], help="List of MIDI note numbers to send, default: %s" % DEFAULT_MIDI_NOTES)
    parser.add_argument("-d", "--duration", default=DEFAULT_NOTE_DURATION, type=float, help="Note duration in seconds, default: %s" % DEFAULT_NOTE_DURATION)

    args = parser.parse_args()

    c = sc.SushiController()
    processor_id = c.audio_graph.get_processor_id(args.processor)

    alsaseq.client(PROC_NAME, 0, 1, True)
    alsaseq.connectto(0, get_alsa_port_by_name(args.alsa_port), 0)
    alsaseq.start()

    c.timings.set_timings_enabled(True)
    c.timings.reset_all_timings()
    time.sleep(0.5)
    timings_no_load = c.timings.get_processor_timings(processor_id)
    print("Processor load without Note ONs:  %s avg, %s max" % (timings_no_load[0], timings_no_load[2]))

    while (True):
        c.timings.reset_all_timings()
        for note in args.notes:
            alsaseq.output(noteonevent(0, note, 127))

        time.sleep(args.duration)
        timings_end = c.timings.get_processor_timings(processor_id)
        for note in args.notes:
            alsaseq.output(noteoffevent(0, note, 127))

        print("Notes: %s, %s avg, %s max" % (args.notes,
            timings_end[0], timings_end[2]))
        time.sleep(0.5 * args.duration)
