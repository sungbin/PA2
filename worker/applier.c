#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

time_t start;
char f_name[500];

void
handler(int sig)
{
	if(time(NULL) - start > 3.0) {
		kill(0, SIGKILL);
		wait(0x0);
	
		int fd = open(f_name, O_WRONLY | O_CREAT, 0644) ;
                close(STDOUT_FILENO);
                dup(fd);
		
		printf("3 seconds");

		exit(0) ;	
	} else {
		return;
	}
}


int main(int argc, char** argv)
{
	signal(SIGTERM, handler) ;
	strcpy(f_name,argv[1]);

	if(argc < 4) {
		printf("error: argc is %d\n",argc);
		exit(1);
	}

//	printf("n: %d\n",strlen(argv[3])) ;

        int des_p[2];
        if(pipe(des_p) == -1) {
          perror("Pipe failed");
          exit(1);
        }

        if(fork() == 0)            //first fork
        {
            close(STDOUT_FILENO);  //closing stdout
            dup(des_p[1]);         //replacing stdout with pipe write 
            close(des_p[0]);       //closing pipe read

	char string[10001] = "";
	strcpy(string,argv[3]);
	strcat(string,"\n");

	write(des_p[1], string, strlen(string));
	close(des_p[1]);
	exit(1);
        }
	start = time(NULL);
        if(fork() == 0)            //creating 2nd child
        {
            close(STDIN_FILENO);   //closing stdin

            dup(des_p[0]);         //replacing stdin with pipe read
            close(des_p[1]);       //closing pipe write
		
		int fd = open(argv[1], O_WRONLY | O_CREAT, 0644) ;
		close(STDOUT_FILENO);
		dup(fd);
		close(des_p[0]); 
		close(fd) ;


	    execl(argv[2], argv[2], (char *) 0x0) ;
            exit(1);
        }

        close(des_p[0]);
        close(des_p[1]);
        wait(0);
        wait(0);
        return 0;
}
