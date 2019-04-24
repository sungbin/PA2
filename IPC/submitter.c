#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#define BUFFER_SIZE 256

char ip[50] = "";
char port[10] = "";
char* user_id;
char* user_pw;
char* file_name;
char result[100001] = "";
char message[1000001] = "";
int pipes[2] ;

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
}

void submit(char const* ip_address, int port, char const* message) {
	/*
        printf("user_id: %s\n",user_id);
        printf("user_pw: %s\n",user_pw);
        printf("file_name: %s\n",file_name);
        printf("ip: %s\n",ip);
        printf("port: %s\n",port);
        */
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
//      printf("message:\n%s\n",message);
/*   */

	struct sockaddr_in serv_addr; 
	int sock_fd ;
	int s, len ;
	char buffer[1024] = {0}; 
	char * data ;
	
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

	strcpy(buffer, message);
	//scanf("%s", buffer) ;
	
	data = buffer ;
	len = strlen(buffer) ;
	s = 0 ;
	while (len > 0 && (s = send(sock_fd, data, len, 0)) > 0) {
		data += s ;
		len -= s ;
	}

	shutdown(sock_fd, SHUT_WR) ;

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

void
child_proc(int conn)
{
	char buf[1024] ;
	char * data = 0x0, * orig = 0x0 ;
	int len = 0 ;
	int s ;

	while ( (s = recv(conn, buf, 1023, 0)) > 0 ) {
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
	printf(">%s\n", data) ;
	
	orig = data ;
	while (len > 0 && (s = send(conn, data, len, 0)) > 0) {
		data += s ;
		len -= s ;
	}
	shutdown(conn, SHUT_WR) ;
	if (orig != 0x0) 
		free(orig) ;
}

//wating until receive message from instagrapd
void
parent_proc(int port_id)
{
	int listen_fd, new_socket ; 
	struct sockaddr_in address; 
	int opt = 1; 
	int addrlen = sizeof(address); 

	char buffer[1024] = {0}; 

	listen_fd = socket(AF_INET /*IPv4*/, SOCK_STREAM /*TCP*/, 0 /*IP*/) ;
	if (listen_fd == 0)  { 
		perror("socket failed : "); 
		exit(EXIT_FAILURE); 
	}
	
	memset(&address, '0', sizeof(address)); 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY /* the localhost*/ ; 
	address.sin_port = htons(port_id); 
	if (bind(listen_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("bind failed : "); 
		exit(EXIT_FAILURE); 
	} 

	while (1) {
		if (listen(listen_fd, 16 /* the size of waiting queue*/) < 0) { 
			perror("listen failed : "); 
			exit(EXIT_FAILURE); 
		} 

		new_socket = accept(listen_fd, (struct sockaddr *) &address, (socklen_t*)&addrlen) ;
		if (new_socket < 0) {
			perror("accept"); 
			exit(EXIT_FAILURE); 
		} 

		if (fork() > 0) {
			child_proc(new_socket) ;
			exit(0);
		}
		else {
			close(new_socket) ;
		}
	}
}

int
main(int argc, char const *argv[])
{
	
	cli(argc,argv);
	
	pid_t child_pid ;
	int exit_code ;

	if (pipe(pipes) != 0) {
		perror("Error") ;
		exit(1) ;
	}

	child_pid = fork() ;
	if (child_pid == 0) {
		submit(ip,atoi(port),message); //ip, port, message
	}
	else {
		parent_proc(atoi(port)) ;
	}
	wait(&exit_code) ;

	exit(0) ;
	
}
