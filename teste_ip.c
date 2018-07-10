#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main(){
	FILE *config_file;
	char ipLinha[2][20];
	int i=0;
	if ((config_file = fopen("config.txt", "r")) == NULL) {
		printf("Erro ao abrir o arquivo!");
		return 1;
	}
	while(fgets(ipLinha[i], 13, config_file) != NULL) {
		printf("%s", ipLinha[i]);
		i++;
	}
}
