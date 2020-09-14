# I want to read from the NI daqmx stuff
""" Some setup to test NIDAQmx in python.."""
import nidaqmx
import nidaqmx.system
from nidaqmx.stream_readers import AnalogSingleChannelReader as ASCR
import numpy

sampleArray = numpy.zeros(10)

system = nidaqmx.system.System.local()
dv = system.driver_version
print(f"NIDAQmx Driver version {dv.major_version}.{dv.minor_version}.{dv.update_version}")

print("Available NIDAQ devices on this system:")
for device in system.devices:
    print(device)

# And "Dev1/ai4" on my test bench
with nidaqmx.Task() as task:
    task.ai_channels.add_ai_voltage_chan("Dev1/ai0")
    reader = ASCR(task.in_stream)
    reader.read_many_sample(sampleArray, 10)

print(sampleArray)