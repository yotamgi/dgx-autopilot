from socket import *
import struct
import time
import graph

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
		

server_sock = socket(AF_INET, SOCK_STREAM)

server_sock.bind(("localhost", 0x6666))
server_sock.listen(1)


while (True):
	client, client_addr  = server_sock.accept()
	print client_addr, "Connected!"

	session = graph.session(SockDataGen(client, 0))
	session.start()
