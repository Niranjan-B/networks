#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdbool.h>

int main() {
	struct sockaddr_in remoteServer, remoteClient;
	int sock;
	char buffer[40], resultBuffer[40];
	unsigned int len;

	if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("Socket");
		exit(-1);
	}

	remoteServer.sin_family = AF_INET;
	remoteServer.sin_port = htons(22355);
	inet_pton(AF_INET, "127.0.0.1", &remoteServer.sin_addr);
	len = sizeof(struct sockaddr_in);


	while(1) {
		bzero(buffer, 40);
		bzero(resultBuffer, 40);

		printf("Enter message ");
		fgets(buffer, 40, stdin);
		//buffer[strlen(buffer)] = '\0';
		
		if (sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&remoteServer, len) < 0) {
			puts("send failed");
			return 1;
		} else {
			puts("Sent to server successfully");
		}
		int n = recvfrom(sock, resultBuffer, 40, 0, (struct sockaddr *)&remoteServer, &len);
		if(n < 0) {
			perror("Failed to receive");
			exit(-1);
		}
		printf("Got an ack %s\n", resultBuffer);
	}

}