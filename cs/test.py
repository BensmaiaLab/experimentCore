""" Integrate and test... """
# from itertools import count
from components.nidaq import DAQ
from recorder import Recorder

rec = Recorder('samples')
daq = DAQ()
sampleIter = daq.sampleStream(sampleSize=10)
c = 0
for s in sampleIter:
    if c > 10:
        break
    rec.recordSamples(s)
    print(c)
    c = c + 1
print("10 samples in sequence, I think:")
rec.print()


#? Think it might be time to think about this: https://github.com/kingspp/python-module-boilerplate
