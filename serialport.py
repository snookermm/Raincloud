#!/bin/python3
import socket,sys
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.bind(('localhost', int(sys.argv[1])))
sock.listen(10)
sock,addr = sock.accept()
#sock.connect(("127.0.0.1", int(sys.argv[1])))
sock.settimeout(6000)
while True:
	sys.stdout.buffer.write(sock.recv(1))
	sys.stdout.flush()
	#print("%c" % sock.recv(1).decode(), end='')
	
	
