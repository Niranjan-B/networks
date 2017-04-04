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
	char buffer[1024];

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

	unsigned int fromLength = sizeof(clientAddress);

	while(true) {
		n = recvfrom(sock, buffer,1024,0, (struct sockaddr *)&clientAddress, &fromLength);
		if(n < 0) {
			//perror("Received from");	
		} else {

			// splitting string based on , here
		 	char *token;
    	 	char *rest = buffer;
		 	char *result;  
 
		 	int count = 0;
		 	int numOne = 0;
		 	int numTwo = 0;
		 	int indexN;
		 	int resultOfAnd = 0;
 
    		while ((token = strtok_r(rest, ",", &rest))) {
				if (count == 1) {
					//printf("%s\n", token);
					numOne = atoi(token);
					token = NULL;
				} else if (count == 2) {
					//printf("%s\n", token);
					numTwo = atoi(token);
					token = NULL;
				} else if (count == 3){
					indexN = atoi(token);
					//indexN = token;
					printf("%s\n", token);
					printf("%d\n", indexN);
				}
				count++;     			
			}
			printf("%d\n", indexN);
			bzero(buffer, 1024);
			
			char tempBuffer[41];
			bzero(tempBuffer, 41);
			
			resultOfAnd = numOne & numTwo;
			int tempResult = 0;

			//char indexBuffer[5];
			
			// converting decimal to binary here
			int i = 0;
			for (i = 9; i >= 0; i--) {
				tempResult = resultOfAnd >> i;
				if (tempResult & 1) {
					tempBuffer[9-i] = '1';
				} else {
					tempBuffer[9-i] = '0';
				}
			}
			printf("%d\n", indexN);
			int firstDigit = indexN / 10;
			int lastDigit = indexN % 10;
			char c1 = '0' + firstDigit;
			char c2 = '0' + lastDigit;

			tempBuffer[10] = ',';
			//sprintf(indexBuffer, "%d", indexN);
			tempBuffer[11] = c1;
			tempBuffer[12] = c2;
			tempBuffer[13] = '\0';

			//sprintf(tempBuffer, "%d", tempBuffer);

			sendto(sock, tempBuffer, strlen(tempBuffer), 0, (struct sockaddr *)&clientAddress, fromLength);
		}				
	} 
}

	

