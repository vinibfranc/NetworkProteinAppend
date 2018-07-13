/*
	Vinícius Franceschi
	Informática Biomédica - UFCSPA
	Redes de Computadores
 */

/*
	Comandos para compilar e rodar no terminal do Linux:
		gcc client.c -o cli -pthread
		./cli
 */

/* Cliente: responsável pelo envio das solicitações e montagem da proteína */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <time.h>

// TODO: Alterar para quantidade de hosts (alunos) conforme quantos forem apresentar e arquivo de configuração */
#define CLIENTS 13
#define PORT 1337
#define PROTEIN_SIZE 609
void *t_client_join;
pthread_t t_client[CLIENTS];
pthread_mutex_t lock_resource;
int count = 0;
int conn_active[CLIENTS];
char each_ip[CLIENTS][15];

//int ips[CLIENTS][15];

/*==================================================
Implementação do protocolo definido pela turma
====================================================*/

// struct que representa a mensagem */
// Estrutura: quantidade de aminoácidos, solicitação ou reposta e lista com aminoácidos solicitados */
typedef struct {
    uint8_t size;
    char method;
    char payload[5];
} aatp_msg;

// Retorna a lista de IPs presentes no arquivo de configuração 
/*void get_ips() {

	FILE *config_file;
	char each_ip[CLIENTS][20];
	config_file = fopen("config.txt", "r"); 
	if (config_file == NULL) {
		perror("Erro ao abrir o arquivo\n");
		exit(EXIT_FAILURE);
	}

	int i=0;
	//pega todos os IPs
	while(fgets(each_ip[i], CLIENTS, config_file) != NULL) {
		//ips[CLIENTS][i] = each_ip[i];
		printf("%s", each_ip[i]);
		i++;
	}
	//fclose(config_file); 
}*/

// Número aleatório de aminoácidos a serem solicitados
int random_quantity() {
	int num;	
	num = 1 + (rand() % 5);
	return num;
}


/*==================================================
Implementação do cliente
====================================================*/

