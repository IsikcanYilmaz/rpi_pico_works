#!/usr/bin/env python3

import socket
import socketserver
import argparse
import time

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

class TCPSendPicture(socketserver.BaseRequestHandler):
    def __init__(self):
        f = open("oled_ali.png.bin", "rb")
        self.data = f.read()
        f.close()

    def handle(self):
        self.request.sendall(self.data)

def serveForever():
    # # Create the server, binding to localhost on port 9999
    # with socketserver.TCPServer((HOST, PORT), TCPEcho) as server:
    #     # Activate the server; this will keep running until you
    #     # interrupt the program with Ctrl-C
    #     server.serve_forever()
    with socketserver.TCPServer((HOST,PORT), TCPSendText) as server:
        server.serve_forever()

def sendText():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT))
        s.listen(1)
        conn, addr = s.accept()
        with conn:
            print("Connection: ", addr)
            while True:
                data = input("Enter input:")
                conn.send(bytes(data.encode()))

def sendSingleImage():
    f = open("oled_ali.png.bin", "rb")
    data = f.read()
    f.close()
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT))
        s.listen(1)
        conn, addr = s.accept()
        with conn:
            print("Connection: ", addr)
            conn.send(data)

def main():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT))
        s.listen(1)
        conn, addr = s.accept()
        with conn:
            print("Connection: ", addr)
            frame = 1
            numFrames = 120
            goingUp = True
            while(True):
                f = open(f"./video_test/oled_test{frame}.jpg.bin", "rb")
                data = f.read()
                f.close()
                conn.send(data)
                time.sleep(0.25)
                frame = frame + (1 if goingUp else -1)
                if (frame > numFrames):
                    frame = numFrames
                    goingUp = False
                elif (frame < 1):
                    frame = 1
                    goingUp = True

if __name__ == "__main__":
    main()
