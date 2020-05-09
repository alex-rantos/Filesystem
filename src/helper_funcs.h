#ifndef _HELPER_FUNCS_H
#define _HELPER_FUNCS_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

#include "Stack.h"
#include "InodeList.h"

#define COPY_BUFFER 2043

enum FILE_TYPE {
	// E(num).Avoiding name collisions
	E_DIRECTORY,
	E_FILE,
	UNKNOWN
};
char *newStr (char *charBuffer);
char* stradd(const char* a, const char* b);
char *add_chars(char *c1,char *c2);
void delete_char(char *str, int i);
int check_first(char *str, char k);
const char * Get_cwd();
void Recursive_dir_helper(char *name,Stack* stack);
void Recursive_dir(char *name,Stack* stack);

int File_size(const char *path_to_file);
int File_size(FILE *ptrfile);
void Copy_Files(char *src,char *dest);
FILE_TYPE Check_path(char const *path);             // checks given path for it's file-type

void delete_path(std::string path);             // deletes ALL files of given path and removes it.
std::string Get_cwd(std::string file);         // returns absolute path of file
char *Strduplicate(const char *source);       // returns a new pointer of the given string/char *

#endif
