#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<strings.h>
#include<stdbool.h>

char numOneBuffer[10];
char numTwoBuffer[10];
char numBuffer[20];
char buffer[40];
int numBufferIndex = 0;
bool commaFlag = false;


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
	//printf("second num = %s\n", numTwoBuffer);
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
	//printf("first num = %s\n", numOneBuffer);
}

void paddZeros(char temp[]) {
	// splitting string based on ,
	char *resultToken;
	char *result;
	char *rest = temp;
	int count = 0;

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

int main() {
	int index = 0;
	bool flag = true;
	int bufferIndex = 0;

	FILE *fp;
	int c;
	fp = fopen("job.txt", "r");

	while((c=fgetc(fp)) != EOF) {
		if (c == '\n' && flag) {
			flag = false;
			
			paddZeros(numBuffer);
			bufferIndex = storePaddedNumbersIntoBuffer(bufferIndex);

			buffer[bufferIndex++] = ',';
			sprintf(&buffer[bufferIndex], "%d", index);
			printf("%s", buffer);
			index++;
			clearBuffer();
			bufferIndex = 0;
			printf("\n");
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
	return 0;
}
