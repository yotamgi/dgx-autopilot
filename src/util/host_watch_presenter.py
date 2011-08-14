from socket import *
import struct
import time


server_sock = socket(AF_INET, SOCK_STREAM)

server_sock.bind(("localhost", 0x6666))
server_sock.listen(1)


client, client_addr  = server_sock.accept()
print client_addr, "Connected!"

while True:
	client.send("GIVE")
	data = client.recv(4*3)
	print len(data)
	print struct.unpack("fff", data)
	time.sleep(1.0/10)

