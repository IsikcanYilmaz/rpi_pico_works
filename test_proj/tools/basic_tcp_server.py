#!/usr/bin/env python3

import socket
import socketserver
import argparse

HOST = "192.168.50.222"
PORT = 9999

# https://stackoverflow.com/questions/54353238/how-can-i-create-a-tcp-server-using-python-3
class TCPEcho(socketserver.BaseRequestHandler):
    def handle(self):
        # self.request is the TCP socket connected to the client
        self.data = self.request.recv(1024).strip()
        print("{} wrote:".format(self.client_address[0]))
        print(self.data)
        # just send back the same data, but upper-cased
        self.request.sendall(self.data.upper())
        # here you can do self.request.sendall(use the os library and display the ls command)

class TCPSendText(socketserver.BaseRequestHandler):
    def handle(self):
        print("Enter text:")
        self.data = input()
        self.request.sendall(bytes(self.data.upper().encode()))

def serveForever():
    # # Create the server, binding to localhost on port 9999
    # with socketserver.TCPServer((HOST, PORT), TCPEcho) as server:
    #     # Activate the server; this will keep running until you
    #     # interrupt the program with Ctrl-C
    #     server.serve_forever()
    with socketserver.TCPServer((HOST,PORT), TCPSendText) as server:
        server.serve_forever()

def main():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT))
        s.listen(1)
        conn, addr = s.accept()
        with conn:
            print("Connection: ", addr)
            while True:
                data = input("Enter input:")
                conn.send(bytes(data.encode()))
                

if __name__ == "__main__":
    main()
