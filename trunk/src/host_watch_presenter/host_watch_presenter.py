from socket import *
import struct
import time
import graph_presenter as graph
import angle_presenter_3d as angle3d
import sys

class SockDataGen(object):
    def __init__(self, sock, n):
        self.sock = sock
        self.n = n
    
    def __call__(self):
        client.send("GIVE")
        data = self.sock.recv(4*3)
        unpacked =  struct.unpack("fff", data)[self.n]
        print unpacked
        return unpacked

    
class SockDataGen3d(object):
    def __init__(self, sock):
        self.sock = sock
    
    def __call__(self):
        client.send("GIVE")
        data = self.sock.recv(4*3)
        unpacked =  struct.unpack("fff", data)
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
        (watch_name, minval, maxval, stam) = data.split(";")
        print watch_name, "connected!"
        #session = graph.session(SockDataGen(client, 0), float(minval), float(maxval))
        session = angle3d.session(SockDataGen3d(client))
        session.start()

    except Exception  as e:
        raise e;
    finally:
        client.close()
        server_sock.close()

