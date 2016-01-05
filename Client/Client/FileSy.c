#include "FileSy.h"


File *newFile(char *name) {
	File *f = malloc(sizeof(File));
	f->name = malloc(sizeof(name));
	f->name = name;
	f->position = 0;
	f->endOfFile = 0;
	return f;
}

