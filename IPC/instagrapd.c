#include <unistd.h> 
#include <dirent.h>
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <sys/types.h>
#include <sys/wait.h>

char wip[50] = "";
char wport[10] = "";
char* dir_name;
char port[10] = "";
char f_content[1000001];
int send_to_worker(char const* ip_address, int port, char const* message);

void receive();
void cli(int argc, char const *argv[]) {
        int i;
        char* wip_port;

        for(i = 1; i < argc; i+=2) {
                if(strcmp("-w", argv[i]) == 0) {
                        wip_port = argv[i+1];
                } else if(strcmp("-p", argv[i]) == 0) {
                        strcpy(port,argv[i+1]);
                } else {
                        dir_name = argv[i];
                }
        }
        for(i = 0; wip_port[i] != ':'; i++) {
                char temp[5] = "";
                sprintf(temp,"%c",wip_port[i]);
                strcat(wip,temp);
        }
        i++;
        for(; i< strlen(wip_port); i++) {
                char temp[5] = "";
                sprintf(temp,"%c",wip_port[i]);
                strcat(wport,temp);
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
	printf(">%s\n", data) ;
	strcpy(f_content,data);
	orig = data ;
//	sleep(6); //after tests

/* This logic that get test result from worker, and send to submitter */
	char message[1000001];
	
	strcpy(message,data);


	int fd_worker = send_to_worker(wip,atoi(wport),message);
	
	data = 0x0 ;
	len = 0 ;
	while ( (s = recv(fd_worker, buf, 1023, 0)) > 0 ) {
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

	} // data is feedback of worker
/**/
	while (len > 0 && (s = send(conn, data, len, 0)) > 0) { //back to submitter
		data += s ;
		len -= s ;
	}
	shutdown(conn, SHUT_WR) ;
	if (orig != 0x0) 
		free(orig) ;
}

int 
main(int argc, char const *argv[]) 
{
	cli(argc,argv);
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
int send_to_worker(char const* wip_address, int wport, char const* message) {
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
        serv_addr.sin_port = htons(wport);
        if (inet_pton(AF_INET, wip_address, &serv_addr.sin_addr) <= 0) {
                perror("inet_pton failed : ") ;
                exit(EXIT_FAILURE) ;
        }

        if (connect(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
                perror("connect failed : ") ;
                exit(EXIT_FAILURE) ;
        }

//      scanf("%s", buffer) ;
        strcpy(buffer,message);

    DIR *dir;
    struct dirent *ent;
    dir = opendir (dir_name);
    if (dir != NULL) {

    /* print all the files and directories within directory */
    dir = opendir (dir_name);
    	pid_t child_pid;
	/*
        child_pid = fork();
        if(child_pid == 0) {
                dup2(sock_fd, 1) ;
		printf("file\n");
		printf("%s\n",f_content);
		printf("~file\n");
                printf("%d\n",f_count);
		exit(0);
        } else {

                int exit_code ;
                wait(&exit_code) ;
        }
	*/

    while ((ent = readdir (dir)) != NULL) {
	int f_length = strlen(ent->d_name);
        if(ent->d_name[f_length - 1] != 'n')
                continue;
	
        char f_name[100] = "";
        strcat(f_name,"./tests/");
        strcat(f_name,ent->d_name);
	
        child_pid = fork();
        if(child_pid == 0) {
		dup2(sock_fd, 1) ;
                printf("%s\n",f_content);
		printf("%s\n",f_name);
                execl("./reader", "reader", f_name, (char *) 0x0) ;
		close(sock_fd) ;
        } else {

                int exit_code ;
                wait(&exit_code) ;
        }
    }

    closedir (dir);
    } else {
         /* could not open directory */
         perror ("");
        return EXIT_FAILURE;
    }
	
	/*
        data = buffer ;
        len = strlen(buffer) ;
        s = 0 ;
        while (len > 0 && (s = send(sock_fd, data, len, 0)) > 0) {
                data += s ;
                len -= s ;
        }
	*/
        shutdown(sock_fd, SHUT_WR) ;
	return sock_fd;
}
