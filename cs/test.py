""" Integrate and test... """

from cs.components.nidaq import DAQ
from cs.recorder import Recorder

rec = Recorder('samples')
daq = DAQ()
rec.recordSamples(daq.sampleStream().tolist())
rec.print()


#? Think it might be time to think about this: https://github.com/kingspp/python-module-boilerplate