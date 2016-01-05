#include "FileSy.h"

initClient(char * MCAdress, char* Port);
struct request fromApp(File *f,struct request *req, int base, int tempposi);
struct answer *receiveHello();
struct answer *receiveNack();
void listenToSock();
