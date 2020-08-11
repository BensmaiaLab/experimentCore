#! /usr/bin/python3
"""Motor controller classes."""

from .common import Entity


class Motor(Entity):
    """Generic motor implementation."""

        def __init__(self, *args, **kwargs):
        super(Motor, self).__init__(*args, **kwargs)


class TeknicMotor(Motor):
    """Implementation of Teknic ClearPath Software control.
    https://www.teknic.com/products/clearpath-brushless-dc-servo-motors/clearpath-sc/"""

    def __init__(self, *args, **kwargs):
        super(TeknicMotor, self).__init__(*args, **kwargs)

    def scan():
        """SC Hub will initialize connected motors, and return a list of
        available motors."""
        pass