#include <stdio.h>

int main() {
	int count, a,b, sum = 0;
	scanf("%d",&count);
	for(int i = 0; i < count; i++) {
		scanf("%d %d",&a,&b);
		sum += (a * b);
	}
	printf("%d\n",sum);

	return 0;
}
