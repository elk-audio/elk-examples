STATE_MACHINE_MAP = {
    "plugin" : {
        "obxd" : {
            "main" : {
                "Ctf" : [{"plugin" : "obxd", "name" : "Cutoff", "value" : 0.0},],
                "Res" : [{"plugin" : "obxd", "name" : "Resonance", "value" : 0.0},],
                "Vlm" : [{"plugin" : "obxd", "name" : "Volume", "value" : 0.0}]
            },
            "flt_env" : {
                "Atk" : [{"plugin" : "obxd", "name" : "FilterAttack", "value" : 0.0},],
                "D/R" : [{"plugin" : "obxd", "name" : "FilterDecay", "value" : 0.0}, {"plugin" : "obxd", "name" : "FilterRelease", "value" : 0.0}], 
                "Amt" : [{"plugin" : "obxd", "name" : "FilterEnvAmount", "value" : 0.0},]
            },
            "amp_env" : {
                "Atk" : [{"plugin" : "obxd", "name" : "Attack", "value" : 0.0},], 
                "D/R" : [{"plugin" : "obxd", "name" : "Decay", "value" : 0.0}, {"plugin" : "obxd", "name" : "Release", "value" : 0.0}], 
                "Sus" : [{"plugin" : "obxd", "name" : "Sustain", "value" : 0.0},]
            }
        },
        "reverb" : {
            "main" : {
                "Dec" : [{"plugin" : "reverb", "name" : "Decay", "value" : 0.0},],
                "D/W" : [{"plugin" : "reverb", "name" : "Mix", "value" : 0.0},],
                "E/L" : [{"plugin" : "reverb", "name" : "Early/Late Mix", "value" : 0.0},]
            },
            "pg2" : {
                "Siz" : [{"plugin" : "reverb", "name" : "Size", "value" : 0.0},],
                "Dmp" : [{"plugin" : "reverb", "name" : "Damping", "value" : 0.0},],
                "PrD" : [{"plugin" : "reverb", "name" : "Predelay", "value" : 0.0},]
            }
        },
        "gm_midi" : {
            "main" : {
                "Bnk" : [{"plugin" : "gm_midi", "name" : "bnk", "value" : 0.0},],
                "Pgm" : [{"plugin" : "gm_midi", "name" : "pgm", "value" : 0.0},],
                "Vol" : [{"plugin" : "piano_gain", "name" : "gain", "value" : 0.0},]
            }
        },
        "delay" : {
            "main" : {
                "Fbk" : [{"plugin" : "delay", "name" : "Feedback", "value" : 0.0},],
                "Dly" : [{"plugin" : "delay", "name" : "L Delay ", "value": 0.0},{"plugin" : "delay", "name" : "R Delay ", "value": 0.0}],
                "D/W" : [{"plugin" : "delay", "name" : "FX Mix  ", "value" : 0.0},]
            }
        }

    },
    "sequencer_obxd" : {
        "stp 1" : (0, False),
        "stp 2" : (0, False),
        "stp 3" : (0, False),
        "stp 4" : (0, False),
        "stp 5" : (0, False),
        "stp 6" : (0, False),
        "stp 7" : (0, False),
        "stp 8" : (0, False)
    },
    "sequencer_piano" : {
        "stp 1" : (0, False),
        "stp 2" : (0, False),
        "stp 3" : (0, False),
        "stp 4" : (0, False),
        "stp 5" : (0, False),
        "stp 6" : (0, False),
        "stp 7" : (0, False),
        "stp 8" : (0, False)
    }
}

