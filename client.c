#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define SERVERPORT "3490"	// la porta necessaria per il server
#define MYPORT "3491"

#define MAXBUFLEN 100

int main(int argc, char *argv[])
{
	int s , s1;
	struct addrinfo hints, *servinfo, * clientinfo, *p;
	struct sockaddr_storage their_addr;
	int rv;
	int numbytes;
	socklen_t addr_len;
	char buf[MAXBUFLEN];

	if (argc != 3) {
		fprintf(stderr,"usage: talker hostname message\n");
		exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	//Prendo le info del server e verifico se la porta è libera
	if ((rv = getaddrinfo(argv[1], SERVERPORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// Inizializzo socket endpoint 
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((s = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) {
			perror("Errore nel Socket");
			continue;
		}
		break;
	}

	if (p == NULL) {
		fprintf(stderr, "talker: failed to bind socket\n");
		return 2;
	}

	//Invio e controllo
	if ((numbytes = sendto(s, argv[2], strlen(argv[2]), 0,
			 p->ai_addr, p->ai_addrlen)) == -1) {
		perror("Errore nell'invio del messaggio");
		exit(1);
	}
	printf("Inviati %d bytes\n", numbytes);
	printf("Da qui in poi il Client in attesa...\n");
	addr_len = sizeof their_addr;
	if ((numbytes = recvfrom(s, buf, MAXBUFLEN - 1, 0, (struct sockaddr*)&their_addr, &addr_len)) == -1) {
		perror("recvfrom");
		exit(1);
	}
	char ss[INET6_ADDRSTRLEN];
	inet_ntop(their_addr.ss_family, &(((struct sockaddr_in6*)&their_addr)->sin6_addr), ss, sizeof ss);
	printf("-->Pacchetto ricevuto da %s...\n", ss);
	printf("...di lunghezza %d bytes...\n", numbytes);
	buf[numbytes] = '\0';
	printf("...Contentente: \"%s\"...\n", buf);

	//libero tutto
	freeaddrinfo(servinfo);
	close(s);
	/*
	//RIAPRO PORTA PER ASCOLTARE || RIFACCIO TUTTI I PASSAGGI
	//PRIMA VERIFICA: prendo le info del server e verifico se la porta è libera
	if ((rv = getaddrinfo(NULL, MYPORT, &hints, &clientinfo)) != 0) {
		printf("Errore di gettaddrinfo: %s\n", gai_strerror(rv));
		exit(1);
	}

	//SECONDA VERIFICA: verifico se i dati sono coerenti (problemi di firewall? ecc...)
	//p=servinfo è una lista concatenata: per prendere il primo valore devo fare ai_next
	for (p = clientinfo; p != NULL; p = p->ai_next) {
		if ((s = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("Errore nel Socket");
			continue;
		}
		break;
	}

	//TERZA VERIFICA: verifico se il socket è stato aperto (altri errori?)
	if (p == NULL) {
		fprintf(stderr, "Fallita connessione con il socket\n");
		return 2;
	}
	
	//Se tutto ok-->
	//SECONDA PARTE: BIND

		//faccio il "binding". Il bind è un'operazione che associa un socket ad un indirizzo
		//ripasso tutti i risultati. Aggancio il primo disponibile
	for (p = clientinfo; p != NULL; p = p->ai_next) {
		//APERTURA DEL SOCKET IN ASCOLTO (Server)
		if (bind(s, p->ai_addr, p->ai_addrlen) < 0) {
			close(s);
			perror("Errore nel bind");
			continue;
		}
		break;
	}
	//PROBLEMA:: SE CHIUDO SOCKET INFORMAZIONI PRESE CON &THEIRADDRESS NON VALIDE!!!
	//Mi metto in ascolto per il ricevuto
	printf("Da qui in poi il Client in attesa...\n");
	addr_len = sizeof their_addr;
	if ((numbytes = recvfrom(s, buf, MAXBUFLEN - 1, 0, (struct sockaddr*)&their_addr, &addr_len)) == -1) {
		perror("recvfrom");
		exit(1);
	}
	char ss[INET6_ADDRSTRLEN];
	inet_ntop(their_addr.ss_family, &(((struct sockaddr_in6*)&their_addr)->sin6_addr), ss, sizeof ss);
	printf("-->Pacchetto ricevuto da %s...\n", ss);
	printf("...di lunghezza %d bytes...\n", numbytes);
	buf[numbytes] = '\0';
	printf("...Contentente: \"%s\"...\n", buf);
	
	//libero tutto
	freeaddrinfo(servinfo);
	close(s);
	*/
	return 0;
}
