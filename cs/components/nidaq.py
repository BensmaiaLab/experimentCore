# I want to read from the NI daqmx stuff
""" Some setup to test NIDAQmx in python.."""
import nidaqmx
import nidaqmx.system
from nidaqmx.stream_readers import AnalogSingleChannelReader as ASCR
from nidaqmx import constants as NIconstants
import numpy


class DAQ:
    """Convenience Wrapper to make it easy to record data from NIDAQ."""

    def __init__(self):
        """Assumes first device by default."""
        self.system = nidaqmx.system.System.local()
        self.task = nidaqmx.Task()
        self.sampleSize = 100
        self.devAddr = 'Dev1/ai0'
        self.buffer = numpy.zeros(self.sampleSize)
        self.reader = ASCR(self.task.in_stream)
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

    def _getSamples(self, sampleSize: int = 128, devAddr: str = "Dev1/ai0"):
        """Naive and synchronous. Internal func returns numpy array."""
        sampleArray = numpy.zeros(sampleSize)
        # And "Dev1/ai4" on my test bench
        with self.task as task:
            port = devAddr[(devAddr.find('/') + 1):]  # everything after the /
            if 'ai' in port:
                task.ai_channels.add_ai_voltage_chan(devAddr)
            elif 'di' in port:
                task.di_channels.add_di_chan(devAddr)
            else:
                print('sampleStream(devAddr) type is unsupported oh no')
            self.reader.read_many_sample(sampleArray, sampleSize)
        return sampleArray

    def _callback(
        self,
        taskHandle,
        everyNSamplesEventType,
        numberSamples,
        callbackData
    ):
        """Stuff."""
        self.reader.read_many_sample(
            self.buffer, numberSamples, timeout=NIconstants.WAIT_INFINITELY)
        print(self.buffer.tolist())
        return 0


    def _setupCallback(self):
        """Setup the callback at the beginning."""
        self.task.ai_channels.add_ai_voltage_chan(self.devAddr)
        self.task.timing.cfg_samp_clk_timing(
            rate=100,
            sample_mode=NIconstants.AcquisitionType.CONTINUOUS,
            samps_per_chan=self.sampleSize)
        self.task.register_every_n_samples_acquired_into_buffer_event(
            self.sampleSize,
            self._callback)


    def sampleStreamOnce(self, sampleSize: int = 128, devAddr: str = "Dev1/ai0"):
        """Naive and synchronous. Get a set of samples."""
        self._getSamples(sampleSize, devAddr).tolist()

    def sampleStream(self, sampleSize: int = 128, devAddr: str = "Dev1/ai0"):
        """Naive and synchronous.
        Return an iterator that gets (a set of) samples every time it's queried."""
        class IterSamples:
            """Iterator of samples."""
            def __init__(self, daq: DAQ, sampleSize: int, devAddr: str):
                self.daq = daq
                self.sampleSize = sampleSize
                self.devAddr = devAddr
                self.index = 0  # Really just how many times it's queried

            def __iter__(self):
                return self

            def __next__(self):
                self.index += 1
                return self.daq._getSamples(
                    sampleSize=self.sampleSize,
                    devAddr=self.devAddr).tolist()

        return IterSamples(daq=self, sampleSize=sampleSize, devAddr=devAddr)

def _test():
    daq = DAQ()
    daq.printChans()
    print(daq.sampleStreamOnce().tolist())

if __name__ == "__main__":
    _test()
