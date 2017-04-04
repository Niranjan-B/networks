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

char queue[100][40];
int front = 0;
int rear = -1;
int itemCount = 0;

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


void main() {
	int sock, clientSockDesc, sent, received;
	struct sockaddr_in serverAddress, clientAddress;
	unsigned int addrLen;
	char buffer[40];

	// asking kernel for a socket
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket creation error: ");
		exit(-1);
	}

	// init adddress
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(23355);
	inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);
	bzero(&serverAddress.sin_zero, 8);

	addrLen = sizeof(struct sockaddr_in);

	// bind socket with the address
	if((bind(sock, (struct sockaddr *)&serverAddress, addrLen)) == -1) {
		perror("socket binding error: ");
		exit(-1);
	}

	// listen for incoming connections on that socket
	if((listen(sock, 10)) == -1) {
		perror("failed to listen");
		exit(-1);
	}

	// keep listening for incoming requests and accept one when it comes
	while(1) {
		if((clientSockDesc = accept(sock, (struct sockaddr*)&clientAddress, &addrLen )) == -1) {
			perror("failed to accept incoming client request");
			exit(-1);
		}

		// clearing 2D array
		clearQueue();

		//printf("new client connected with ip = %s and port = %d", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));

		int data_len = 1;

		while(data_len) {
			data_len = recv(clientSockDesc, buffer, 39, 0);
			if (data_len) {
				buffer[data_len] = '\0';
				insert(buffer);
				printf("%s", buffer);
			}
		}

		//sent = send(clientSockDesc, message, strlen(message), 0);
		//printf("Sent %d bytes to client %s\n", sent, inet_ntoa(clientAddress.sin_addr));
		printf("Client disconnected!!");
		close(clientSockDesc);
	}
}

