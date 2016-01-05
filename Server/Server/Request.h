typedef struct Request {
	unsigned char ReqType;
#define ReqHello 'H'				//HELLO
#define ReqData	 'D'				//ReqData
#define ReqClose 'C'				//ReqClose
	long FlNr;						//Data length line of text in byte
	unsigned long SeNr;				//Sequenz Number (==0) beginn of file
#define PufferSize 256
	char		  name[PufferSize];	//Data--> line of Text
};



Request *newRequest();
