from socket import *
import struct
import time
import graph_presenter as graph
import angle_presenter_3d as angle3d
import sys

class SockDataGen(object):
    def __init__(self, sock, channels, n = -1):
        self.sock = sock
        self.channels = channels
        self.n = n
    
    def __call__(self):
        client.send("GIVE")
        data = self.sock.recv(4*3)
        if self.n == -1:
            unpacked =  struct.unpack("f"*self.channels, data)
        else:
            unpacked =  struct.unpack("f"*self.channels, data)[self.n]
        print unpacked
        return unpacked

if len(sys.argv) != 2:
    print "usage:", sys.argv[0], "<bind_addr>"
    sys.exit(1)

server_sock = socket(AF_INET, SOCK_STREAM)

server_sock.bind((sys.argv[1], 0x6666))
server_sock.listen(1)


while (True):
    print "Waiting for connections..."
    client, client_addr  = server_sock.accept()
    try:
        print client_addr, "Connected!"

        data = client.recv(100)
        (watch_name, minval, maxval, channels, stam) = data.split(";")
        print watch_name, "connected with", channels, "channels"
        channels = int(channels)
        
        if channels == 1:
            session = graph.session(SockDataGen(client, channels), float(minval), float(maxval))
        else:
            #session = graph.session(SockDataGen(client, channels, 2), float(minval), float(maxval))
            session = angle3d.session(SockDataGen(client, channels))
        session.start()

    #except Exception  as e:
    #    raise e;
    finally:
        print "Closing connections"
        client.close()
        server_sock.close()

