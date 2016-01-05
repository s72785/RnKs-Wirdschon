typedef struct File {
	int position;
	int endOfFile;
	char *name;
}File;

File *newFile(char *name);