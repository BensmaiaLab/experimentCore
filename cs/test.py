""" Integrate and test... """

from components.nidaq import DAQ
from recorder import Recorder

rec = Recorder('samples')
daq = DAQ()
rec.recordSamples(daq.sampleStreamOnce())
print("10 samples in sequence, I think:")
rec.print()


#? Think it might be time to think about this: https://github.com/kingspp/python-module-boilerplate
