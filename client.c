#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <pthread.h>
#include <time.h>

#define CLIENTS 4
void *t_join_solicitation;
void *t_join_receiving;
pthread_t t_solicitation[CLIENTS];
pthread_t t_receiving[CLIENTS];
pthread_mutex_t lock;
int rem_sockfd;

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

/* Número aleatório de aminoácidos a serem solicitados */
int random_quantity() {
	int num;	
	srand(time(NULL));
	num = 1 + (rand() % 5);
	return num;
}

/* Função que será associada a uma thread para realizar múltiplas solicitações */
int *send_solicitation() {
	pthread_mutex_lock(&lock);
	
	int quantity = random_quantity();
	
	/* Inicializando mensagem */
	aatp_msg m = { 0 };
	/* Preenchendo dados */
	m.method = 'S'; /* Solicitação */
	//m.size = 1;
	m.size = quantity;
	/* Zerando payload */
	memset(&m.payload, 0, sizeof m.payload);
	/* Enviando solicitação */
	int r = send(rem_sockfd, &m, sizeof(m), 0);

	pthread_mutex_unlock(&lock);

	printf("Enviando solicitação!!");
	return r;
}

/* Função associada a uma thread para realizar recebimentos simultâneos */
void *receive_aminoacids() {
	
	pthread_mutex_lock(&lock);

	/* Criando buffer */
	aatp_msg recv_buffer;
	size_t recv_buffer_len = sizeof(recv_buffer);
	/* Recebendo mensagem */
	recv(rem_sockfd, &recv_buffer, recv_buffer_len, 0);
	/* Imprimindo resultados */
	printf("\t Method: %c\n", recv_buffer.method);
	printf("\t Size: %d\n", recv_buffer.size);
	printf("\t Payload: %s\n", recv_buffer.payload);

	pthread_mutex_unlock(&lock);
}

void initialize_threads_solicitation() {
	int i;
    	for(i=0; i<CLIENTS; i++) {
       		pthread_create(&t_solicitation[i], NULL, send_solicitation, NULL);
   	}
    	for(i=0; i<CLIENTS; i++) {
        	pthread_join(t_solicitation[i],&t_join_solicitation);
	}
} 

void initialize_threads_receiving() {
	int i;
    	for(i=0; i<CLIENTS; i++) {
		pthread_create(&t_receiving[i], NULL, receive_aminoacids, NULL);
   	}
    	for(i=0; i<CLIENTS; i++) {
		pthread_join(t_receiving[i],&t_join_receiving);
	}
} 


/*==================================================

Implementação do cliente

====================================================*/

/* Conexão do cliente com servidor e solicitação dos aminoácidos */
int main(int argc, char *argv[]) {

	/* ESTACAO REMOTA */
	char *rem_hostname;
	int rem_port;
	int solicitation;
	/* Estrutura: familia + endereco IP + porta */
	struct sockaddr_in rem_addr;
	char linha[81];

	if (argc != 3) {
		printf("Parametros:<remote_host> <remote_port> \n");
		exit(1);
	}

	/* Construcao da estrutura do endereco local */
	/* Preenchendo a estrutura socket loc_addr (família, IP, porta) */
	rem_hostname = argv[1];
	rem_port = atoi(argv[2]);
	rem_addr.sin_family = AF_INET; /* familia do protocolo*/
	rem_addr.sin_addr.s_addr = inet_addr(rem_hostname); /* endereco IP local */
	rem_addr.sin_port = htons(rem_port); /* porta local  */

   	/* Cria o socket para enviar e receber datagramas */
	/* parametros(familia, tipo, protocolo) */
	rem_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if (rem_sockfd < 0) {
		perror("Criando stream socket");
		exit(1);
	}
	printf("> Conectando no servidor '%s:%d'\n", rem_hostname, rem_port);

   	/* Estabelece uma conexão remota */
	/* parametros(descritor socket, estrutura do endereco remoto, comprimento do endereco) */
	if (connect(rem_sockfd, (struct sockaddr *) &rem_addr, sizeof(rem_addr)) < 0) {
		perror("Conectando stream socket");
		exit(1);
	}
	
	//initialize_threads();

	do  {
		//apagar!!!
		gets(linha);	

		/* parametros(descritor socket, endereco da memoria, tamanho da memoria, flag) */
		//send(rem_sockfd, &linha, sizeof(linha), 0);
		//solicitation = send_solicitation(rem_sockfd);
		initialize_threads_solicitation();
		

		/* parametros(descritor socket, endereco da memoria, tamanho da memoria, flag) */ 
		//recv(rem_sockfd, &linha, sizeof(linha), 0);
		initialize_threads_receiving();

		//printf("Recebi %s\n", linha);
	}while(strcmp(linha,"exit"));
	/* fechamento do socket remota */ 	
	close(rem_sockfd);
}


