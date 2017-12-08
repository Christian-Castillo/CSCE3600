Names: Chalet Shelton, Kevin Hinson, Chelsea Greer, Tara Boyle

Organization: Chalet did most of the client server set up with Kevin contributing to that. Chelsea and Tara
		focused on fixing bugs. 

Design:
	Server: In main we structure the setting up of the server. We error check to make sure that there are 2 command line arguments for the server and
		its port number, then establish appropriate variables for sockets and sock_addr_in for server and clients. The server is set up using the
		INET family with TCP for more realiable connection, with port number set to command line argv[1]. Then we have array for setting up
		binding and listening for both clients. Once both are setup, connections are established and can begin the client handler.
		
	Client:
		The client will connect as normally in the main function.  It uses the functions listenthread,inputthread, and clientServer.
		listenthread gets client input and adds it to the total and lets clients to reconnect if need be.  When total >= 1024
		it creates the clientServer so it will act as a server.
		Inputthread gets input from user to send to the server.


Complete Specification:

Known Bugs:
	When connecting a client it won't accept any ip address that is not its own. This causes issues with the client becoming the server. Also there
	are several warnings when compiling but no compilation errors. You also have to pass the same port for both machines (IP ADDRESS NEEDS TO BE THE
	ADDRESS OF THE SERVER).

*To compile, simply type "make" (without the quotes), so long as the Makefile is in the directory as well as the client and server files. If there is no
 server, just the client file type "make client" (without quotes).*
