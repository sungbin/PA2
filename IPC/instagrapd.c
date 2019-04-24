#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#define BUFFER_SIZE 256

char ip[50] = "";
char wport[50] = "";
char port[10] = "";
char* dir_name;

typedef struct* Submission pSubmission;
typedef struct Submission {
	char id[50];
	char pw[50];
	char source[100001];

} Submission;

void cli(int argc, char const *argv[]) {
	int i;
        char* wip_port;

	for(i = 1; i < argc; i+=2) {
                if(strcmp("-w", argv[i]) == 0) {
                        wip_port = argv[i+1];
                } else if(strcmp("-p", argv[i]) == 0) {
                        port = argv[i+1];
                } else {
                        dir_name = argv[i];
                }
        }
	for(i = 0; wip_port[i] != ':'; i++) {
                char temp[5] = "";
                sprintf(temp,"%c",wip_port[i]);
                strcat(ip,temp);
        }
        i++;
	for(; i< strlen(wip_port); i++) {
                char temp[5] = "";
                sprintf(temp,"%c",wip_port[i]);
                strcat(wport,temp);
        }
}

int
main(int argc, char const *argv[])
{
	cli(argc,argv);
	/*
	printf("dir_name: %s\n",file_name);
	printf("ip: %s\n",ip);
	printf("port: %s\n",port);
	wport
	*/
	
}
