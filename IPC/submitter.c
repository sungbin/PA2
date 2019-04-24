#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#define BUFFER_SIZE 256
#include <unistd.h>
#include <error.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <signal.h>

void submit(char const* ip_address, int port, char const* message);
void read_file(char const* file_name);
void cli(int argc, char const *argv[]);
char ip[50] = "";
char port[10] = "";
char* user_id;
char* user_pw;
char* file_name;
char result[100001] = "";
char message[1000001] = "";
int sock_fd;
struct sockaddr_in serv_addr; 
int s, len ;
char buffer[1024] = {0}; 
char * data ;

void submit(char const* ip_address, int port, char const* message) {
        sock_fd = socket(AF_INET, SOCK_STREAM, 0) ;
        if (sock_fd <= 0) {
                perror("socket failed : ") ;
                exit(EXIT_FAILURE) ;
        }
        
        memset(&serv_addr, '0', sizeof(serv_addr));
        serv_addr.sin_family = AF_INET; 
        serv_addr.sin_port = htons(port); 
        if (inet_pton(AF_INET, ip_address, &serv_addr.sin_addr) <= 0) {
                perror("inet_pton failed : ") ;
                exit(EXIT_FAILURE) ;
        }
        
        if (connect(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
                perror("connect failed : ") ;
                exit(EXIT_FAILURE) ;
        }

//      scanf("%s", buffer) ;
        strcpy(buffer,message);
        
        data = buffer ;
        len = strlen(buffer) ;
        s = 0 ;
        while (len > 0 && (s = send(sock_fd, data, len, 0)) > 0) {
                data += s ;
                len -= s ;
        }
        
        shutdown(sock_fd, SHUT_WR) ;

}

void read_file(char const* file_name) {

	FILE *fp;
	char buffer[BUFFER_SIZE + 1];

	if ((fp = fopen(file_name, "r+")) != NULL) {
		memset(buffer, 0, sizeof(buffer));
		while (fread(buffer, 1, BUFFER_SIZE, fp) != 0) {
			strncat(result,buffer,BUFFER_SIZE);
		}
		fclose(fp);
	}
//	printf("%s",result);
}

void cli(int argc, char const *argv[]) {
	int i;
        char* ip_port;

	for(i = 1; i < argc; i+=2) {
                if(strcmp("-n", argv[i]) == 0) {
                        ip_port = argv[i+1];
                } else if(strcmp("-u", argv[i]) == 0) {
                        user_id = argv[i+1];
                } else if(strcmp("-k", argv[i]) == 0) {
                        user_pw = argv[i+1];
                } else {
                        file_name = argv[i];
                }
        }
	for(i = 0; ip_port[i] != ':'; i++) {
                char temp[5] = "";
                sprintf(temp,"%c",ip_port[i]);
                strcat(ip,temp);
        }
        i++;
	for(; i< strlen(ip_port); i++) {
                char temp[5] = "";
                sprintf(temp,"%c",ip_port[i]);
                strcat(port,temp);
        }

	strcat(message,"<id>\n");
        strcat(message,user_id);
        strcat(message,"\n");
        strcat(message,"<pw>\n");
        strcat(message,user_pw);
        strcat(message,"\n");

        strcat(message,"<file>\n");

        read_file(file_name);
        strcat(message,result);

        strcat(message,"\n<end>");
}

void
parent_proc(int sock_fd) {
	char buf[1024] ;
	data = 0x0 ;
	len = 0 ;
	while ( (s = recv(sock_fd, buf, 1023, 0)) > 0 ) {
		buf[s] = 0x0 ;
		if (data == 0x0) {
			data = strdup(buf) ;
			len = s ;
		}
		else {
			data = realloc(data, len + s + 1) ;
			strncpy(data + len, buf, s) ;
			data[len + s] = 0x0 ;
			len += s ;
		}

	}
	printf(">%s\n", data); 	
}

void receive() {
	char buf[1024] ;
        data = 0x0 ;
        len = 0 ;
        while ( (s = recv(sock_fd, buf, 1023, 0)) > 0 ) {
                buf[s] = 0x0 ;
                if (data == 0x0) {
                        data = strdup(buf) ;
                        len = s ;
                }
                else {
                        data = realloc(data, len + s + 1) ;
                        strncpy(data + len, buf, s) ;
                        data[len + s] = 0x0 ;
                        len += s ;
                }

        }
        printf(">%s\n", data);
}

int 
main(int argc, char const *argv[]) 
{ 
	cli(argc,argv);
	submit(ip,atoi(port),message); //ip, port, message`
	receive();
/*	
	submit(ip,atoi(port),message); //ip, port, message
	parent_proc(sock_fd) ;
	exit(0);
*/
} 

