#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MAX_CLIENTS 2
int count=0;
int globalSocket =0;
int CLIENTS[MAX_CLIENTS];
// may need mutex
void* clientHandler(void* client_socket);

int main(int argc, char *argv[])
{
	if((argc < 2)||(argc >= 3))
	{
	 printf("Error, incorrect arguments, usage: ./server <svr_port>\n");
	 exit(0);
	}

	int server_socket;
	int client_socket;
	int client_addr_size;
	int error;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	int portnum;
	int i;
	pthread_attr_t attr; //used to create detached thread

	//detach the thread
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	//getting the server set up
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(server_socket < 0)
	{
		perror("Server Socket: ");
		exit(0);
	}

	int key=1;
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &key, sizeof(key));

	bzero((char*) &server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	portnum = atoi(argv[1]);
	server_addr.sin_port = htons(portnum);
	
	for(i =0; i < MAX_CLIENTS; i++)
		CLIENTS[i] = -1; //initilize the clients array

	if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0 )
	{
		perror("bind");
		exit(0);
	}

	printf("Waiting for Incoming Connections...\n");

	error = listen(server_socket, 2);
	if(error < 0)
	{
		perror("listening");
		exit(0);
	}
//printf("test1\n");
	while(1)
	{
		//get a new connection
		client_addr_size = sizeof(client_addr);
		client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size);
		if(client_socket > 0)
		{
			count++;
		//	printf("count = %d\n", count);
		}
//printf("test2\n");
//printf("count = %d\n", count);
		else if(client_socket < 0)
		{
			perror("client socket"); //error accepting client
			close(client_socket);
			continue;
		}
		else if( count > MAX_CLIENTS) //if there already are 2 clients
		{
			printf("Max amount of clients has been reached\n");
			write(client_socket, "Rejected", strlen("Rejected")); //reject the new connection
			close(client_socket);
			continue;
		}
		//printf("test3\n");
		printf("Client Connection Accepted\n");
		write(client_socket, "Sucess", strlen("Sucess"));
		
		// check which position on the array the client will go in
		for(i =0; CLIENTS[i] != -1; i++);
			CLIENTS[i] = client_socket;
		globalSocket = client_socket;
		//printf("client socket main= %d\n", globalSocket);
		//create handling thread
		pthread_t tID = i + 1; //threads ID
		pthread_create(&tID, &attr, clientHandler,(void *)&client_socket);
	}

	//close(client_socket);
	return 0;
}
/* thread used handle communication with a single client */
void* clientHandler(void* socket)//client_socket was socket
{
	char buffer[256];
	int client_socket = (int)socket;
	int end;
	int num;
	int i;
	int total = 0;
	char IPstring[INET_ADDRSTRLEN];

	//printf("client socket = %d\n", globalSocket);
	//printf("int socket = %d\n", (int)socket);
	while(1)
	{
		//printf("test4\n");
		//read the number from the client
		memset(buffer, 0, 256);
		//printf("test5\n");
		end = read(globalSocket, buffer, 255);//was 255
		//printf("test6\n");
		buffer[end]=0;
		//printf("buffer= %s\n", buffer);
		
		//make the string into a number and increase the total
		num = atoi(buffer);
		total = total + num;
		//num++;
	//	printf("Total = %d\n", total);
		if(num == 0) //if the user quits
		{
			printf("Disconnecting...\n");
			count--;
			break;
		}
		else if(total >= 1024 && count == 1) //if one client connects and passes the threshold then reset total
		{
			total = 0;
		}
		else if( total >= 1024) // if the user enters a well known portnum
		{
			//get the ip address so that it can be sent to the other user
			struct sockaddr_in client;
			socklen_t length = sizeof(client);
			getpeername(globalSocket, (struct sockaddr*)&client, &length);
			inet_ntop(AF_INET, &client.sin_addr, IPstring, INET_ADDRSTRLEN);
			//printf("address: %s\n", IPstring); //test output

			for(i=0; i <MAX_CLIENTS; i++) //send message to all connected users
			{
				if(CLIENTS[i] != globalSocket) //except this client
				{
					write(CLIENTS[i], "PORT", strlen("PORT")); //write the portnum

					//send the port num
					memset(buffer, 0, 256);
					sprintf(buffer, "%d", total);
					usleep(1000);
					write(CLIENTS[i], buffer, strlen(buffer)); // write the port

					//send the ip
					usleep(1000);
					write(CLIENTS[i], IPstring, strlen(IPstring)); //write the ip
				}
			}

			//send the total back to the client that reached the threshold
			memset(buffer, 0, 256);
			sprintf(buffer, "%d", total);
			write(globalSocket, buffer, strlen(buffer));
			usleep(1000);
			write(globalSocket, "PORT", strlen("PORT"));
		}
		else //if the user ented some number
		{
			//send the total back
			printf("sending total %d\n", total);
			memset(buffer, 0, 256);
			sprintf(buffer, "%d", total);

			write(globalSocket, buffer, strlen(buffer));
		}

	}
	close(globalSocket);
	pthread_exit(NULL);
}
