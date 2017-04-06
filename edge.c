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

int lengthOfInputFile = 4;

char resultBuffer[100];
char *tempPointer;
char resultArray[100][40];

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
	//printf("insert = %s\n", str);
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

void sendDataToClient() {
	int j=0;
	printf("Here's the data client---\n");
	
	for(j=0; j<lengthOfInputFile; j++) {
		printf("%s\n", resultArray[j]);
	}
}

char *getResultInString(char tmp[]) {
	char *resultToken;
	char *result;
	char *rest = tmp;
	int count = 0;

	while ((resultToken = strtok_r(rest, ",", &rest))) {
		if (count == 0) {
			result = resultToken;	
		}
		count++; 			
	}
	return result;
}

int insertedLength = 0;
// this will ensure results are entered in the correct order
void insertIntoArrayAtSpecificIndex(int index, char *resultString) {
	insertedLength += 1;
	strcpy(resultArray[index], resultString);
	
	//call function to send all results to client socket
	if (insertedLength == 4) {
		printf("%s\n", resultArray[0]);
		printf("%s\n", resultArray[1]);
		printf("%s\n", resultArray[2]);
		printf("%s\n", resultArray[3]);
	}
}

char *token;
// function to segment the string to get the index
int getIndexInString(char temp[]) {
	int index = 0;
	char *rest = temp;
	int count = 0;

	while ((token = strtok_r(rest, ",", &rest))) {
		if (count == 1) {
			index = atoi(token);
		}
		count++; 			
	}
	return index;
}

// function to add string to the queue
void addToResultArray(char temp[]) {
	int index = getIndexInString(temp);
	char *slicedString = getResultInString(temp);
	insertIntoArrayAtSpecificIndex(index, slicedString);
	//insertIntoArrayAtSpecificIndex(getIndexInString(temp), getResultInString(temp));
}

void insertBufferHelper(char resultBuffer[]) {
	int count = 0;
	char tempNumBuffer[40];
	bzero(tempNumBuffer, 40);
	int tempNumBufferCount = 0;
	
	for (count = 0; count < strlen(resultBuffer); count++) {
        if (resultBuffer[count] == 'a') {
            int m;
            // run for 27 times
            for (m=count; m<(count+27); m++) {
                tempNumBuffer[tempNumBufferCount] = resultBuffer[m];
                tempNumBufferCount++;
            }
			insert(tempNumBuffer);
            bzero(tempNumBuffer, 40);
            tempNumBufferCount = 0;    
        }
        if (resultBuffer[count] == 'o') {
            int n;
            // run for 26 times
            for (n=count; n<(count+26); n++) {
                tempNumBuffer[tempNumBufferCount] = resultBuffer[n];
                tempNumBufferCount++;
            }
			insert(tempNumBuffer);
            bzero(tempNumBuffer, 40);
            tempNumBufferCount = 0;    
        }
	}
}

void sendBufferToServers() {

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
			addToResultArray(resultBuffer);
			bzero(resultBuffer, 40);
		}
		sentOr = recvfrom(orSock, resultBuffer, 40, 0, (struct sockaddr *)&orRemoteServer, &addrLen);
		if (sentOr < 0) {
			perror("Failed to receive from or server");
			exit(-1);
		} else {
			printf("Got an ack from or server %s\n", resultBuffer);
			addToResultArray(resultBuffer);
			bzero(resultBuffer, 40);
		}
	}
}

int main() {

	int sock, clientSockDesc, sent, received;
	struct sockaddr_in serverAddress, clientAddress;
	unsigned int addrLen;
	char buffer[4000];
	bzero(buffer, 4000);

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

		//printf("new client connected with ip = %s and port = %d\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));

		int data_len = 1;

		while(data_len) {
			data_len = recv(clientSockDesc, buffer, 4000, 0);
			if (data_len) {
				//buffer[][data_len] = '\0';
				insertBufferHelper(buffer);
				sendBufferToServers();
				//printf("%s", buffer);
				//printf("\n");
			}
		}

		//sent = send(clientSockDesc, message, strlen(message), 0);
		//printf("Sent %d bytes to client %s\n", sent, inet_ntoa(clientAddress.sin_addr));
		//printf("Client disconnected!!");
	}
}

