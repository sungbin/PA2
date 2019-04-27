#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

void receive();
void* function(void *arg);
void child_proc(int conn);
void test_a_case(char output[], char content[], char in_name[], char in_content[]);


char content[1000001];
char port[10] = "";

struct Arg {
	char test_id[50];
	char content[100001];
	char result[100001];
};

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
    char message[1000001] = "";
    char *ptr = strtok(data, "\n");      // " " 공백 문자를 기준으로 문자열을 자름, 포인터 반환
    char test_f_name[50] = "";
    char test_in_str[1000001] = "";
    int arg_count = 0;
    struct Arg args[1000001];


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
		
		printf("test_in_str: %s\n",test_in_str);
		printf("content: %s\n",content);

        //add THREAD: test_id, content
	//get test_id
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
		for(i = 0; i < strlen(str); i++) {
 	               if(str[i] == '.') {
        	                div = i;
                        break;
              	  }
        	}
		char test_id = "";
        	strncpy(test_id,str,div); //test_id: 

		strcpy(args[arg_count].test_id,test_id);
		strcpy(args[arg_count].content,content);
		arg_count++;
	}
	
	//add THREAD
	/*
	pthread_t threads[1000001];
	for(int i = 0; i < arg_count; i++) {
		pthread_create(&(threads[i]), NULL, function, (void*) &args[i]);
	}
	for(int i = 0; i < arg_count; i++) {
		pthread_join(threads[i], NULL);
        }
	for(int i = 0; i < arg_count; i++) {
		strcat(message,args[i].result);
	}
*/

        ptr = strtok(NULL, "\n");      // 다음 문자열을 잘라서 포인터를 반환
    }	
       
        orig = data ;
        strcpy(data,message);
/**/    
	printf(">%s\n",data);
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
	strcat(result,out_name);
	strcat(result,"\n");
	// apply it: make 1.out
	pid_t child_pid = fork() ;

	if(child_pid == 0)            //creating 2nd child
        {
		int fd = open(out_name, O_WRONLY | O_CREAT, 0644) ;
		close(STDOUT_FILENO);
		dup(fd);
		close(fd);
		execl("./applier", "./applier", out_name, "program", in_content, (char *) 0x0) ;
        	exit(1);
        } else {
		sleep(3);
		kill(child_pid, SIGKILL);
		wait(0x0);
        }


	// read 1.out
	char ch;
	int n;
	FILE * fp=fopen(out_name,"r");	
	if(fp==NULL){		
		printf("파일 오픈 실패 !\n");
		return ;
	}
	while(1){
		ch=fgetc(fp);
		if(ch==EOF)
			break;
		char temp[5] = "";
                sprintf(temp,"%c",ch);
                strcat(result,temp);	
//		putchar(ch);
	}
	fclose(fp);
	char t_command[70] = "";
	strcat(t_command,"exec rm ");
	strcat(t_command,in_name);
	strcat(t_command," ");
	strcat(t_command,out_name);
	strcat(t_command," ");
	strcat(t_command,"program");	
	strcat(t_command," ");
	strcat(t_command,"program.c");
	
	system(t_command); //clear command

	strcpy(output,result);
}
/*
struct Arg {
        char test_id[50];
        char content[100001];
        char result[100001];
}
*/
void* function(void *arg) {
    struct Arg *marg = (struct Arg *)arg;
    char program[500];
    char program_c[500];
    strcpy(program,"program");
    strcat(program,marg->test_id);
    strcpy(program_c,program);
    strcat(program_c,".c");

	//make program with content
		char rm_1[500];
		strcpy(rm_1,"exec rm ");
		strcat(rm_1,program);
		strcat(rm_1," ");
		strcat(rm_1,program_c);

		system(rm_1);
			//1. make program.c
		if(fork() == 0)            //creating 2nd child
        	{
            		close(STDIN_FILENO);   //closing stdin

			int fd = open(program_c, O_WRONLY | O_CREAT, 0644) ;
			close(STDOUT_FILENO);
			dup(fd);
			close(fd) ;

			printf("%s",content);
        	    exit(1);
        	} else {
              	 	int exit_code ;
              	 	wait(&exit_code) ;
        	}
			//2. compile program.c as program
		char rm_2[500];
                strcpy(rm_2,"exec gcc -o ");
                strcat(rm_2,program);
                strcat(rm_2," ");
                strcat(rm_2,program_c);

		system(rm_2);
		char ar_3[100];
		strcpy(ar_3,"./");
		strcat(ar_3,program);
		int nResult = access( ar_3, 0 );
		if( nResult == -1 )
		{
			strcpy(marg->result,"build fail\n");
		} else {

		//test		
			char output[100001];
			//test_a_case(output, content, str, test_in_str); //test_in_name, test_in_str
			strcpy(marg->result,output);
		}
}

