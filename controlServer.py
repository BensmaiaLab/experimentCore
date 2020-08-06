#! /usr/bin/python3
# pylint: disable=invalid-name
""" Backend control server. Keeps state, and responds to messages about state."""
import argparse
import logging
import signal
import sys
import os
import json

from concurrent import futures
import grpc
import pyClient.helloWorld_pb2
import pyClient.helloWorld_pb2_grpc

logging.basicConfig(
    format='%(asctime)-15s %(name)s|%(levelname)s| %(message)s',
    datefmt="%Y-%m-%dT%H:%M:%S%z")
log = logging.getLogger('controlServer')
log.setLevel(logging.DEBUG)

def loadConfig(filename: str = "configServer.json"):
    """Loads JSON config file from disk."""
    if not os.path.exists(filename):
        log.info("No config file found, setting default config")
        return {"serverAddress": "localhost", "serverPort": 42000}
    else:
        with open(filename) as configFile:
            return json.load(configFile)

config = loadConfig()

class helloServicer(pyClient.helloWorld_pb2_grpc.helloRPCServicer):
    """Implements the helloRPC server."""

    def sendRequest(self, request, context):
        """Implements stuff."""
        name = request.name
        log.info("sendRequest('%s')", name)
        return pyClient.helloWorld_pb2.helloReply(reply=f"Hello, {name}!")


def serve():
    """Run the server."""
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    pyClient.helloWorld_pb2_grpc.add_helloRPCServicer_to_server(
        helloServicer(), server)
    #TODO: during config load, confirm required params are there
    address = config['listenAddress'] + ':' + str(config['port'])
    log.info("Listening on: %s", address)
    server.add_insecure_port(address)
    server.start()
    server.wait_for_termination()


def shutdown():
    """Destructor stuff."""
    log.info("shutdown() called.")
    logging.shutdown()


def sigHandler(sig, frame):
    # pylint: disable=unused-argument
    """Handler for posix style signals from OS."""
    shutdown()
    sys.exit(0)


def main():
    """ main() loop. """
    running = True
    # state = 'Initialized'
    signal.signal(signal.SIGINT, sigHandler) # register sig handler
    log.info("Starting main()")
    while running:
        serve()

    # Destructor stuff
    shutdown()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=('''
Control Server for BensmaiaLab.
'''))
    # parser.add_argument('input', type=str, help='Filename of the log to parse')
    parser.add_argument('--verbose', '-v', action='store_true')
    args = parser.parse_args()
    main()
