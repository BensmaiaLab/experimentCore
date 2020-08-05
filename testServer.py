#! /usr/bin/python3
# pylint: disable=invalid-name
""" Writing the test server. """

from concurrent import futures
import logging

import grpc
import pyClient.helloWorld_pb2
import pyClient.helloWorld_pb2_grpc


class helloServicer(pyClient.helloWorld_pb2_grpc.helloRPCServicer):
    """Implements the helloRPC server."""

    def sendRequest(self, request, context):
        """Implements stuff."""
        return pyClient.helloWorld_pb2.helloReply(f"Hello, {request}!")


def serve():
    """Run the server."""
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    pyClient.helloWorld_pb2_grpc.add_helloRPCServicer_to_server(
        helloServicer(), server)
    server.add_insecure_port('[::]:50051')
    server.start()
    # server.wait_for_termination()


if __name__ == '__main__':
    logging.basicConfig()
    serve()
