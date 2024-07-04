#import socket module
from socket import *
from _thread import *
import sys # In order to terminate the program


def server_thread(connection):
    try:
        message = connection.recv(1024)
        filename = message.split()[1]
        print("filename: "+str(filename[1:]))
        f = open(filename[1:], "r")
        outputdata = f.read()
        print(outputdata)
        #Send one HTTP header line into socket
        connection.send("HTTP/1.1 200 OK\r\n\r\n".encode("utf-8"))
        
        #Send the content of the requested file to the client
        for i in range(0, len(outputdata)):
            connection.send(outputdata[i].encode("utf-8"))
        connection.send("\n\n".encode("utf-8"))

        connection.close()
    except IOError:
        print("IO ERROR")
        #Send response message for file not found
        connection.send("HTTP/1.1 200 OK\r\n".encode())
        outdata = '<html><body><center><h3>Error 404: File not found</h3><p>Python HTTP Server</p></center></body></html>\n\n'
        connection.send("\r\n".encode("utf-8"))
        connection.send(outdata.encode("utf-8"))
        #Close client socket
        connection.close()



serverSocket = socket(AF_INET, SOCK_STREAM)

threadCount = 0

#Prepare a sever socket
serverSocket.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
address_port = ("127.0.0.1", 4080)
serverSocket.bind(address_port)
serverSocket.listen(8)

while True:
    client, addr = serverSocket.accept()
    start_new_thread(server_thread,(client,),)
    threadCount += 1
serverSocket.close()

sys.exit()#Terminate the program after sending the corresponding data


