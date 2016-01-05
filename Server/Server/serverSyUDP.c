#include <time.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <ctype.h>
#include "data.h"

#include "serverSyUDP.h"

#pragma comment(lib, "Ws2_32.lib")

struct request req;
SOCKET ConnSocket;
struct sockaddr *sockadd_ip6_local = NULL;
static struct sockaddr_in6 remoteAddr;

int pruef = 1;
FD_SET fds;
TIMEVAL timeval;


int initServer(char *MCAdress, char *Port) {

	char buffer[128];
	
	int trueValue = 1, loopback = 0; //setsockopt
	int val, i = 0;
	int addr_len,b;
	struct ipv6_mreq mreq; //multicastaddress
	struct addrinfo *resultLocalAddress = NULL, *resultMulticastAddress = NULL, *ptr = NULL, hints;

	WSADATA wsaData;
	WORD wVersionRequested;

	wVersionRequested = MAKEWORD(2, 1);
	if (WSAStartup(wVersionRequested, &wsaData) == SOCKET_ERROR) {
		printf("SERVER:		WSAStartup() failed!\n");
		printf("			error code:  %d\n", WSAGetLastError());
		exit(-1);
	}

	//***************Create Socket***************

	ConnSocket = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);

	if (ConnSocket <0) {
		fprintf(stderr, "Client: Error Opening socket: Error %d\n", WSAGetLastError());
		WSACleanup();
		exit(-1);
	}

	/*Init Socket*/
	/*Reusing port for several server...local*/

	setsockopt(ConnSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&trueValue, sizeof(trueValue));

	/*Resolve mc group address to join mc group*/

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_flags = AI_NUMERICHOST;

	if (getaddrinfo(MCAdress, NULL, &hints, &resultMulticastAddress) != 0) {
		fprintf(stderr, "getaddrinfo MCAddress failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		exit(-1);
	}
	/*Resolve local address (anyaddress) to bind*/
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;

	hints.ai_flags = AI_PASSIVE; // localhost

	val = getaddrinfo(NULL, Port, &hints, &resultLocalAddress);
	
	InetNtop(AF_INET6, resultMulticastAddress, buffer, 128);
	printf("MC:Adresse: %s\n",buffer);

	printf("getaddrinfor succeeded\n");
	if (val != 0) {
		printf("getaddrinfo localAddress failed with error: %d\n");
		WSACleanup();
		exit(-1);
	}

	// Retrieve the address an print out the hex bytes

	for (ptr = resultLocalAddress; ptr != NULL; ptr = ptr->ai_next) {
		printf("getaddrinfo response %d\n", i++);
		printf("\tFlags: 0x%x\n", ptr->ai_flags);
		printf("\tFamily: ");

		switch (ptr->ai_family) {

		case AF_UNSPEC:printf("Unspecified\n"); break;
		case AF_INET:printf("AF_INET (IPv4)"); break;
		case AF_INET6:
			printf("AF_INET6 (IPv6)");

			sockadd_ip6_local = (struct sockaddr*) ptr->ai_addr;
			addr_len = ptr->ai_addrlen;
			break;
		default: printf("Other %ld\n", ptr->ai_family); break;
		}
	}

	/*Bind Socket*/
	printf("in bind\n");
	b = bind(ConnSocket, sockadd_ip6_local, addr_len);
	if (b == SOCKET_ERROR) {
		fprintf(stderr, "bind() failed: error %d\n", WSAGetLastError);
		WSACleanup();
		exit(-1);
	}

	/*Specify the multicast group*/
	memcpy(&mreq.ipv6mr_multiaddr, &((struct sockaddr_in6*)(resultMulticastAddress->ai_addr))->sin6_addr, sizeof(mreq.ipv6mr_multiaddr));

	/*Accept multicast from any Interface*/
	mreq.ipv6mr_interface = 1;

	/*Join the mc address */
	if (setsockopt(ConnSocket, IPPROTO_IPV6, IPV6_JOIN_GROUP, (char*)&mreq, sizeof(mreq)) != 0) {
		fprintf(stderr, "setsockopt(IPV6_JOIN_GROUP) failed %d\n");
		WSACleanup();
		
		exit(-1);
	}
	freeaddrinfo(resultLocalAddress);
	freeaddrinfo(resultMulticastAddress);
	return 0;
}

void listenToSock() {
	timeval.tv_sec = 3; //Sekunden
	timeval.tv_usec = 0; //Microsekunden des Intervalls
	FD_ZERO(&fds);
	FD_SET(ConnSocket, &fds);
	pruef = 0;
	pruef = select(ConnSocket, &fds, NULL, NULL, &timeval);
	switch (pruef)
	{
	case 0: printf("Timeout!\n"); break;
	case -1: printf("error");
	default:
		printf("Habn Packet bekommen!\n");
		Sleep(100);
		break;
	}
}

struct request *getRequest() {
	static long seq_number = 0;
	int recvcc;
	int remoteAddrSize = sizeof(struct sockaddr_in6);
	//Ausgabevariablen fuer die Adressinformationen [Optional]
	char info[128];

	/*Receive a message from a socket*/
	
	recvcc = recvfrom(ConnSocket, (char*)&req, sizeof(req), 0, (struct sockaddr*)&remoteAddr, &remoteAddrSize);
	inet_ntop(AF_INET6, &remoteAddr, info, 128);
	//printf("Adresse des Client: %s\n",info);
	if (recvcc == SOCKET_ERROR) {
		fprintf(stderr, "recv() failed: error %d\n",WSAGetLastError());
		closesocket(ConnSocket);
		WSACleanup();
		exit(-1);
	}

	if (recvcc == 0) {
		printf("Client closed connection\n");
		closesocket(ConnSocket);
		WSACleanup();
		exit(-1);
	}

	return &req;

}

void sendAnswer(struct answer *answ) {
	/*Send NACK back to Unicast Address*/
	int w;
	answ->AnswType = AnswNACK;
	w = sendto(ConnSocket, (struct answer*)answ, sizeof(*answ), 0, (struct sockaddr *)&remoteAddr, sizeof(remoteAddr));
	if (w == SOCKET_ERROR) {
		fprintf(stderr, "send() failed: error %d\n");
	}

}


int exitServer() {
	/*Close Socket*/
	closesocket(ConnSocket);
	printf("in exit server\n");

	if (WSACleanup() == SOCKET_ERROR) {
		printf("SERVER: WSACleanup() failed!\n");
		printf("		error code:		%d", WSAGetLastError());
		exit(-1);
	}

	return 0;
}

