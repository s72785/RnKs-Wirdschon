#define _CRT_SECURE_NO_WARNINGS
#include <WinSock2.h>
#include <stdio.h>
#include <WS2tcpip.h>
#include "data.h"
#include "toUDP.h"
#include <stdlib.h>
#include "ClientSy.h"
#include "config.h"
//Timer geaddet----------
#include <time.h>
#include "Timer.h"
//-----------------------
#pragma comment (lib, "Ws2_32.lib")

#define MAX_MC_RECEIVER 3
#define true 1
#define false 0

struct request req;
struct request buffer[200];
struct request sendbuffer[200];
int isHelloSended = 0;
int senddata = 0;


void Usage(char *ProgName) {
	fprintf(stderr, P_MESSAGE_1);
	fprintf(stderr, P_MESSAGE_6, ProgName);
	fprintf(stderr, P_MESSAGE_7, (DEFAULT_SERVER == NULL) ? "loopback address" : DEFAULT_SERVER);
	fprintf(stderr, P_MESSAGE_8, DEFAULT_PORT);
	fprintf(stderr, P_MESSAGE_9);
	exit(1);
}

int printAnswer(struct answer *answPtr) {


	switch (answPtr->AnswType) {
	case AnswHello:
		printf("Answer Hello");
		break;
	case AnswOk:
		printf("Answer Ok: Packet acknowledged No: %u ", answPtr->SeNo);
		break;
	case AnswNACK:
		printf("Answer NACK : Packet negative acknowledged No: %u", answPtr->SeNo);
		break;
	case AnswClose:
		printf("Answer Close");
		break;
	case AnswErr:
		printf("Answer Error: %s", errorTable[answPtr->ErrNo]);
		break;
	default:
		printf("Illegal Answer");
		break;
	}; /* switch */
	puts("\n");
	return answPtr->AnswType;
} /* printAnswer */

int main(int argc, char *argv[]) {

	
	long i;
	int nextSN = 0;
	int base = 0;
	int F = 2;
	int tempposi=0;
	
	
	//--------------
	char *Server = DEFAULT_SERVER;
	char *Filename = "";
	char *Port = DEFAULT_PORT;
	char *Window_size = DEFAULT_WINDOW;
	//Timerhandling
	struct timeouts *timelist = NULL;
	int intervall = 1000; //1000millisekunden

	struct request req;
	struct answer ans;
	int isDropped = false;


	FILE *fp;

	if (argc > 1) {
		for (i = 1; i < argc; i++) {
			if (((argv[i][0] == '-') || (argv[i][0] == '/')) && (argv[i][1] != 0) && (argv[i][2] == 0)) {
				switch (tolower(argv[i][1]))
				{
				case 'a'://Server address
					if (argv[i + 1]) {
						if (argv[i + 1][0] != '-') {
							Server = argv[++i];
							break;
						}
					}
					Usage(argv[0]);
					break;
				case 'p': //server port
					if (argv[i + 1]) {
						if (argv[i + 1][0] != '-') {
							Port = argv[++i];
							break;
						}
					}
					Usage(argv[0]);
					break;
				case 'f': //file name
					if (argv[i + 1]) {
						if (argv[i + 1][0] != '-') {
							Filename = argv[++i];
							break;
						}
					}
					Usage(argv[0]);
					break;
				case 'w': //window size
					if (argv[i + 1]) {
						if (argv[i + 1][0] != '-') {
							Window_size = argv[++i];
							break;
						}
					}
					Usage(argv[0]);
					break;
				default:
					Usage(argv[0]);
					break;
				}
			}
			else Usage(argv[0]);
		}
	}

	initClient(Server, Port);
	File *f = newFile("test.txt");
	
	while (1) {

		
		fromApp(f, &req, nextSN,&tempposi);
	
		//Fenster 1 ---------------------------------------------
		if (nextSN < base + F) {
			printf("Sending Packet: %d\n", nextSN);
			if (nextSN == 1) {
				isDropped = true;
			}
			else isDropped = false;
			timelist = sendData(&req, timelist, isDropped);
			listenToSock();
			//NACKRECEIVE() + DEL_TIMER
			decrement_timer(timelist);
			nextSN++;
		}//-------------------------------------------------------WAIT for TIMEOUT 3 TIMES

		if (nextSN >= base + F) {
			if (timelist->timer == 0) {
				timelist = del_timer(timelist, timelist->seq_nr);
				base++;
			}
			else {
				decrement_timer(timelist);
				listenToSock();
			}
		}
				
		

	
		
	}

	
			
	
	
	getchar();
	getchar();
	return 0;
}