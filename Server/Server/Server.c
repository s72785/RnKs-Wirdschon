#define _CRT_SECURE_NO_WARNINGS
#include "data.h"
#include "packet.h"
#include "toUdp.h"			
#include <dos.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <winsock2.h>
#include "serverSyUDP.h"	
#define MAXWINDOW 10

int main(int argc, char *argv[]) {

	struct request *req;
	struct answer *ans;
	
	struct packet packet[200];

	char *buffer;
	//Diagram vars
	int base = 0;
	int F = 2;
	struct packet tmp[MAXWINDOW];


	//Speicher allocation aller variablen
	buffer = malloc(PufferSize);
	req = malloc(sizeof(req));
	//Initialisierung des Servers
	initServer(DEFAULT_SERVER, DEFAULT_PORT);
	
	//Rückgabe einer Referenz vom Typ struct request
	while (1) {
		
		listenToSock();
		
		req = getRequest();
		
		if (base < req->SeNr< base + F) {
			 tmp[base].req = *req;
			 printf("Packet Buffered: %d\n", tmp[base].req.SeNr);
			//buffer packet
			 if (req->SeNr !=  base) {
				 printf("Hier Nack senden!\n");
				 //sendNACK();
			 }
			 else {
				 packet[base].req = *req;
				 printf("Delivered to data layer: %d\n", packet[base].req.SeNr);
				 base = base++;				
			 }
		}




		
		
		







		/*
		if (req->ReqType == 'D') {
			snprintf(buffer, PufferSize + 1, req->name);
			printf("\nRequest Type: %c\n", req->ReqType);
			printf("Sequenze Number: %d\n", req->SeNr);
			//pruefeSequenzNumber(&req);
			printf("%s\n", buffer);
			memset(buffer, 0, sizeof buffer);
		}*/
	
	
	}
	
	
	

	getchar();
	getchar();
	return 0;

}