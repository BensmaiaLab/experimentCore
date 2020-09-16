# I want to read from the NI daqmx stuff
""" Some setup to test NIDAQmx in python.."""
import nidaqmx
import nidaqmx.system
from nidaqmx.stream_readers import AnalogSingleChannelReader as ASCR
import numpy


class DAQ:
    """Convenience Wrapper to make it easy to record data from NIDAQ."""

    def __init__(self):
        """Assumes first device by default."""
        self.system = nidaqmx.system.System.local()
        if len(self.system.devices) > 0:
            self.device = self.system.devices[0]
        else:
            raise RuntimeError("NIDAQmx device not found during init." \
            + " Please make sure an NI device is connected to system.")

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

    def sampleStreamOnce(self, devAddr: str = "Dev1/ai0"):
        """Get many samples in a stream."""
        sampleSize = 100
        sampleArray = numpy.zeros(sampleSize)
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
            reader.read_many_sample(sampleArray, sampleSize)
        return sampleArray.tolist()

    def sampleStream(self):
        """Return an iterator that gets a sample every time it's queried."""
        pass

def _test():
    daq = DAQ()
    daq.printChans()
    print(daq.sampleStream().tolist())

if __name__ == "__main__":
    _test()
