#! /usr/bin/python3
# pylint: disable=invalid-name
""" Backend control server. Keeps state, and responds to messages about state."""
import argparse
import logging
import signal
import sys


logging.basicConfig(
    format='%(asctime)-15s | %(message)s',
    datefmt="%Y-%m-%dT%H:%M:%S%z")
log = logging.getLogger('controlServer')
log.setLevel(logging.DEBUG)


def shutdown():
    """Acts as main() destructor; called when the main() loop is shutdown."""
    log.info("shutdown()")
    logging.shutdown()


def sigHandler(sig, frame):
    # pylint: disable=unused-argument
    """Handler for posix style signals from OS."""
    log.info('Ctrl+C pressed, exiting cleanly.')
    shutdown()
    sys.exit(0)


def main():
    """ main() loop. """
    running = True
    state = 'Initialized'
    signal.signal(signal.SIGINT, sigHandler) # register sig handler
    log.info("Starting main()")
    while running:
        pass # do Thing


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=('''
Control Server for BensmaiaLab.
'''))
    # parser.add_argument('input', type=str, help='Filename of the log to parse')
    parser.add_argument('--verbose', '-v', action='store_true')
    args = parser.parse_args()

    main()
