#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <pthread.h>
#include <time.h>

/* struct que representa a mensagem */
/* Estrutura: quantidade de aminoácidos, solicitação ou reposta e lista com aminoácidos solicitados */

/*==================================================

Implementação do protocolo definido pela turma

====================================================*/

typedef struct {
    uint8_t size;
    char method;
    char payload[5];
} aatp_msg;

/*int randomize_aminoacid(int quantity_solicited) {
	int aminoacids_qtd = 22;
	char aminoacids[aminoacids_qtd] = {'A', 'R', 'N', 'D', 'C', 'E', 'Q', 'G', 'H', 'I', 'U', 'L', 'K', 'M', 'F', 'P', 'S', 'T', 'W','Y', 'V', 'O'};
	int i, j, num;
	int quantity = quantity_solicited;
	int sorteados[quantity];	
	srand(time(NULL));	
	
	for(i=0; i<quantity; i++) {
		num = 0 + (rand() % 21);
		printf("Numero sorteado: %d \n", num);
		
		for(j=0; j<aminoacids_qtd; j++) {
			if(j == num) {
				sorteados[i] = aminoacids[j];
				printf("Sorteado: %d -> Aminoácido: %d", num, aminoacids[j]);
				break;
			}
		}
	}

	//return sorteado;
}*/

int randomize_aminoacid() {
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
	return sorteado;
}

void send_aminoacid(int quantity) {

	int i;
	/* Inicializando mensagem */
	aatp_msg m = { 0 };
	/* Preenchendo dados */
	m.method = 'R'; /* Resposta */
	m.size = quantity; /* Enviar a quantidade que foi solicitada */
	/* Zerando payload para evitar enviar lixo
	   caso seja feita uma solicitação de menos de 5 aminoácidos */
	memset(&m.payload, 0, sizeof m.payload);
	/* Preencher com os dados */
	for(i=0; i<quantity; i++){
		int rand_n = randomize_aminoacid();
		m.payload[i] = rand_n;
	}
	/* Enviando resposta */
	int r = send(rem_sockfd, &m, sizeof(m), 0);
}


/*==================================================

Implementação do servidor

====================================================*/

int main(int argc, char *argv[]) {

	/* Variaveis Locais */
	int loc_sockfd, loc_newsockfd, tamanho;
	char linha[81];		
	/* Estrutura: familia + endereco IP + porta */
	struct sockaddr_in loc_addr;
	
	if (argc != 2) {
		printf("Parametros: <local_port> \n");
		exit(1);
	}

   	/* Cria o socket para enviar e receber datagramas */
	/* parametros(familia, tipo, protocolo) */
	loc_sockfd = socket(AF_INET, SOCK_STREAM, 0);	
	
	if (loc_sockfd < 0) {
		perror("Criando stream socket");
		exit(1);
	}

	/* Construcao da estrutura do endereco local */
	/* Preenchendo a estrutura socket loc_addr (família, IP, porta) */
	loc_addr.sin_family = AF_INET; /* familia do protocolo*/
	loc_addr.sin_addr.s_addr = INADDR_ANY; /* endereco IP local */
	loc_addr.sin_port = htons(atoi(argv[1])); /* porta local  */
	bzero(&(loc_addr.sin_zero), 8);

   	/* Bind para o endereco local*/
	/* parametros(descritor socket, estrutura do endereco local, comprimento do endereco) */
	if (bind(loc_sockfd, (struct sockaddr *) &loc_addr, sizeof(struct sockaddr)) < 0) {
		perror("Ligando stream socket");
		exit(1);
	}
	
	/* parametros(descritor socket,
	numeros de conexões em espera sem serem aceites pelo accept)*/
	listen(loc_sockfd, 5);
	printf("> aguardando conexao\n");

	tamanho = sizeof(struct sockaddr_in);
   	/* Accept permite aceitar um pedido de conexao, devolve um novo "socket" ja ligado ao emissor do pedido e o "socket" original*/
	/* parametros(descritor socket, estrutura do endereco local, comprimento do endereco)*/
       	loc_newsockfd =	accept(loc_sockfd, (struct sockaddr *)&loc_addr, &tamanho);

	do  {

		/* parametros(descritor socket, endereco da memoria, tamanho da memoria, flag) */
 		recv(loc_newsockfd, &linha, sizeof(linha), 0);
		printf("Recebi %s\n", linha);

		/* parametros(descritor socket, endereco da memoria, tamanho da memoria, flag) */ 
		send(loc_newsockfd, &linha, sizeof(linha), 0);
		printf("Renvia %s\n", linha);
	}while(strcmp(linha,"exit"));
	/* fechamento do socket local */ 
	close(loc_sockfd);
	close(loc_newsockfd);
}