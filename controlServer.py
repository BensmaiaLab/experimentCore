#! /usr/bin/python3
# pylint: disable=invalid-name
""" Backend control server. Keeps state, and responds to messages about state."""
import argparse
import logging
import signal
import sys

from concurrent import futures
import grpc
import pyClient.helloWorld_pb2
import pyClient.helloWorld_pb2_grpc

logging.basicConfig(
    format='%(asctime)-15s %(name)s|%(levelname)s| %(message)s',
    datefmt="%Y-%m-%dT%H:%M:%S%z")
log = logging.getLogger('controlServer')
log.setLevel(logging.DEBUG)


class helloServicer(pyClient.helloWorld_pb2_grpc.helloRPCServicer):
    """Implements the helloRPC server."""

    def sendRequest(self, request, context):
        """Implements stuff."""
        log.info("sendRequest()")
        return pyClient.helloWorld_pb2.helloReply(reply=f"Hello, {request.name}!")


def serve():
    """Run the server."""
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    pyClient.helloWorld_pb2_grpc.add_helloRPCServicer_to_server(
        helloServicer(), server)
    server.add_insecure_port('[::]:50051')
    server.start()
    server.wait_for_termination()


def shutdown():
    """Destructor stuff."""
    log.info("shutdown()")
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
