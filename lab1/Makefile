all : Client.exe Server.exe
Server.exe: Server.cpp
	g++ .\Server.cpp -o .\Server.exe -lwsock32
Client.exe: Client.cpp
	g++ .\Client.cpp -o .\Client.exe -lwsock32
.PHONY: clean
clean : 
	del *.exe