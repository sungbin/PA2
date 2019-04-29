#include <stdio.h>

int
main(int argc, char const *argv[])
{
	char ch;

	int n;

	FILE * fp=fopen(argv[1],"rt");



	if(fp==NULL){

		printf("파일 오픈 실패 !\n");

		return -1;

	}



	while(1){

		ch=fgetc(fp);



		if(ch==EOF)

			break;



		putchar(ch);

	}

	fclose(fp);



}
