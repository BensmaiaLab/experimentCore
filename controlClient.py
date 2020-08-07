#! /bin/python3
""" Control Client application."""
import argparse
from cc.common import ControlClientApp

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=('''
Control Client GUI App for BensmaiaLab.
'''))
    parser.add_argument('--verbose', '-v', action='store_true')
    cliArgs = parser.parse_args()
    ccApp = ControlClientApp(0)
    ccApp.MainLoop()
