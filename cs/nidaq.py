# I want to read from the NI daqmx stuff
""" Some setup to test NIDAQmx in python.."""
import nidaqmx
import nidaqmx.system
from nidaqmx.stream_readers import AnalogSingleChannelReader as ASCR
import numpy

from .recorder import Recorder


class DAQ:
    """Convenience Wrapper to make it easy to record data from NIDAQ."""

    def __init__(self):
        """Assumes first device by default."""
        self.system = nidaqmx.system.System.local()
        self.device = self.system.devices[0]

    def __repr__(self):
        dv = self.system.driver_version
        return "<DAQ Wrapper class - NIDAQmx Driver ver: " +\
            f"{dv.major_version}.{dv.minor_version}.{dv.update_version}>"

    def printChans(self):
        """Returns a list of available device identifiers."""
        print(self.device.name + " | " + self.enumChans())

    def enumChans(self):
        """Returns prettied device detail string."""
        out = ''
        # Analog and counter channels bc they're similiar first:
        for chanType in ['ai', 'ao', 'ci', 'co', 'di', 'do']:
            if chanType in ['di', 'do']:
                count = len(getattr(self.device, chanType + '_lines')) - 1
            else:
                count = len(getattr(self.device, chanType + '_physical_chans')) - 1
            if count == 0:
                out += f'/{chanType}0 '
            elif count == -1:
                out += f'/{chanType}N '
            else:
                out += f'/{chanType}0:{count} '
        return out

    def sampleStream(self, devAddr: str = "Dev1/ai0"):
        """Get many samples in a stream."""
        sampleArray = numpy.zeros(10)
        # And "Dev1/ai4" on my test bench
        with nidaqmx.Task() as task:
            port = devAddr[(devAddr.find('/') + 1):]  # everything after the /
            if 'ai' in port:
                task.ai_channels.add_ai_voltage_chan(devAddr)
            elif 'di' in port:
                task.di_channels.add_di_chan(devAddr)
            else:
                print('sampleStream(devAddr) type is unsupported oh no')
            reader = ASCR(task.in_stream)
            reader.read_many_sample(sampleArray, 10)
        return sampleArray


if __name__ == "__main__":
    r = Recorder('samples')
    d = DAQ()
    r.recordSamples(d.sampleStream().tolist())
    r.print()
    input()
