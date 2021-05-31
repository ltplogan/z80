#include <stdio.h>
#include <stdlib.h>

typedef struct Cat
{
	char *name;
	int birth;
	float weight;
} Cat;

typedef long long int BigInt;

int main(int argc, char const *argv[]) {
	Cat *dan = malloc(sizeof(Cat));
	dan->name = "Lorenzo";
	dan->birth = 1977;
	dan->weight = 80.50f;
	
	printf("Nombre: %s Birth: %d Weight: %f\n", dan->name, dan->birth, dan->weight);
	free(dan);
	
	BigInt bInt = 45;
	printf("%lld\n", bInt);
	
	return 0;
}