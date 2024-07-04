#import socket module
from socket import *
import sys # In order to terminate the program
serverSocket = socket(AF_INET, SOCK_STREAM)

#Prepare a sever socket
#Fill in start
serverSocket.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
address_port = ("127.0.0.1", 4080)
serverSocket.bind(address_port)
serverSocket.listen(1)
#Fill in end
while True:
 
    #Establish the connection
    print('Ready to serve...')
    connectionSocket, addr = serverSocket.accept()
    try:
        message = connectionSocket.recv(1024)
        filename = message.split()[1]
        print("filename: "+str(filename[1:]))
        f = open(filename[1:], "r")
        #outputdata = "".join(line for line in f)
        outputdata = f.read()
        print(outputdata)
        #Send one HTTP header line into socket
        #Fill in start
        connectionSocket.send("HTTP/1.1 200 OK\r\n\r\n".encode("utf-8"))
        #Fill in end
        
        #Send the content of the requested file to the client
        for i in range(0, len(outputdata)):
            connectionSocket.send(outputdata[i].encode("utf-8"))
        connectionSocket.send("\n\n".encode("utf-8"))

        connectionSocket.close()
    except IOError:
        print("IO ERROR")
        #Send response message for file not found
        #Fill in star
        connectionSocket.send("HTTP/1.1 200 OK\r\n".encode())
        outdata = '<html><body><center><h3>Error 404: File not found</h3><p>Python HTTP Server</p></center></body></html>\n\n'
        connectionSocket.send("\r\n".encode("utf-8"))
        connectionSocket.send(outdata.encode("utf-8"))
        #Fill in end
        #Close client socket
        #Fill in star
        connectionSocket.close()
        #Fill in end
serverSocket.close()

sys.exit()#Terminate the program after sending the corresponding data
