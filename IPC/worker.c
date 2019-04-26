#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

void receive();
void child_proc(int conn);
void test_a_case(char output[], char content[], char in_name[], char in_content[]);


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
		
		char content[100001] = "";

		while (strcmp("</file>",ptr) != 0) {
//			printf("%s\n",ptr);
			strcat(content,ptr);
			strcat(content,"\n");

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
		int leng = strlen(test_f_name);
		int i,div;
		char str[50] = "";
		for(i = leng-1; i >= 0; i--) {
			if(test_f_name[i] == '/' || test_f_name[i] == '\\') {
				div = i;
				break;
			}
		}
		for(i = div+1; i<leng; i++) {

			char temp[5] = "";
                	sprintf(temp,"%c",test_f_name[i]);
                	strcat(str,temp);
		}

//		printf("%s\n",content);
		printf("fname: %s\n",str);
//		printf("%s\n",test_in_str);

		//make program with content
		system("exec rm program program.c");
			//1. make program.c
		if(fork() == 0)            //creating 2nd child
        	{
            		close(STDIN_FILENO);   //closing stdin

			int fd = open("program.c", O_WRONLY | O_CREAT, 0644) ;
			close(STDOUT_FILENO);
			dup(fd);
			close(fd) ;

			printf("%s\n",content);
        	    exit(1);
        	} else {
              	 	int exit_code ;
              	 	wait(&exit_code) ;
        	}
/*
		FILE *f;
       		f=fopen("program.c","w");
        	fprintf(f, content);
        	fclose(f);
*/
			//2. compile program.c as program
		system("exec gcc program.c -o program");

		//test		
		char output[100001];
		test_a_case(output, content, str, test_in_str);
		printf("%s\n",output);
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

void test_a_case(char output[], char content[], char in_name[], char in_content[]) {
	char result[100001] = "";

	// make 1.in

	FILE *f;

	f=fopen(in_name,"w");
	fprintf(f,in_content);
	fclose(f);

	// name of output
	int div;
	char out_name[50];
	for(int i = 0; i < strlen(in_name); i++) {
		if(in_name[i] == '.') {
			div = i;
			break;
		}
	}
	strncpy(out_name,in_name,div);
	if(out_name[strlen(out_name)-1] != 't')
		strcat(out_name,".out");
//	printf("out name: %s\n",out_name);

	// apply it: make 1.out
	if(fork() == 0)            //creating 2nd child
        {
		int fd = open(out_name, O_WRONLY | O_CREAT, 0644) ;
		close(STDOUT_FILENO);
		dup(fd);
		close(fd);
		execl("./applier", "./applier", out_name, "program", in_content, (char *) 0x0) ;
        	exit(1);
        } else {
              	 int exit_code ;
              	 wait(&exit_code) ;
        }

	// read 1.out
	strcpy(output,result);
}
