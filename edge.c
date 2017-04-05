#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<errno.h>
#include<string.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdbool.h>
#include <netdb.h>

char queue[100][40];
int front = 0;
int rear = -1;
int itemCount = 0;
int sent = 0;
int sentOr = 0;

char resultBuffer[100];
char *tempPointer;

// functions for queue operations
const char* peek() {
	return queue[front];
}
bool isEmpty() {
	return itemCount == 0;
}
bool isFull() {
	return itemCount == 100;
}
int size() {
	return itemCount;
}
void insert(char* str) {
	if(!isFull()) {
		if (rear == 99) {
			rear = -1;
		}
		strcpy(queue[++rear], str);
		itemCount++;
	}
}
const char* removeData() {
	char *data = queue[front++];

	if (front == 100) {
		front = 0;
	}
	itemCount--;
	return data;
}
void clearQueue() {
	for(int i=0; i<100; i++){
		for (int j=0; j<40; j++) {
			queue[i][j]=0;
		}
	}
	front = 0;
	rear = -1;
	itemCount = 0;
}


int main() {

	insert("and,0000010111,0000010100,44\0");
	insert("or,0000000010,0000001011,33\0");
	insert("or,0000000011,0000010001,22\0");
	insert("and,0000001001,0000000111,11\0");

	struct sockaddr_in andRemoteServer, orRemoteServer;
	int andSock, orSock;
	char resultBuffer[40];
	unsigned int addrLen;

	// creating and/or server socket descriptor
	andSock = socket(AF_INET, SOCK_DGRAM, 0);
	if(andSock < 0) {
		perror("Error creating and server socket");
		exit(-1);
	}
	orSock = socket(AF_INET, SOCK_DGRAM, 0);
	if(orSock < 0) {
		perror("Error creating or server socket");
		exit(-1);
	}

	// init and server addr
	andRemoteServer.sin_family = AF_INET;
	andRemoteServer.sin_port = htons(22355);
	inet_pton(AF_INET, "127.0.0.1", &andRemoteServer.sin_addr);
	addrLen = sizeof(struct sockaddr_in);
	// init or server addr
	orRemoteServer.sin_family = AF_INET;
	orRemoteServer.sin_port = htons(21355);
	inet_pton(AF_INET, "127.0.0.1", &orRemoteServer.sin_addr);

	bzero(resultBuffer, 40);

	printf("starting to send packets to respective servers\n");
	bool flag = true;

	while(1) {
		if(flag){
			int i=0;
			int tempSize = size();
			int sent = 0;
			int sentOr = 0;

			for (i=0; i<tempSize; i++) {
		 		tempPointer = removeData();
		 		if (*tempPointer == 'a') {
		 			// send data to and server here
					sent = sendto(andSock, tempPointer, strlen(tempPointer), 0, (struct sockaddr *)&andRemoteServer, addrLen);
					if (sent < 0) {
						perror("Send to and failed");
						exit(-1);
					} else {
						printf("Sent to and server\n");
					}
					sent = 0;
		 		} else {
		 			// send data to or server here
					sentOr = sendto(orSock, tempPointer, strlen(tempPointer), 0, (struct sockaddr *)&orRemoteServer, addrLen);
						if (sentOr < 0) {
							perror("Send to or failed");
							exit(-1);
						} else {
							printf("Sent to or server\n");
						}
					sentOr = 0; 
		 		}
			}
			flag = false;
		}
		// listen for response here
		sent = recvfrom(andSock, resultBuffer, 40, 0, (struct sockaddr *)&andRemoteServer, &addrLen);
		if (sent < 0) {
			perror("Failed to receive from and server");
			exit(-1);
		} else {
			printf("Got an ack from and server %s\n", resultBuffer);
		}
		sentOr = recvfrom(orSock, resultBuffer, 100, 0, (struct sockaddr *)&orRemoteServer, &addrLen);
		if (sentOr < 0) {
			perror("Failed to receive from or server");
			exit(-1);
		} else {
			printf("Got an ack from or server %s\n", resultBuffer);
		}

	}


	// int sock, clientSockDesc, sent, received;
	// struct sockaddr_in serverAddress, clientAddress;
	// unsigned int addrLen;
	// char buffer[40];

	// // asking kernel for a socket
	// if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	// 	perror("socket creation error: ");
	// 	exit(-1);
	// }

	// // init adddress
	// serverAddress.sin_family = AF_INET;
	// serverAddress.sin_port = htons(23355);
	// inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);
	// bzero(&serverAddress.sin_zero, 8);

	// addrLen = sizeof(struct sockaddr_in);

	// // bind socket with the address
	// if((bind(sock, (struct sockaddr *)&serverAddress, addrLen)) == -1) {
	// 	perror("socket binding error: ");
	// 	exit(-1);
	// }

	// // listen for incoming connections on that socket
	// if((listen(sock, 10)) == -1) {
	// 	perror("failed to listen");
	// 	exit(-1);
	// }

	// // keep listening for incoming requests and accept one when it comes
	// while(1) {
	// 	if((clientSockDesc = accept(sock, (struct sockaddr*)&clientAddress, &addrLen )) == -1) {
	// 		perror("failed to accept incoming client request");
	// 		exit(-1);
	// 	}

	// 	// clearing 2D array
	// 	clearQueue();

	// 	//printf("new client connected with ip = %s and port = %d", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));

	// 	int data_len = 1;

	// 	while(data_len) {
	// 		data_len = recv(clientSockDesc, buffer, 39, 0);
	// 		if (data_len) {
	// 			buffer[data_len] = '\0';
	// 			insert(buffer);
	// 			printf("%s", buffer);
	// 		}
	// 	}

	// 	//sent = send(clientSockDesc, message, strlen(message), 0);
	// 	//printf("Sent %d bytes to client %s\n", sent, inet_ntoa(clientAddress.sin_addr));
	// 	printf("Client disconnected!!");
	// 	close(clientSockDesc);
	// }
}

