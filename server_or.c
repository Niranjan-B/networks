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

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0) {
		perror("Socket");
		exit(-1);	
	}
	
	length = sizeof(serverAddress);
	bzero(&serverAddress, length);
	
	// init adddress
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(21355);
	inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);
	bzero(&serverAddress.sin_zero, 8);

	if(bind(sock, (struct sockaddr *)&serverAddress, length) < 0) {
		perror("Binding");
		exit(-1);	
	}

	unsigned int fromLength = sizeof(clientAddress);

	while(true) {
		bzero(buf, 40);
		n = recvfrom(sock, buf, 40, 0, (struct sockaddr *)&clientAddress, &fromLength);
		if(n < 0) {
			//perror("Received from");	
		} else {
			// splitting string based on , here
			int j=0;
			char buffer[40]; //= {'a','n','d',',','0','0','1','1','1','1','1','1','1','1',',','1','1','1','1','1','1','1','1','1','1',',','2','2'};
		 	bzero(buffer, 40);
			strncpy(buffer, buf, strlen(buf));
			buffer[strlen(buf)] = '\0';
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
				} else if (count == 2) {
					char *numTwoTemp = token;
					numTwo = numTwoTemp;
				} else if (count == 3){
					indexN = atoi(token);
				}
				count++;     			
			}

			char tempBuffer[41];
			bzero(tempBuffer, 41);
			
			int i=0;
			while(i < 10) {
				tempBuffer[i] = (((*numOne++)-'0') | ((*numTwo++)-'0')) + '0';
				i++;
			}

			
			int firstDigit = indexN / 10;
			int lastDigit = indexN % 10;
			char c1 = '0' + firstDigit;
			char c2 = '0' + lastDigit;

			tempBuffer[10] = ',';
			tempBuffer[11] = c1;
			tempBuffer[12] = c2;
			tempBuffer[13] = '\0';

			sendto(sock, tempBuffer, strlen(tempBuffer), 0, (struct sockaddr *)&clientAddress, fromLength);
		}				
	} 
}