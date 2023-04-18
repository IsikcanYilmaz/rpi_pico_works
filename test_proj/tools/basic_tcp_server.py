#!/usr/bin/env python3

import socketserver
import argparse

HOST = "localhost"
PORT = 9999

# https://stackoverflow.com/questions/54353238/how-can-i-create-a-tcp-server-using-python-3
class TCPHandler(socketserver.BaseRequestHandler):
    def handle(self):
        # self.request is the TCP socket connected to the client
        self.data = self.request.recv(1024).strip()
        print("{} wrote:".format(self.client_address[0]))
        print(self.data)
        # just send back the same data, but upper-cased
        self.request.sendall(self.data.upper())
        # here you can do self.request.sendall(use the os library and display the ls command)


def main():
    # Create the server, binding to localhost on port 9999
    with socketserver.TCPServer((HOST, PORT), TCPHandler) as server:
        # Activate the server; this will keep running until you
        # interrupt the program with Ctrl-C
        server.serve_forever()

if __name__ == "__main__":
    main()
