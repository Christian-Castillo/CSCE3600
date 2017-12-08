#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

int RECONNECT = 0; //used if the other client is forcing the user to reconnect
int EXIT = 0; //used to determine if the user voluntarily quit
int CLISRV_IP[12]; // the port sent by the other client
int CLISVR_PORT =0;
int TOTAL = 0; // the user total sent by the server

void* listeningThread(void* socket);
void* inputThread(void* socket);
void* clientServer(void* i);

int main(int argc, char* argv[])
{
	int client_socket; //the clients socket
	struct sockaddr_in client_addr; //clients address information
	//int portnum;
	int error; //used to check for errors
	char buffer[256];
	int end; // used to mark end from read function
	pthread_t threadID;
	pthread_t inputThreadID;
	int done =0; //looping variable
	int num; // used to hold user input
	//printf("test1\n");
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	//printf("test2\n");
	if((argc < 4)||(argc >= 5))
	{
		printf("ERROR: usage: ./client <svr_host> <svr_port> <rem_ipaddr>");
		exit(0);
	}
	
	//printf("test3\n");
	//set up client stuff
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(client_socket < 0)
	{
		perror("client socket");
		exit(0);
	}
	
	//printf("test4\n");

	char *hostname = argv[1]; //hostname...ins't this A21?
	//printf("test5\n");
	bzero((char*)&client_addr, sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_addr.s_addr = inet_addr(argv[3]);//ip
	int portnum = atoi(argv[2]);
	client_addr.sin_port = htons(portnum);//port
	//printf("test6\n");
	if(gethostname(hostname, sizeof(hostname)) < 0)
	{
		herror("gethostbyhostname"); //not sure this will work. Saw it on stack overflow. JK included in the <netdb.h> library
		exit(0);
	}
	//printf("test7\n");
	error = connect(client_socket, (struct sockaddr*)&client_addr, sizeof(client_addr));
	if(error < 0)
	{
		perror("connect");
		exit(0);
	}
	//printf("test8\n");
	//wait fot a message from the server to determine if the client was accepted
	memset(buffer, 0, 256);
	end = read(client_socket, buffer, 255);
	buffer[end] =0;
	//printf("test9\n");
	//printf("buffer = %s\n", buffer);
	
	if(strcmp(buffer, "Rejected") == 0)
	{
		printf("The server cannot handle more connections");
		exit(0);
	}
	//printf("test10\n");
	//launch listening thread
	pthread_create(&threadID, &attr, listeningThread, (void*)client_socket);
	//launch input thread
	pthread_create(&inputThreadID, &attr, inputThread, (void*)client_socket);
	//printf("test11\n");
	// wait for the user to quit or for the other client to become the server
	while(!EXIT && !RECONNECT)
	{

	}
	//printf("test12\n");
	close(client_socket);
	usleep(300000); //give the other user a chance to setup the server if neccessary
	//printf("test13\n");
	if(EXIT ==1)
		printf("close and exit");// if the user voluntarily quits, close the program
	else if(RECONNECT == 1)// if another user became the server
	{
	//	printf("test14\n");
		// set up the connection again
		client_socket = socket(AF_INET, SOCK_STREAM,0);
		if(client_socket <0)
		{
			perror("socket");
			exit(0);
		}

		//printf("if reconnect else if\n");
		bzero((char*)&client_addr, sizeof(client_addr));
		client_addr.sin_family = AF_INET;
		client_addr.sin_addr.s_addr = inet_addr(CLISRV_IP);//ip//this is why it can only get the same ip
		//CLISRV_IP = atoi(argv[2]);//this gets the given portnum in the comm line?
		//portnum = atoi(CLISVR_PORT);
		portnum = CLISVR_PORT;
		client_addr.sin_port = htons(portnum);//port was portnum
		char *hostname = argv[1]; //hostname
		//printf("test15\n");
		if(gethostname(hostname, sizeof(hostname)) < 0)
		{
			herror("gethostbyhostname"); //not sure this will work. Saw it on stack overflow. JK included in the <netdb.h> library
			exit(0);
		}
		//printf("test16\n");
		error = connect(client_socket, (struct sockaddr*)&client_addr, sizeof(client_addr));
		if(error < 0)
		{
			perror("connect");
			exit(0);
		}
		//printf("test17\n");
		//write the total to the client server
		printf("TOTAL before closing %d\n", TOTAL);
		memset(buffer, 0, 256);
		sprintf(buffer, "%d", TOTAL);
		write(client_socket, buffer, strlen(buffer));
		//printf("test18\n");
		close(client_socket);
	}


	return 0;
}
void* listeningThread(void* socket)
{
	int client_socket = (int)socket; // unpack socket descriptor
	char buffer[256];
	int end;
	int readNum;
	int portnum;

	//thread variables incase the slientServer is launched
	pthread_t ID;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	while(1)
	{
		// read from server
		memset(buffer, 0, 256);
		//printf("listen1\n");
		end = read(client_socket, buffer, 255);
		//printf("listen2\n");
		buffer[end] = 0;
		//printf("listenBuffer = %s\n", buffer);

		if(strcmp(buffer, "PORT") == 0) // if the client needs to reconnect
		{
		//	printf("\n%s\n", buffer);
			memset(buffer, 0, 256);
			end = read(client_socket, buffer, 255); // read the port
			buffer[end] = 0;
			//printf("port: %s\n", buffer); // test print
			CLISVR_PORT = atoi(buffer);

			memset(buffer, 0, 256);
			end = read(client_socket, buffer, 255); // read the address
			buffer[end] = 0;
			//printf("address: %s\n", buffer); // test print
			strcpy(CLISRV_IP, buffer); // copy the address into the global variable

			RECONNECT = 1; // set reconnect to 1

			break; // end the loop
		}
		else // received the total from the sever
		{
			readNum = atoi(buffer);
			TOTAL = readNum;
			if(TOTAL == 0) //if the user wants to quit the program
			{
				//printf("total = %d\n", TOTAL);
				printf("Disconnecting...\n");
				exit(0);
			}
			else if(readNum >= 1024) // if the client is going to become a server
			{
				printf("\ntotal: %d\n", readNum); // display the total

				// read the port message
				end = read(client_socket, buffer, 255);
				buffer[end] = 0;
			//	printf("%s\n", buffer);

				// launch the clientServer thread for the other user
				pthread_create(&ID, &attr, clientServer, (void*)ID);
			}
			else // print the total
			{
				printf("total: %d\n", TOTAL);
			}
		}
	}

	pthread_exit(NULL);
}
void* inputThread(void* socket)
{
	int client_socket = (int)socket;
	int done = 0;
	char buffer[256];
	int num;

	while(!done)
	{
		// get a number from the user
		memset(buffer, 0, 256);
		printf("Enter number: ");
		scanf("%d", &num);

		if(num == 0) // if the user is quitting
		{
			sprintf(buffer, "%d", num);
			printf("sending: %s\n", buffer); // test print to see the buffer sent
			write(client_socket, buffer, strlen(buffer)); //we want the server to know that he's quitting
			done = 1; // should send a quit message to server probably
			break;
		}
		else // the user is sending a number
		{
			sprintf(buffer, "%d", num);
			printf("sending: %s\n", buffer); // test print to see the buffer sent

			// if the other user has not become the server
			if(!RECONNECT) write(client_socket, buffer, strlen(buffer)); // write the number
			else // a client will become a new server
			{
				printf("gonna reconnect to the new server\n"); // test
				break;
			}
		}

		usleep(10000); // allow server to write back the total before asking for a number
	}

	pthread_exit(NULL);
}
/* this thread creates a new server for the second client to connect to */
void* clientServer(void* i)
{
	int clientServer;
	int secondClient;

	int secondCli_addr_size;
	int error;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	int end;
	char buffer[256];
	memset(buffer, 0, 256);//initialize buffer
	// set up server
	clientServer = socket(AF_INET, SOCK_STREAM, 0);
	if(clientServer < 0)
	{
		perror("socket");
		exit(0);
	}

	bzero((char*)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);//was just INADDDR_ANY
	server_addr.sin_port = htons(TOTAL);

	if(bind(clientServer, (struct sockaddr*)&server_addr, sizeof(server_addr))<0)
	{
		perror("binding");
		exit(0);
	}

	error = listen(clientServer, 2);//why 5?(was 5)
	if(error < 0)
	{
		perror("listening");
		exit(0);
	}

	// connect the other client
	secondCli_addr_size = sizeof(client_addr);

	secondClient = accept(clientServer, (struct sockaddr*)&client_addr, &secondCli_addr_size);//code seems sto stop here

	if(secondClient < 0)//accept error check
	{
		perror("accept");
		exit(0);
	}

	// read the other client's total
	end = read(secondClient, buffer, 255);
	buffer[end] = 0;

	printf("\nThe other client's total: %s\n", buffer);

	close(clientServer);
	close(secondClient);

	pthread_exit(NULL);
}
