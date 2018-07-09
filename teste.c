#include <stdio.h>
#include <time.h>	


int main() {	
	int aminoacids_qtd = 22;
	char aminoacids[] = {'A', 'R', 'N', 'D', 'C', 'E', 'Q', 'G', 'H', 'I', 'U', 'L', 'K', 'M', 'F', 'P', 'S', 'T', 'W','Y', 'V', 'O'};
	int j, num, sorteado;	
	srand(time(NULL));	
	num = 0 + (rand() % 21);
	printf("Numero sorteado: %d \n", num);
		
	for(j=0; j<aminoacids_qtd; j++) {
		if(j == num) {
			sorteado = aminoacids[j];
			printf("Sorteado: %d -> Aminoácido: %c", num, aminoacids[j]);
			break;
		}
	}
	return 0;
}
