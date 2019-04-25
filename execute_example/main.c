#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(int argc, char** argv)
{
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
//            close(des_p[1]);

	char string[50] = "Hi~!\n";
	write(des_p[1], string, strlen(string));
	close(des_p[1]);
//	    printf("Hi~!\n");

        }

        if(fork() == 0)            //creating 2nd child
        {
            close(STDIN_FILENO);   //closing stdin
            dup(des_p[0]);         //replacing stdin with pipe read
            close(des_p[1]);       //closing pipe write
            close(des_p[0]);

	    execl("./printer", "printer", (char *) 0x0) ;
            exit(1);
        }

        close(des_p[0]);
        close(des_p[1]);
        wait(0);
        wait(0);
        return 0;
}
