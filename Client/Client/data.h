/*
This code was provided by [Prof. Dr.-Ing S. Kühn](http://www.htw-dresden.de/en/faculty-of-informaticsmathematics/personal/professuren/prof-dr-sabine-kuehn.html)
in December 2015
at HTW Dresden for the purpose of solving the exercise
without any notice of a license or copyright.
All rights reserved, German law applies.
*/

/*Data Declaration*/
/*For Client and Server*/
#ifndef _DATAH_	/* added condition by erlix */
#define _DATAH_

#define TIMEOUT_INT 300
#define TIMEOUT 3
#define MAX_WINDOW 10
#define DEFAULT_WINDOW 10
#define MAX_SEQNR 2*MAX_WINDOW-1

#define MAX_BUFFER 2*MAX_WINDOW

extern char *errorTable[];

struct request {
	unsigned char ReqType;
#define ReqHello 'H'				//HELLO
#define ReqData	 'D'				//ReqData
#define ReqClose 'C'				//ReqClose
	long FlNr;						//Data length line of text in byte
	unsigned long SeNr;				//Sequenz Number (==0) beginn of file
#define PufferSize 256
	char		  name[PufferSize];	//Data--> line of Text
};

struct answer {
	unsigned char AnswType;
#define AnswHello 'H'
#define AnswOk    'O'
#define AnswNACK  'N'			//MC Group receiver sends
								//negative Ack if received packet is not in Order
#define AnswClose 'C'
#define AnswErr   0xFF
	unsigned   FlNr;
	unsigned   SeNo;
#define ErrNo SeNo
};

#endif _DATAH_