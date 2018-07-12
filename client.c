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

// TODO: Alterar para quantidade de hosts (alunos) conforme quantos forem apresentar */
#define CLIENTS 13
#define PORT 1338
#define PROTEIN_SIZE 609
void *t_client_join;
pthread_t t_client[CLIENTS];
pthread_mutex_t lock_resource;
//char sequence[] = "MKWVTFISLLFLFSSAYSRGVFRRDAHKSEVAHRFKDLGEENFKALVLIAFAQYLQQCPFEDHVKLVNEVTEFAKTCVADESAENCDKSLHTLFGDKLCTVATLRETYGEMADCCAKQEPERNECFLQHKDDNPNLPRLVRPEVDVMCTAFHDNEETFLKKYLYEIARRHPYFYAPELLFFAKRYKAAFTECCQAADKAACLLPKLDELRDEGKASSAKQGLKCASLQKFGERAFKAWAVARLSQRFPKAEFAEVSKLVTDLTKVHTECCHGDLLECADDRADLAKYICENQDSISSKLKECCEKPLLEKSHCIAEVENDEMPADLPSLAADFVGSKDVCKNYAEAKDVFLGMFLYEYARRHPDYSVVLLLRLAKTYETTLEKCCAAADPHECYAKVFDEFKPLVEEPQNLIKQNCELFEQLGEYKFQNALLVRYTKKVPQVSTPTLVEVSRNLGKVGSKCCKHPEAKRMPCAEDCLSVFLNQLCVLHEKTPVSDRVTKCCTESLVNGRPCFSALEVDETYVPKEFNAETFTFHADICTLSEKERQIKKQTALVELVKHKPKATKEQLKAVMDDFAAFVEKCCKADDKETCFAEEGKKLVAASQAALGL";	
int count = 0;

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
	char each_line[CLIENTS][15];

	if(!(fopen("config.txt", "r"))) {
		printf("Erro ao abrir o arquivo!");
		return 1;
	}

	int i=0;
	//pega todos os IPs
	while(fgets(each_line[i], CLIENTS, config_file) != NULL) {
		ips[CLIENTS][i] = each_line[i];
		printf("%s", each_line[i]);
		i++;
	}
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
void *client_tasks() {

	while(1) {
		// Atributos do cliente
		char *rem_hostname;
		int rem_port;
		int rem_sockfd;
		// Estrutura: familia + endereco IP + porta
		struct sockaddr_in rem_addr;

		/*if (argc != 3) {
			//printf("Parametros: <remote_port> \n");
			printf("Parametros: <remote_host> <remote_port> \n");
			exit(1);
		}*/

		/*rem_hostname = argv[1];
		rem_port = atoi(argv[2]);*/

		// Construcao da estrutura do endereco local
		// Preenchendo a estrutura socket loc_addr (família, IP, porta)
		//int k;
		//for(k=0; k < CLIENTS; k++) {
		rem_hostname = "127.0.0.1";
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
			perror("Conectando stream socket");
			exit(1);
		}

		/*========================
		Envio da solicitação
		==========================*/

//int quantity = random_quantity();
		int quantity = 5;
		printf("\n=======================================\n");
		printf("Estou solicitando | %d | aminoácidos\n", quantity);
		printf("=======================================\n\n");

		//int z = 0;
		//for(z = 0; z < quantity; z++) {
			// Inicializando mensagem
			aatp_msg m = { 0 };
			// Preenchendo dados 
			m.method = 'S'; // Solicitação
//m.size = quantity;
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

			int q;
			printf("\n\t====================================\n");
			for(q=0; q < recv_buffer.size; q++) {
					printf("\n\t Recebi --->  %c \n\n", recv_buffer.payload[q]);
					//printf("\t== Method: %c ==\n", recv_buffer.method);
					//printf("\t== Size: %d ==\n", recv_buffer.size);
					//printf("\t== Payload: %c ==\n", recv_buffer.payload[q]);
					/*if((sequence[q]==recv_buffer.payload[0])||(sequence[q]==recv_buffer.payload[1])||(sequence[q]==recv_buffer.payload[2])||(sequence[q]==recv_buffer.payload[3])||(sequence[q]==recv_buffer.payload[4])){
				
						FILE *file; 
						file = fopen("appending.txt","a");
						if(file==NULL){
							printf("Não foi possível abrir o arquivo.\n");
							getchar();
							exit(0);
						}
						fprintf(file,"%c", sequence[q]);
						fclose(file);
						q++;
					}*/
			}
			printf("\t====================================\n");

			/*======================================
			Gerenciamento dos aminoácidos recebidos
			=======================================*/

			/*char *copy[PROTEIN_SIZE+1];
			//copy = malloc(PROTEIN_SIZE+1);
			char sequence[] = "MKWVTFISLLFLFSSAYSRGVFRRDAHKSEVAHRFKDLGEENFKALVLIAFAQYLQQCPFEDHVKLVNEVTEFAKTCVADESAENCDKSLHTLFGDKLCTVATLRETYGEMADCCAKQEPERNECFLQHKDDNPNLPRLVRPEVDVMCTAFHDNEETFLKKYLYEIARRHPYFYAPELLFFAKRYKAAFTECCQAADKAACLLPKLDELRDEGKASSAKQGLKCASLQKFGERAFKAWAVARLSQRFPKAEFAEVSKLVTDLTKVHTECCHGDLLECADDRADLAKYICENQDSISSKLKECCEKPLLEKSHCIAEVENDEMPADLPSLAADFVGSKDVCKNYAEAKDVFLGMFLYEYARRHPDYSVVLLLRLAKTYETTLEKCCAAADPHECYAKVFDEFKPLVEEPQNLIKQNCELFEQLGEYKFQNALLVRYTKKVPQVSTPTLVEVSRNLGKVGSKCCKHPEAKRMPCAEDCLSVFLNQLCVLHEKTPVSDRVTKCCTESLVNGRPCFSALEVDETYVPKEFNAETFTFHADICTLSEKERQIKKQTALVELVKHKPKATKEQLKAVMDDFAAFVEKCCKADDKETCFAEEGKKLVAASQAALGL";
			//puts(sequence);
			strcpy(copy, sequence);
			//puts(copy);
			char appending[PROTEIN_SIZE];
			int changed[PROTEIN_SIZE];
			int additions = 0;
			int step = 0;

			// Varre a proteína sequencialmente verificando se os valores enviados estão na sequência
			for(c=0; c < PROTEIN_SIZE; c++) {
			
				for(q=step; q < recv_buffer.size + step; q++) {
					for(r=step; r < recv_buffer.size + step; r++) {
						
						if(sequence[q] == recv_buffer.payload[r] && sequence[q] != '-' && changed[q] != 1) {
							appending[q] = recv_buffer.payload[r];
							changed[q] = 1;
							sequence[q] = '-';
							//strcpy(sequence, '-');
							printf("\nAchei %c na sequencia na posição %d \n", recv_buffer.payload[r], q);
							puts(sequence);
							additions++;
								
						}
						step = additions;
					}
				}
				
			}*/
			
			char sequence[] = "MKWVTFISLLFLFSSAYSRGVFRRDAHKSEVAHRFKDLGEENFKALVLIAFAQYLQQCPFEDHVKLVNEVTEFAKTCVADESAENCDKSLHTLFGDKLCTVATLRETYGEMADCCAKQEPERNECFLQHKDDNPNLPRLVRPEVDVMCTAFHDNEETFLKKYLYEIARRHPYFYAPELLFFAKRYKAAFTECCQAADKAACLLPKLDELRDEGKASSAKQGLKCASLQKFGERAFKAWAVARLSQRFPKAEFAEVSKLVTDLTKVHTECCHGDLLECADDRADLAKYICENQDSISSKLKECCEKPLLEKSHCIAEVENDEMPADLPSLAADFVGSKDVCKNYAEAKDVFLGMFLYEYARRHPDYSVVLLLRLAKTYETTLEKCCAAADPHECYAKVFDEFKPLVEEPQNLIKQNCELFEQLGEYKFQNALLVRYTKKVPQVSTPTLVEVSRNLGKVGSKCCKHPEAKRMPCAEDCLSVFLNQLCVLHEKTPVSDRVTKCCTESLVNGRPCFSALEVDETYVPKEFNAETFTFHADICTLSEKERQIKKQTALVELVKHKPKATKEQLKAVMDDFAAFVEKCCKADDKETCFAEEGKKLVAASQAALGL";

			if((sequence[count]==recv_buffer.payload[0])||(sequence[count]==recv_buffer.payload[1])||(sequence[count]==recv_buffer.payload[2])||(sequence[count]==recv_buffer.payload[3])||(sequence[count]==recv_buffer.payload[4])){
				FILE *file; 
				file = fopen("appending.txt","a");
				if(file == NULL){
					printf("Não foi possível abrir o arquivo.\n");
					getchar();
					exit(0);
				}
				fprintf(file,"%c", sequence[count]);
				fclose(file);
				count++;

			}
			
			if(count==PROTEIN_SIZE){
				printf("Proteína completa!!");
				exit(1);
			}

			//Fim da seção crítica
			pthread_mutex_unlock(&lock_resource);
		
		close(rem_sockfd);
		
	}
	
}

//int main(int argc, char *argv[]) {
int main() {

	srand(time(NULL));
	pthread_mutex_init(&lock_resource, NULL);	

	int i;
    	for(i=0; i<1; i++) {
		pthread_create(&t_client[i], NULL, client_tasks, NULL);
   	}
	
	for(i=0; i<1; i++) {
		pthread_join(t_client[i],&t_client_join);
	}

}
