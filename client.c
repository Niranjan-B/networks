#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<strings.h>
#include<stdbool.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<errno.h>
#include<arpa/inet.h>
#include<unistd.h>
#include <netdb.h>

char numOneBuffer[10];
char numTwoBuffer[10];
char numBuffer[20];
char buffer[40];
int numBufferIndex = 0;
bool commaFlag = false;

char resultBuffer[4000];
char outputBuffer[4000];

int numOfLines = 0;


void clearBuffer() {
	bzero(&numOneBuffer, 10);
	bzero(&numTwoBuffer, 10);
	bzero(&numBuffer, 20);
	bzero(&buffer, 40);
	commaFlag = false;
	numBufferIndex = 0;
}

void storeSecondNum(char *num, int size) {
	int i=0;
	for (i=0; i<(10 - size); i++) {
		numTwoBuffer[i] = '0';
	}
	while(*num) {
		numTwoBuffer[i] = *num++;
		i++;
	}
}

void storeFirstNum(char *num, int size) {
	int i=0;
	for (i=0; i<(10 - size); i++) {
		numOneBuffer[i] = '0';
	}
	while(*num) {
		numOneBuffer[i] = *num++;
		i++;
	}
}

void paddZeros(char temp[]) {
	// splitting string based on ,
	char *resultToken;
	char *result;
	char *rest = temp;
	int count = 0;

	// re-used this chunk of code from stack-overflow
	while ((resultToken = strtok_r(rest, ",", &rest))) {
			if (count == 0) {
				storeFirstNum(resultToken, strlen(resultToken));
			} else {
				storeSecondNum(resultToken, strlen(resultToken));
			}
			count++;
	}

}

int storePaddedNumbersIntoBuffer(int bufferIndex) {
	buffer[bufferIndex++] = ',';
	// storing first number
	int i=0;
	for (i=0; i < 10; i++) {
		buffer[bufferIndex++] = numOneBuffer[i];
	}
	i = 0;
	buffer[bufferIndex++] = ',';
	// storing second number
	for (i=0; i < 10; i++) {
		buffer[bufferIndex++] = numTwoBuffer[i];
	}
	return bufferIndex;
}

void processAndDisplayOutput(char resultOfComp[]) {
	printf("The client has successfully finished receiving all computation results from the edge server\n");
	printf("The final computation results are:\n");

	char tmpBuffer[11];
	bzero(tmpBuffer, 11);
	int i=0;
	int j=1;

	for (i = 0; i < strlen(resultOfComp); i++) {
        if ((j % 10) == 0) {
            tmpBuffer[j-1] = resultOfComp[i];
            printf("%s\n", tmpBuffer);
            bzero(tmpBuffer, 11);
            j = 0;
        }
    	tmpBuffer[j-1] = resultOfComp[i];
    	j++;
	}
}

int main(int argc, char **argv) {

	if (argc != 2) {
		printf("only the file is required as an argument");
		exit(-1);
	}

	printf("The client is up and running\n");

	int index = 0;
	bool flag = true;
	int bufferIndex = 0;

	bzero(resultBuffer, 4000);

	FILE *fp;
	int c;
	fp = fopen(argv[1], "r");

	while((c=fgetc(fp)) != EOF) {
		if (c == '\n' && flag) {
			flag = false;

			numOfLines += 1;
			
			paddZeros(numBuffer);
			bufferIndex = storePaddedNumbersIntoBuffer(bufferIndex);

			buffer[bufferIndex++] = ',';
			// check for single or double digit, if single digit => make it into double digit
			if (index < 10) {
				sprintf(&buffer[bufferIndex++], "%d", 0);
				sprintf(&buffer[bufferIndex++], "%d", index);	
			} else {
				sprintf(&buffer[bufferIndex++], "%d", index);
			}
			
			// store lines of strings inside a buffer
			strncat(resultBuffer, buffer, strlen(buffer)); 

			index++;
			clearBuffer();
			bufferIndex = 0;
		} else {
			if (c == ',') {
				commaFlag = true;
				numBuffer[numBufferIndex++] = c;
			} else {
				if (!commaFlag) {
					buffer[bufferIndex++] = c;
				} else {
					numBuffer[numBufferIndex++] = c;
				}
			}

			flag = true;
		}
	}
	fclose(fp);

	// ----------------------------- start transmitting the buffer here ------------------------------------------------
	//printf("sending result buffer %s %d\n", resultBuffer, numOfLines);
	//printf("%s\n", *resultBuffer);
	
	struct sockaddr_in edgeServer;
	int sock, addrLen;

	if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket creation error: ");
		exit(-1);
	}

	edgeServer.sin_family = AF_INET;
	edgeServer.sin_port = htons(23355);
	inet_pton(AF_INET, "127.0.0.1", &edgeServer.sin_addr);
	bzero(&edgeServer.sin_zero, 8);
	addrLen = sizeof(struct sockaddr_in);

	int con = connect(sock, (struct sockaddr *)&edgeServer, addrLen);
	if (con < 0) {
		perror("Connect :");
		exit(-1);
	}

	send(sock, resultBuffer, strlen(resultBuffer), 0);
	printf("The client has successfully finished sending %d lines to the edge server\n", numOfLines);
	
	bzero(outputBuffer, 4000);
	// waiting for response from server
	while(1) {
		int length = 0;
		length = recv(sock, outputBuffer, 4000, 0);
		outputBuffer[length] = '\0';
		processAndDisplayOutput(outputBuffer);
	}

	return 0;
}
