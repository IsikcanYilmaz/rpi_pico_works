#!/usr/bin/env python3

import socket
import argparse
import sys
import time

HOST = "192.168.50.222"
PORT = 9999

# https://stackoverflow.com/questions/54353238/how-can-i-create-a-tcp-server-using-python-3
def main():
    # Create a socket (SOCK_STREAM means a TCP socket)
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        # Connect to server and send data
        sock.connect((HOST, PORT))
        while(True):
            received = str(sock.recv(1024), "utf-8")
            if not received: 
                break
            print("RECV:", received)
            time.sleep(1)

if __name__ == "__main__":
    main()
