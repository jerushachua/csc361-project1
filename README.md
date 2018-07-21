CSC361 - Fall 2017 <br />
Mantis Cheng <br />
Project 1 <br />

ufc.c - ./udp_client <filename> <br />
ufs.c - ./udp_server <directory> <br /> 

This is an exercise with file transfer through UDP sockets. 
* The server listens on UDP port 8080. 
* The client will request a file, sending the filename to the server. 
* The server checks in the given directory if the requested file exists. If it does, send the file in packets of size up to 1024 bytes to the client. 
