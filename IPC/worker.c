#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

void receive();
void child_proc(int conn);

char content[1000001];
char port[10] = "";

int
main(int argc, char const *argv[])
{
	strcpy(port,argv[2]);
        receive();
}

void receive() {
	
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
        address.sin_port = htons(atoi(port));
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
                }
                else {
                        close(new_socket) ;
                }
        }
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
//        printf(">%s\n", data) ;
    char *ptr = strtok(data, "\n");      // " " 공백 문자를 기준으로 문자열을 자름, 포인터 반환
    char test_f_name[50] = "";
    char test_in_str[1000001] = "";
    while (ptr != NULL)               // 자른 문자열이 나오지 않을 때까지 반복
    {
        //printf("%s\n", ptr);          // 자른 문자열 출력
	if(strcmp("<file>",ptr) == 0) {
		ptr = strtok(NULL, "\n");
		
		while (strcmp("</file>",ptr) != 0) {
			printf("%s\n",ptr);
			ptr = strtok(NULL, "\n");
		}
		ptr = strtok(NULL, "\n");
		
		//1.in
        	strcpy(test_f_name,ptr);
        	ptr = strtok(NULL, "\n"); // 1.in ~~
        	strcpy(test_in_str,"");
        	do {
               		strcat(test_in_str,ptr);
                	strcat(test_in_str,"\n");

                	ptr = strtok(NULL, "\n");
        	} while(ptr != NULL && strcmp("<id>",ptr) != 0);

        //add THREAD
        	printf("name: %s\n",test_f_name);
        	printf("%s\n",test_in_str);
	}

	//add THREAD

        ptr = strtok(NULL, "\n");      // 다음 문자열을 잘라서 포인터를 반환
    }	
       
        orig = data ;
//        sleep(3); //after tests

/* This logic that get test result from worker, and send to submitter */
        char message[1000001];
        
        strcpy(message,data);
/**/    
        while (len > 0 && (s = send(conn, data, len, 0)) > 0) { //back to submitter
                data += s ;
                len -= s ;
        }
        shutdown(conn, SHUT_WR) ;
        if (orig != 0x0) 
                free(orig) ;
}

