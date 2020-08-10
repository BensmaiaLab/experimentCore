#! /usr/bin/python3
"""Data structures for control server."""

from uuid import UUID, uuid4
from datetime import timedelta


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


class Event(Entity):
    """Contains a reference to a stimulus or sensor in time.
    'time' of Event is relative to start of the
    Routine/Trial/Experiment."""

    def __init__(self, *args, time: timedelta = None, **kwargs):
        super(Event, self).__init__(*args, **kwargs)
        self.time = time
        self.type = None


class Routine():
    """Routines are repeatable sections within trials."""

    def __init__(self):
        self.name = None
        self.uuid = None
        self.events = {}


class Trial():
    """An experiment is composed of Trials."""

    def __init__(self):
        self.routines = {}


class Experiment():
    """An Experiment is the top level object."""
    def __init__(self):
        self.trials = {}


if __name__ == "__main__":
    # Here for testing purposes.
    e = Event()
    print(e)
