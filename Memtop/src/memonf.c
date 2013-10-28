/*
 * src/memonf.c
 *
 * Author: Martinasso Maxime
 *
 * (C) Copyright 2007 INRIA 
 * Projet: MESCAL / ANR NUMASIS
 * 
 */
#include <string.h>
#include <stdlib.h>
#include "memon.h"

void finalize_memon__() {
	finalize_memon();
}

void init_memon__() {
	init_memon();
}

void attach_memory__(long* pid, char *name_f, void *ptr, long* size) {
	char *name;
	int strsize;
	strsize = strlen(name_f)+1;
	name = malloc(strsize*sizeof(char));
	strncpy(name,name_f,strsize-1);
	name[strsize-1] = '\0';

	attach_memory((int)*pid,name,ptr,(size_t)*size);

	free(name);
}


void detach_memory__(long* pid, char *name_f, void *ptr) {
	char *name;
	int strsize;
	strsize = strlen(name_f)+1;
	name = malloc(strsize*sizeof(char));
	strncpy(name,name_f,strsize-1);
	name[strsize-1] = '\0';

	detach_memory((int)*pid,name,ptr);

	free(name);
}