class StateMachine(object):
    def __init__(self):
        self._state_map = STATE_MACHINE_MAP
        self._selected_plugin = "obxd"
        self._parameter_page = "main"
        self._mode = "plugin"
        self._active_sequencer = "sequencer_obxd"

    def get_state_text(self):
        s = [] 
        if self._mode == "plugin":
            s.append(self._mode + ": " + self._selected_plugin + ": " + self._parameter_page)
            s.append(self.get_knob_name(0) + "    " + self.get_knob_name(1) + "    " + self.get_knob_name(2))
            s.append("{:.2}  {:.2}  {:.2}".format(
                float(self._state_map[self._mode][self._selected_plugin][self._parameter_page][self.get_knob_name(0)][0]["value"]),
                float(self._state_map[self._mode][self._selected_plugin][self._parameter_page][self.get_knob_name(1)][0]["value"]),
                float(self._state_map[self._mode][self._selected_plugin][self._parameter_page][self.get_knob_name(2)][0]["value"])))
        elif "sequencer" in self._mode:
            steps = self._state_map[self._active_sequencer]
            for step in steps:
                s.append("{} {} {}".format(step, steps[step][0], steps[step][1]))
        return s

    def cycle_parameter_page(self, dir):
        parameter_pages = list(self._state_map["plugin"][self._selected_plugin].keys())
        self._parameter_page = parameter_pages[(parameter_pages.index(self._parameter_page) + dir) % len(parameter_pages)]
 
    def cycle_active_plugin(self):
        plugins = list(self._state_map["plugin"].keys())
        self._parameter_page = "main"
        self._selected_plugin = plugins[(plugins.index(self._selected_plugin) + 1) % len(plugins)]
        if self._selected_plugin == "obxd" or self._selected_plugin == "reverb":
            self._active_sequencer = "sequencer_obxd"
        else:
            self._active_sequencer = "sequencer_piano"
    
    def get_page_parameters(self, index):
        parameters = self._state_map["plugin"][self._selected_plugin][self._parameter_page]
        key = list(parameters.keys())[index]
        return parameters[key]


    def get_knob_name(self, index):
        return list(self._state_map["plugin"][self._selected_plugin][self._parameter_page].keys())[index]

    def get_plugin(self, index):
        parameters = self._state_map["plugin"][self._selected_plugin][self._parameter_page]
        key = list(parameters.keys())[index]
        return parameters[key][0]["plugin"]

    def get_mode(self):
        if "sequencer" in self._mode:
            return "sequencer"
        return self._mode

    def get_map(self):
        plugins = self._state_map["plugin"]
        out = []
        for plugin in plugins:
            pages = self._state_map["plugin"][plugin]
            for page in pages:
                knobs = self._state_map["plugin"][plugin][page]
                for knob in knobs:
                    parameters = self._state_map["plugin"][plugin][page][knob]
                    for parameter in parameters:
                        out.append((parameter["plugin"], parameter["name"]))
        return out

    def get_steps(self):
        out = []
        steps = self._state_map[self._active_sequencer]
        for step in steps:
            out.append(steps[step][1])
        return out

    def get_parameter(self, parameter_name):
        plugins = self._state_map["plugin"]
        for plugin in plugins:
            pages = self._state_map["plugin"][plugin]
            for page in pages:
                knobs = self._state_map["plugin"][plugin][page]
                for knob in knobs:
                    parameters = self._state_map["plugin"][plugin][page][knob]
                    for parameter in parameters:
                        if parameter["name"] == parameter_name:
                            return parameter["value"]

    def get_active_sequencer(self):
        return self._active_sequencer

    def set_parameter(self, parameter_name, value):
        plugins = self._state_map["plugin"]
        out = []
        for plugin in plugins:
            pages = self._state_map["plugin"][plugin]
            for page in pages:
                knobs = self._state_map["plugin"][plugin][page]
                for knob in knobs:
                    parameters = self._state_map["plugin"][plugin][page][knob]
                    for parameter in parameters:
                        if parameter["name"] == parameter_name:
                            parameter["value"] = value

    def set_parameter_from_page(self, index, value):
        parameters = self._state_map["plugin"][self._selected_plugin][self._parameter_page]
        key = list(parameters.keys())[index]
        for parameter in parameters[key]:
            parameter["value"] = value
    
    def set_mode(self, mode):
        if mode == "sequncer":
            mode = self._active_sequencer
        self._mode = mode

    def set_sequencer_enabled(self, index, status):
        key = list(self._state_map[self._active_sequencer])[index]
        value = self._state_map[self._active_sequencer][key][0]
        self._state_map[self._active_sequencer][key] = (value, bool(status))

    def set_sequencer_pitch(self, index, value):
        key = list(self._state_map[self._active_sequencer])[index]
        status = self._state_map[self._active_sequencer][key][1]
        self._state_map[self._active_sequencer][key] = (value, not status)

    def toggle_sequencer_enabled(self, index):
        key = list(self._state_map[self._active_sequencer])[index]
        status = self._state_map[self._active_sequencer][key][1]
        value = self._state_map[self._active_sequencer][key][0]
        self._state_map[self._active_sequencer][key] = (value, not status)