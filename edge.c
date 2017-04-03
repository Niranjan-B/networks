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

char queue[100][40]; // toa add to the queue, use strcpy(queue[0], 'blah');
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



void main() {
	int sock, clientSockDesc, sent, received;
	struct sockaddr_in serverAddress, clientAddress;
	char message[] = "Got your message";
	unsigned int addrLen;
	char buffer[2048];

	if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket creation error: ");
		exit(-1);
	}

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(23335);
	inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);
	bzero(&serverAddress.sin_zero, 8);

	addrLen = sizeof(struct sockaddr_in);

	if((bind(sock, (struct sockaddr *)&serverAddress, addrLen)) == -1) {
		perror("socket binding error: ");
		exit(-1);
	}

	if((listen(sock, 10)) == -1) {
		perror("failed to listen");
		exit(-1);
	}

	while(1) {
		if((clientSockDesc = accept(sock, (struct sockaddr*)&clientAddress, &addrLen )) == -1) {
			perror("failed to accept incoming client request");
			exit(-1);
		}

		bzero(buffer, 2048);

		//printf("new client connected with ip = %s and port = %d", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));

		int data_len = 1;

		while(data_len) {
			data_len = recv(clientSockDesc, buffer, 2047, 0);
			if (data_len) {
				send(clientSockDesc, buffer, data_len, 0);
				buffer[data_len] = '\0';
				printf("Sent message = %s", buffer);
			}
		}

		//sent = send(clientSockDesc, message, strlen(message), 0);
		//printf("Sent %d bytes to client %s\n", sent, inet_ntoa(clientAddress.sin_addr));

		printf("Client disconnected!!");
		close(clientSockDesc);
	}
    close(sock);
}

