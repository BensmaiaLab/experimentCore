#! /usr/bin/python3
"""Takes data streams as input, with a schema, and records them."""

class Recorder():
    """Record data stream to data structure (memory)."""


class FileRecorder(Recorder):
    """Record data stream to a buffered file."""


class DatabaseRecorder(Recorder):
    """Record data stream to a database. (MySQL)"""

