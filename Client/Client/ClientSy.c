#include "ClientSy.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "data.h"
#include <stdio.h>
//Timer geaddet----------
#include <time.h>
#include "Timer.h"
//--------------------------
SOCKET sock;
#define DEFAULT_ADDRESS "FF02::10"
#define PORT 50000
WSADATA wsa;
SOCKADDR_IN6 MCADDRESS;
SOCKADDR_IN6 remote;
int remotelen = sizeof(SOCKADDR_IN6);
char message[256];
struct request req;
struct answer ans;

//TIMER
int pruef = 1;
FD_SET fds;
TIMEVAL timeval;
//TIMEOUTS
struct timeouts *timeouts;
//Position Var
int tempPosi = 1;
//for fails
int drop = 0;
int isDropped = 0;
int tmp = 0;
int *receiveNACK();
int initClient(char *MCAdress, char* Port) {
	u_long on = 1;
		if ((WSAStartup(MAKEWORD(2, 1), &wsa)) != 0) {
		printf("Fail winsock:%d", WSAGetLastError());
		WSACleanup();
		exit(-1);
	}

	//create Socket
	sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);

	if (sock == INVALID_SOCKET) {
		printf("FAIL: %s", WSAGetLastError);
		exit(-1);
	}
	memset((char *)&MCADDRESS, 0, sizeof(MCADDRESS));
	MCADDRESS.sin6_family = AF_INET6;
	MCADDRESS.sin6_port = htons(PORT);

	//schneller beschreiben nice!
	inet_pton(AF_INET6, DEFAULT_ADDRESS, &MCADDRESS.sin6_addr);

    ioctlsocket(sock, FIONBIO, &on);
	return 0;
}


struct timeouts *sendData(struct request *req, struct timeouts *timer, int isDropped) {
	req->ReqType = ReqData;
	if(!isDropped)
	sendto(sock, (struct request*)req, sizeof(*req), 0, (SOCKADDR *)&MCADDRESS, remotelen);
    timer =	add_timer(timer, 3, req->SeNr);	
	return timer;
}

void listenToSock() {
	timeval.tv_sec = 1; //Sekunden
	timeval.tv_usec = 0; //Microsekunden des Intervalls
	FD_ZERO(&fds);
	FD_SET(sock, &fds);
	pruef = 0;
    pruef = select(sock, &fds, NULL, NULL, &timeval);	
	switch (pruef)
	{
	case 0: printf("sending...\n"); break;
	case -1: printf("error");
	default:
		printf("NACK");
		Sleep(250);
		break;
	}
}

void sendHello(struct request *req) {
	int timestat;
	//Der timer ist gut bei...
	//Datensenden alle 300ms
	timeval.tv_usec = 1000000;
	FD_ZERO(&fds);
	FD_SET(sock, &fds);
	pruef = 0;
	
	req->ReqType = ReqHello;
	printf("HELLO SENDED: %c\n", req->ReqType);
	
	sendto(sock, (struct request*)req, sizeof(*req), 0, (SOCKADDR *)&MCADDRESS, remotelen);
	pruef = select(sock, &fds, NULL, NULL, &timeval);
	
	
	switch (pruef) {
	case 0: printf("timeout");break;
	case -1: printf("error");break;
	default: printf("alles tutti");break;
	}

}
void sendClose(struct request *req) {
	
	req->ReqType = ReqClose;
	printf("Close SENDED: %c\n", req->ReqType);

	sendto(sock, (struct request*)req, sizeof(*req), 0, (SOCKADDR *)&MCADDRESS, remotelen);

}
struct answer *receiveHello() {	

	int recvcc = 0;
	char info[128];
	
	recvcc = recvfrom(sock, (char*)&ans, sizeof(ans), 0, &remote, &remotelen);
	
	if (recvcc == SOCKET_ERROR) {
		int ret = WSAGetLastError();
		if (ret != 10035 && ret != 10054) {
			fprintf(stderr, "recv() failed: error %d\n", WSAGetLastError());
			closesocket(sock);
			WSACleanup();
			exit(-1);
		}
	}
	else {
		inet_ntop(AF_INET6,&remote,&info,128);
		printf("MCADDRESS mit IPv6: %s Angemeldet!\n", info);
	}
	
	return &ans;
}

int *receiveNACK() {
	int recvcc = 0;
	char info[128];

	recvcc = recvfrom(sock, (char*)&ans, sizeof(ans), 0, &remote, &remotelen);

	if (recvcc == SOCKET_ERROR) {
		int ret = WSAGetLastError();
		if (ret != 10035 && ret != 10054) {
			fprintf(stderr, "recv() failed: error %d\n", WSAGetLastError());
			closesocket(sock);
			WSACleanup();
			exit(-1);
		}
		else {
			return 0;
		}
	}
	else {
		inet_ntop(AF_INET6, &remote, &info, 128);
		//printf("MCADDRESS mit IPv6: %c Fehlt: %d !\n", ans.AnswType,ans.SeNo);
	}
	
	printf("Nack received: %d\n", ans.SeNo);
	return ans.SeNo;
}


struct request fromApp(File *f,struct request *req, int base,int tempPosi) {
	FILE *fp;
	int i=0;
	f->position = base * 256;
	if (tempPosi == base) {
		//File nicht lesen
	}
	else {
		//Öffne Datei
		fopen_s(&fp, f->name, "rb");
		if (fp == 0) {
			printf("Fehler beim öffnen der Datei!");
		}
		//store data in request buffer
		//search position in File (SEEK_SET==Begin of file)
		fseek(fp, f->position, SEEK_SET);

		for (i = 0; i < PufferSize; i++) {
			int c = fgetc(fp);
			if (c == EOF) {
				req->name[i] = '\0';
				f->endOfFile = 1;
				break;
			}
			else {
				req->name[i] = c;
			}
		}
		tempPosi = base;
		f->position = ftell(fp);
		req->SeNr = base;
		fclose(fp);
	}

	
}