// Será criado um cliente (thread) para cada aplicação dos colegas 
void *client_tasks(void *param) {
	sleep(1);
	int aux = *(int *)param;
	conn_active[aux] = 1;
	while(conn_active[aux]) {
		// Atributos do cliente
		char *rem_hostname;
		int rem_port;
		int rem_sockfd;
		// Estrutura: familia + endereco IP + porta
		struct sockaddr_in rem_addr;

		// Construcao da estrutura do endereco local
		// Preenchendo a estrutura socket loc_addr (família, IP, porta)
		//pthread_mutex_lock(&lock_resource);
		rem_hostname = (each_ip[aux]);
		//pthread_mutex_unlock(&lock_resource);
//rem_hostname = "127.0.0.1";
//printf("IP: %c", rem_hostname);
		rem_port = PORT;
		rem_addr.sin_family = AF_INET; // familia do protocolo
		rem_addr.sin_addr.s_addr = inet_addr(rem_hostname); // endereco IP local 
		rem_addr.sin_port = htons(rem_port); // porta local  

		// Cria o socket para enviar e receber pacotes 
		// parametros(familia, tipo, protocolo) 
		rem_sockfd = socket(AF_INET, SOCK_STREAM, 0);
		
		if (rem_sockfd < 0) {
			perror("Criando stream socket");
			exit(1);
		}
//printf("> Conectando ao servidor '%s:%d'\n", rem_hostname, rem_port);

		// Estabelece uma conexão remota 
		// parametros(descritor socket, estrutura do endereco remoto, comprimento do endereco) 
		if (connect(rem_sockfd, (struct sockaddr *) &rem_addr, sizeof(rem_addr)) < 0) {
			conn_active[aux] = 0;
			perror("Conectando stream socket");
			exit(1);
		} 
		else {

			/*========================

			Envio da solicitação

			==========================*/

			int quantity = random_quantity(); //em torno de 4000 solicitações necessárias para completar
			//int quantity = 5; //em torno de 2500 solicitações necessárias para completar
			printf("\n=======================================\n");
			printf("Estou solicitando | %d | aminoácidos\n", quantity);
			printf("=======================================\n");

			// Inicializando mensagem
			aatp_msg m = { 0 };
			// Preenchendo dados 
			m.method = 'S'; // Solicitação
			m.size = quantity;
			// Zerando payload
			memset(&m.payload, 0, sizeof m.payload);

			// Enviando solicitação
			send(rem_sockfd, &m, sizeof(m), 0);

			/*========================

			Recebimento da resposta

			==========================*/

			// Criando buffer 
			aatp_msg recv_buffer;
			size_t recv_buffer_len = sizeof(recv_buffer);
			// Recebendo mensagem 
			recv(rem_sockfd, &recv_buffer, recv_buffer_len, 0);
			// Imprimindo resultados 

			/*=====Garantindo exclusão mútua dos recebimentos com uso de mutex=====*/

			//Início da seção crítica
			pthread_mutex_lock(&lock_resource);

			// Indica o recebimento dos aminoácidos
			int q, q_solicitations;
			printf("\n------------------\n");
			for(q=0; q < recv_buffer.size; q++) {
					printf("| Recebi --->  %c |\n", recv_buffer.payload[q]);
			}
			printf("------------------\n");
			
			/*========================

			Comparando "matches" e adicionando no arquivo

			==========================*/

			char sequence[] = "MKWVTFISLLFLFSSAYSRGVFRRDAHKSEVAHRFKDLGEENFKALVLIAFAQYLQQCPFEDHVKLVNEVTEFAKTCVADESAENCDKSLHTLFGDKLCTVATLRETYGEMADCCAKQEPERNECFLQHKDDNPNLPRLVRPEVDVMCTAFHDNEETFLKKYLYEIARRHPYFYAPELLFFAKRYKAAFTECCQAADKAACLLPKLDELRDEGKASSAKQGLKCASLQKFGERAFKAWAVARLSQRFPKAEFAEVSKLVTDLTKVHTECCHGDLLECADDRADLAKYICENQDSISSKLKECCEKPLLEKSHCIAEVENDEMPADLPSLAADFVGSKDVCKNYAEAKDVFLGMFLYEYARRHPDYSVVLLLRLAKTYETTLEKCCAAADPHECYAKVFDEFKPLVEEPQNLIKQNCELFEQLGEYKFQNALLVRYTKKVPQVSTPTLVEVSRNLGKVGSKCCKHPEAKRMPCAEDCLSVFLNQLCVLHEKTPVSDRVTKCCTESLVNGRPCFSALEVDETYVPKEFNAETFTFHADICTLSEKERQIKKQTALVELVKHKPKATKEQLKAVMDDFAAFVEKCCKADDKETCFAEEGKKLVAASQAALGL";

			if((sequence[count]==recv_buffer.payload[0])||
				(sequence[count]==recv_buffer.payload[1])||
				(sequence[count]==recv_buffer.payload[2])||
				(sequence[count]==recv_buffer.payload[3])||
				(sequence[count]==recv_buffer.payload[4]))
			{
				FILE *file; 
				file = fopen("appending.txt","a");
				if(file == NULL){
					printf("Erro ao abrir o arquivo.\n");
					exit(EXIT_FAILURE);
				}
				fprintf(file,"%c", sequence[count]);
				fclose(file);
				count++;
			}

			// Incrementa contador de solicitações
			q_solicitations ++;
			
			// Se chegou aqui é porque completou a sequência!!
			if(count==PROTEIN_SIZE){
				printf("\n=====================================================\n");
				printf("Proteína completa! Todos os aminoácidos foram adicionados!\n");
				printf("\n-----> Original: \n");
				puts(sequence);
				printf("\n------> Resultado: \n");

				//Lendo arquivo para confirmar a sequência
				char letter;
				FILE *result;
				result = fopen("appending.txt", "r"); 
				if (result == NULL) {
					perror("Erro ao abrir o arquivo\n");
					exit(EXIT_FAILURE);
				}
				
				while((letter = fgetc(result)) != EOF)
					printf("%c", letter);
				
				fclose(result);
				//Fim da leitura do arquivo

				printf("\n=====================================================\n");
				printf("\n-----> Resultado no arquivo appending.txt");
				printf("\n-----> Você pode comparar os arquivos original_seq.txt e appending.txt para verificar! \n");
				printf("\n=====================================================\n");
				// Mostra a quantidade de solicitações realizadas
				printf("Quantidade de solicitações: %d\n",q_solicitations);
				exit(1);
			}
		}

			//Fim da seção crítica
			pthread_mutex_unlock(&lock_resource);
			close(rem_sockfd);

			//conn_active[aux] = 0;
	}
	
}

int main() {

	srand(time(NULL));
	pthread_mutex_init(&lock_resource, NULL);	

	// Arquivo de configuração com todos os IPs será lido
	FILE *config_file;
	config_file = fopen("config.txt", "r"); 
	if (config_file == NULL) {
		perror("Erro ao abrir o arquivo\n");
		exit(EXIT_FAILURE);
	}

	int i=0;
	// Pega todos os IPs
	while(fgets(each_ip[i], CLIENTS, config_file) != NULL) {
		printf("%s", each_ip[i]);
		pthread_create(&t_client[i], NULL, client_tasks, &i);
		sleep(2);
		i++;
	}

	/*int i;
    for(i=0; i<1; i++) {
			pthread_create(&t_client[i], NULL, client_tasks, &i);
   	}*/
	
	for(i=0; i<1; i++) {
		pthread_join(t_client[i],&t_client_join);
	}

}