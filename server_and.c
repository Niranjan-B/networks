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

int main() {
	int sock, length, n;
	struct sockaddr_in serverAddress, clientAddress;
	char buf[40];
	bzero(buf, 40);

	bool printCompFlag = true;
	
	int numOfAndOperations = 0;
	bool isFirstTransmision = true;
	char sizeOfAndOperationsBuffer[10];
	int numOfAndOperationsFromBuffer = 0;
	bzero(sizeOfAndOperationsBuffer, 10);

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0) {
		perror("Socket");
		exit(-1);	
	}
	
	length = sizeof(serverAddress);
	bzero(&serverAddress, length);
	
	// init adddress
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(22355);
	inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);
	bzero(&serverAddress.sin_zero, 8);

	if(bind(sock, (struct sockaddr *)&serverAddress, length) < 0) {
		perror("Binding");
		exit(-1);	
	}

	printf("The Server AND is up and running using UDP on port 22355\n");

	unsigned int fromLength = sizeof(clientAddress);

	while(true) {
		bzero(buf, 40);
		n = recvfrom(sock, buf, 40, 0, (struct sockaddr *)&clientAddress, &fromLength);
		if(n < 0) {
			//perror("Received from");	
		} else {
			// splitting string based on , here

			if (printCompFlag) {
				printf("The server AND started receiving lines from the edge server for AND computation. The computation results are:\n");
				printCompFlag = false;
			}

			int j=0;
			char buffer[40]; 
		 	bzero(buffer, 40);
			strncpy(buffer, buf, strlen(buf));
			buffer[strlen(buf)] = '\0';

			// checking if it's the first reception, so as to get the count
			if (isFirstTransmision) {
				strncpy(sizeOfAndOperationsBuffer, buffer, strlen(buffer));
				numOfAndOperationsFromBuffer = atoi(sizeOfAndOperationsBuffer);
				isFirstTransmision = false;
			} else {
				numOfAndOperations += 1;

				char *token;
    	 		char *rest = buffer;
		 		char *result;  
 
		 		int count = 0;
		 		char *numOne;
		 		char *numTwo;
				int indexN;
			
    			while ((token = strtok_r(rest, ",", &rest))) {
					if (count == 1) {
						char *numOneTemp = token;
						numOne = numOneTemp;
						printf("%s and ",token);
					} else if (count == 2) {
						char *numTwoTemp = token;
						numTwo = numTwoTemp;
						printf("%s = ", token);
					} else if (count == 3){
						indexN = atoi(token);
					}
					count++;     			
				}

				char tempBuffer[41];
				bzero(tempBuffer, 41);
			
				int i=0;
				while(i < 10) {
					tempBuffer[i] = (((*numOne++)-'0') & ((*numTwo++)-'0')) + '0';
					i++;
				}

			
				int firstDigit = indexN / 10;
				int lastDigit = indexN % 10;
				char c1 = '0' + firstDigit;
				char c2 = '0' + lastDigit;

				puts(tempBuffer);

				tempBuffer[10] = ',';
				tempBuffer[11] = c1;
				tempBuffer[12] = c2;
				tempBuffer[13] = '\0';

				//puts(tempBuffer);

				sendto(sock, tempBuffer, strlen(tempBuffer), 0, (struct sockaddr *)&clientAddress, fromLength);

				// print all sent if the numOfAndOperations = sizeOfAndOperations
				if (numOfAndOperations == numOfAndOperationsFromBuffer) {
					printf("The Server AND has successfully received %d lines from the edge server and finished all AND computations.\n",numOfAndOperationsFromBuffer);
					printf("The Server AND has successfully finished sending all computation results to the edge server.\n");
				}
			}
		}				
	} 
}

	

