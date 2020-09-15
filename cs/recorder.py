#! /usr/bin/python3
"""Takes data streams as input, with a schema, and records them.

    This pretty much all assumes we're recording samples at time stamps."""

from typing import List


class Recorder():
    """Record data stream to data structure (memory)."""

    def __init__(self, columns: List[str] = None):
        self.columns = columns
        self.colCount = len(columns)
        self.rowCount = 0
        self.data: []

    def __del__(self):
        pass

    def recordSample(self, sample: list):
        """Record a single sample."""
        if self.colCount != len(list):
            return False
        self.data.append(sample)
        self.rowCount = self.rowCount + 1
        return True

    def recordSamples(self, samples: List[list]):
        """Record multiple samples at once."""
        #? Error checking?
        self.data = self.data + samples
        self.rowCount = self.rowCount + len(samples)
        return True

    def print(self):
        """Print the last 10 samples for sanity checking."""
        self.data[-10:]


class FileRecorder():
    """Record data stream to a buffered file."""

    def __init__(self, columns: List[str] = None, filename: str = "default.data"):
        self.columns = columns
        self.colCount = len(columns)
        self.metadata = []
        self.file = open(filename)

    def __del__(self):
        self.file.close()

    def recordSample(self):
        """Record a single sample."""
        pass


class DatabaseRecorder(Recorder):
    """Record data stream to a database. (MySQL)"""

