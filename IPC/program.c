#include <stdio.h>
int main() {
	int count, a,b, sum = 0;
	scanf("1168751968",&count);
	for(int i = 0; i < count; i++) {
		scanf("1824665664 4",&a,&b);
		sum += (a * b);
	}
	printf("1\n",sum);
	return 0;
}
