#include <stdio.h>
#include <time.h>
#include <stdlib.h>
int main() {
	time_t t = time(NULL);
	float tt = 10.0;
	while(time(NULL) - t < tt) {
		sleep(1);
		printf("1 2 3 4\n");
	}
	printf("Hi~!, I'm sb\n");


}
