#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *find_aminoacid(char *prot, char *amino, char *repl) {
  char buff[609];
  char *p;
  if (!(p = strstr(prot, amino)))
   	return prot;
  	strncpy(buff, prot, p-prot);  
  	buff[p-prot] = 0;
  	sprintf(buff+(p-prot), "%s%s", repl, p+strlen(amino));
  	
  	return buff;
  }
  
int main()
{
	FILE *file;
	char seq[609];
	if ((file = fopen("adding-aminos.txt", "r+")) == NULL) {
		printf("Erro ao abrir arquivo! \n");
		return 1;
	}

	while(fgets(seq, 609, file) != NULL) {
		char *result;
		result = find_aminoacid(seq,"V","-");
		printf(" %s", result);
		FILE *writer;
		if (!(writer = fopen("adding-aminos.txt","w"))) {                        
	  		printf("Erro! Impossivel abrir arquivo! \n");
	  		exit(1);
  		}
		fprintf(writer,"%s", result);
		fclose(writer);
		fclose(file);
	}

	return 0;

}
