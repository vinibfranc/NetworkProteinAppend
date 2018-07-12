/*
	Vinícius Franceschi
	Informática Biomédica - UFCSPA
	Redes de Computadores
 */

/* Servidor aguarda solicitações de aminoácidos dos clientes */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <pthread.h>
#include <time.h>

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

// Retorna um aminoácido aleatório para enviar ao cliente
char randomize_aminoacid() {

	int aminoacids_qtd = 20;
	char aminoacids[] = {'A', 'R', 'N', 'D', 'C', 'E', 'Q', 'G', 'H', 'I', 'L', 'K', 'M', 'F', 'P', 'S', 'T', 'W','Y', 'V'};
	int j, num;	
	num = 0 + (rand() % 20); 
	//printf("\nNumero sorteado: %d -> Aminoácido: %c\n", num, aminoacids[num]);
		
	return aminoacids[num]; 
}


/*==================================================
Implementação do servidor
====================================================*/

int main(int argc, char *argv[]) {

	// Inicializando semente para números aleatórios
	srand(time(NULL));

	// Variaveis Locais 
	int i;
	int loc_sockfd, loc_newsockfd, tamanho;
	

	// Estrutura: familia + endereco IP + porta 
	struct sockaddr_in loc_addr;
	
	if (argc != 2) {
		printf("Parametros: <local_port> \n");
		exit(1);
	}

   	// Cria o socket para enviar e receber pacotes
	// parametros(familia, tipo, protocolo) 
	loc_sockfd = socket(AF_INET, SOCK_STREAM, 0);	
	
	if (loc_sockfd < 0) {
		perror("Criando stream socket");
		exit(1);
	}

	// Construcao da estrutura do endereco local
	// Preenchendo a estrutura socket loc_addr (família, IP, porta)
	loc_addr.sin_family = AF_INET; // familia do protocolo
	loc_addr.sin_addr.s_addr = INADDR_ANY; // endereco IP local 
	loc_addr.sin_port = htons(atoi(argv[1])); // porta local 
	bzero(&(loc_addr.sin_zero), 8);

   	// Bind para o endereco local
	// parametros(descritor socket, estrutura do endereco local, comprimento do endereco)
	if (bind(loc_sockfd, (struct sockaddr *) &loc_addr, sizeof(struct sockaddr)) < 0) {
		perror("Ligando stream socket");
		exit(1);
	}
	
	// parametros(descritor socket, numeros de conexões em espera sem serem aceitos pelo accept)
	
	// Loop infinito para espera de conexões
	printf(">Esperando uma conexao\n");
	while(1) {
		listen(loc_sockfd, 5);
		
		int sock_size = sizeof(struct sockaddr_in);

		/*===================================================================
		Preparação da estrutura do protocolo para recebimento da solicitação
		=====================================================================*/

		// A mensagem recebida em bits é estruturada com o buffer do protocolo definido	
		aatp_msg recv_buffer;

	   	// Accept permite aceitar um pedido de conexao, devolve um novo "socket" ja ligado ao emissor do pedido e o "socket" original
		// parametros(descritor socket, estrutura do endereco local, comprimento do endereco)
		loc_newsockfd =	accept(loc_sockfd, (struct sockaddr *)&loc_addr, &sock_size);

		// Pega a quantidade de aminoácidos enviada
		size_t buffer_len = sizeof(recv_buffer);
		// parametros(descritor socket, endereco da memoria, tamanho da memoria, flag)
		recv(loc_newsockfd, &recv_buffer, buffer_len, 0);
		printf("\n=================================\n");
		printf("Recebendo solicitação de | %d | aminoácidos\n", recv_buffer.size);
		printf("=================================\n\n");

		/*==================================================
		Resposta com a quantidade de aminoácidos solicitados
		====================================================*/

		// Inicializando mensagem
		aatp_msg m = { 0 };
		// Preenchendo dados
		m.method = 'R'; // Resposta
		m.size = recv_buffer.size; 
		// Zerando payload para evitar enviar lixo caso seja feita uma solicitação de menos de 5 aminoácidos 
		memset(&m.payload, 0, sizeof(m.payload));
		
		printf("\n\t====================================\n");
		// Itera de acordo com size recebido pelo cliente 
		for(i=0; i<recv_buffer.size; i++){
			char rand_n = randomize_aminoacid();
			printf("\n\t Enviei --->  %c \n\n", rand_n);
			m.payload[i] = rand_n;
		}
		printf("\t====================================\n");
		
		// Envia a quantidade de aminoácidos solicitada pelo cliente
		// parametros(descritor socket, endereco da memoria, tamanho da memoria, flag) 
		send(loc_newsockfd, &m, sizeof(m), 0);
//printf("\nEnviando aminoácidos\n!!");

		sleep(2);

		close(loc_newsockfd);
			
	}
	
	close(loc_sockfd);
}

	/* código antigo */

	//listen(loc_sockfd, 1); 

	//while(1){
		//printf("> Aguardando conexao\n");
		/* Accept permite aceitar um pedido de conexao, devolve um novo "socket" ja ligado ao emissor do pedido e o "socket" original */
		/* parametros(descritor socket, estrutura do endereco local, comprimento do endereco)*/
		//while(loc_newsockfd = accept(loc_sockfd, (struct sockaddr *)&loc_addr, &tamanho)){
		//	printf("aceitou");
		//	recv(loc_newsockfd, &recv_buffer, sizeof(recv_buffer), 0);
			//loc_newsockfd =	accept(loc_sockfd, (struct sockaddr *)&loc_addr, &tamanho);
			/* parametros(descritor socket, endereco da memoria, tamanho da memoria, flag) */

			// =============== > agora tu pode manipular o recv_buffer que nele terá as informações que tu precisa!

			/* parametros(descritor socket, endereco da memoria, tamanho da memoria, flag) */ 
			//send(loc_newsockfd, &linha, sizeof(linha), 0);
			//printf("Renvia %s\n", linha);
			//send_aminoacid(1); // Aqui tu não ta enviando o AA pelo socket, tem que por esse valor retornado dentro de um send() ou write()


			/*========================
			Envio do(s) aminoácido(s)
			==========================*/
/*
			if(!fork()){
				close(loc_sockfd);

				recv(loc_newsockfd, &recv_buffer, sizeof(recv_buffer), 0);
				//printf("Recebi %s\n", linha);
				printf("Recebi uma solicitação de aminoácidos!");

				aatp_msg m = { 0 };
				/* Preenchendo dados */
				//m.method = 'R'; /* Resposta */
				//m.size = recv_buffer.size; 
				/* Zerando payload para evitar enviar lixo
				caso seja feita uma solicitação de menos de 5 aminoácidos */
				//memset(&m.payload, 0, sizeof(m.payload));
				
				/* Itera de acordo com size recebido pelo cliente 
				for(i=0; i<recv_buffer.size; i++){
					char rand_n = randomize_aminoacid();
					printf("Aminoácido enviado: %c", rand_n);
					m.payload[i] = rand_n;
				}

				/* Enviando resposta 
				send(loc_newsockfd, &m, sizeof(m),0);
				exit(0);
			}
			close(loc_newsockfd);
			*/
		//}
	//close(loc_sockfd);
	//}
	/* fechamento do socket local */ 
	

