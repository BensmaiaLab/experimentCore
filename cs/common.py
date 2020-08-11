#! /usr/bin/python3
"""Common stuff for implementing server."""

from uuid import UUID
import itertools
import io

def isUUID(obj):
    """Try cast `obj` as UUID, return result if true, else False."""
    try:
        return UUID(obj)
    except ValueError:
        return False


class Entity():
    """Generic object."""

    def __init__(
            self,
            name: str = "NewEntity",
            uuid: UUID = None
    ):
        self.name = name
        self.uuid = uuid or uuid4()

    def __repr__(self):
        return 'CS Entity (' + str(self.uuid) + ')'


class BaseClass():
    """Just an example to remind myself of avail features."""

    def __init__(self):
        # super().__init__(args, kwargs)
        self.__version__ = '1.0'

    def __del__(self):
        # Close out anything here
        pass

    def __call__(self, *args, **kwargs):
        # New instance of self:
        # return self.__class__()
        return True

    def __repr__(self):
        return 'ControlServer v' + self.__version__

    def __dir__(self):
        return ''

    def __len__(self):
        return 0

    def __getitem__(self, index): #?index: int or str?
        return 0

    def __iter__(self):
        return self

    def __next__(self):
        # self.index += 1
        return self



def fastRawFileRead(filename: str):
    # http://rabexc.org/posts/io-performance-in-python
    """Fastest way to read large block data from disk, maybe?"""
    # Use pickling if possible, this otherwise?
    # Currently this just reads it, destroying the buffer over again.
    tmp = bytearray(1048576) # b = bytearray(16)
    handle = io.open(filename, "rb")
    i = 0
    for i in itertools.count():
        numread = handle.readinto(tmp)
        if not numread:
            break
    print(i)
