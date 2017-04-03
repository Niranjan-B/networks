#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<strings.h>
#include<stdbool.h>

int main() {
	int index = 1;
	int bufferIndex = 0;
	char buffer[40];
	bool flag = true;

	FILE *fp;
	int c;
	fp = fopen("job.txt", "r");

	while((c=fgetc(fp)) != EOF) {
		if (c == '\n' && flag) {
			flag = false;
			buffer[bufferIndex++] = ',';
			//buffer[bufferIndex]='1';
			sprintf(&buffer[bufferIndex], "%d", index);
			printf("%s", buffer);
			index++;
			bufferIndex = 0;
			bzero(&buffer, 40);
			printf("\n");
		} else {
			flag = true;
			buffer[bufferIndex++] = c;
		}
	}
	fclose(fp);
	return 0;
}
