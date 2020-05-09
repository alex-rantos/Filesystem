#include <iostream>

#include <stdio.h>
#include <stdlib.h> // strtol();
#include <unistd.h> // fork()/close();
#include <fcntl.h> // open();
#include <string.h> // memset();
#include <sys/stat.h> // [mkfifo();]
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>

#include "Stack.h"
#include "InodeList.h"
#include "helper_funcs.h"

using namespace std;

////characters manipulation////////////////////

char *newStr(char *charBuffer) {
	int length = strlen(charBuffer);
	char *str;
	if (length <= 1) {
		str = (char *) malloc(1);
		str[0] = '\0';
	} else {
		str = (char *) malloc(length);
		strcpy(str, &charBuffer[1]);
	}
	return str;
	free(str);
}

char* stradd(const char* a, const char* b) {
	size_t len = strlen(a) + strlen(b);
	char *ret = (char*) malloc(len * sizeof(char) + 1);
	*ret = '\0';
	return strcat(strcat(ret, a), b);
	free(ret);
}

char *add_chars(char *c1, char *c2) {
	char* full_path = (char*) malloc(1024 * sizeof(char));
	//full_path = malloc(strlen(c1)+1+4); /* make space for the new string (should check the return value ...) */
	strcpy(full_path, c1); /* copy name into the new var */
	strcat(full_path, c2); /* add the extension */
	return full_path;
	free(full_path);

}

void delete_char(char *str, int i) {
	int len = strlen(str);
	for (; i < len - 1; i++) {
		str[i] = str[i + 1];
	}
	str[i] = '\0';
}

int check_first(char *str, char k) {
	char temp = str[0];
	if (temp == k)
		return 1;
	else
		return 0;
}

const char * Get_cwd() {
	char* cwd;
	char buff[PATH_MAX + 1];
	cwd = getcwd(buff, PATH_MAX + 1);
	if (cwd != NULL) {
		return cwd;
	}
	return "\0";
}

void Recursive_dir_helper(char *name, Stack*stack) {
	char newname[256];
	if (check_first(name, '~') == 1) {
		char basic[256], temp[256];
		strcpy(basic, Get_cwd());
		int i = 0, counter = 0;
		while (i != 3) {
			if (basic[counter] == '/')
				i++;
			counter++;
		}
		strncpy(temp, basic, counter);
		temp[counter] = '\0'; // place the null terminator
		char temp1[256];
		strcpy(temp1, newStr(name));
		strcpy(newname, add_chars(temp, temp1));
	} else if ((check_first(name, '~') == 0) && (check_first(name, '/') == 0)
			&& (check_first(name, '.') == 0)) {

		char basic[256], temp[256];
		strcpy(basic, Get_cwd());
		strcpy(temp, stradd(basic, "/"));
		strcpy(newname, add_chars(temp, name));
		printf("%s\n", newname);

	} else if ((check_first(name, '.') == 1))
		strcpy(newname, newStr(name));
	else
		strcpy(newname, name);

	Recursive_dir(newname, stack);

}

void Recursive_dir(char *newname, Stack*stack) {
	DIR *dir;
	struct dirent *entry;

	if (!(dir = opendir(newname)))
		return;
	if (!(entry = readdir(dir)))
		return;

	do {
		if (entry->d_type == DT_DIR) {
			////////////for directories///////////////
			char path[256];
			int len = snprintf(path, sizeof(path) - 1, "%s/%s", newname,
					entry->d_name); /* building the whole path */
			path[len] = 0;

			if (strcmp(entry->d_name, ".") == 0
					|| strcmp(entry->d_name, "..") == 0)
				continue;
			File_metadata *fmetadata = new File_metadata(path);
			List *list = new List();
			stack->GetLastList()->Push(fmetadata);
			stack->GetLastList()->SetLastList(list);
			stack->Push(list);
			Recursive_dir(path, stack);
		} else {
			/////////////for files//////////////////
			char path1[256];
			int len1 = snprintf(path1, sizeof(path1) - 1, "%s/%s", newname,
					entry->d_name); /* building the whole path */
			path1[len1] = 0;
			File_metadata *fmetadata = new File_metadata(path1);
			stack->GetLastList()->Push(fmetadata);
		}
		entry = readdir(dir);
		if (entry == NULL) { // Read whole dir-break and recurse one level backwards
			stack->Delete_Last();
			break;
		}
	} while (1);
	closedir(dir);
}

int File_size(const char *path_to_file) {
	int size = 0;
	FILE *file;
	if ((file = fopen(path_to_file,"rb")) != NULL) {
	    fseek(file, 0L, SEEK_END);
	    size  = ftell(file);
	    fseek(file, 0L, SEEK_SET);
	} else Psystem_error("Failed to get size from",path_to_file);
	return size;
}

int File_size(FILE *file) {
	int size = 0;
	fseek(file, 0L, SEEK_END);
	size  = ftell(file);
	fseek(file, 0L, SEEK_SET);
	return size;
}

void Copy_Files(char *src_path,char *dest_path) {
	FILE *in,*out;
	size_t n;
	char buffer[COPY_BUFFER];

	if ((in = fopen(src_path,"rb")) != NULL) {
		if ((out = fopen(dest_path,"ab")) != NULL) { // append + binary
			while ((n = fread(buffer, 1, sizeof(buffer), in)) != 0) {
				fwrite(buffer, 1, n, out);
			}
		} else Psystem_error("Copy_Files::Failed to open",dest_path);
	} else Psystem_error("Copy_Files::Failed to open",src_path);
}

FILE_TYPE Check_path(char const* p) {
	struct stat sb;
	if (stat(p,&sb) == 0) {
		if (S_ISDIR(sb.st_mode))
			return E_DIRECTORY;
		if (S_ISREG(sb.st_mode) || S_ISLNK(sb.st_mode))
			return E_FILE;
	} else
		Puser_error("\tUnable to open",p);
	return UNKNOWN;
}

void delete_path(string p) {
	DIR *d;
	struct dirent *dir;
	char const *cpath = p.c_str();
	d = opendir(cpath);
	int ret;
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			string delfile(dir->d_name);
			string delpath = p + delfile;
			if (delfile == "." || delfile == "..")
				// Ignore current('.') and previous('..') directory
				continue;
			ret = remove(delpath.c_str());
			if (ret == -1) {
				Psystem_error("\tCannot remove",delpath.c_str());
			}
			else if (ret == 0)
				cout << "\tRemoved : " << delpath << endl;
		}
	} else {
		Psystem_error("\tCannot open",cpath);
	}
	rmdir(cpath);
}

char *Strduplicate(const char *source) {
	char *dest = (char *) malloc(strlen(source) + 1); // +1 = '\0'
	if (dest == NULL)
		return NULL;
	strcpy(dest, source);
	return dest;
}

string Get_cwd(string word) {
    char cwd[256];
    getcwd(cwd, sizeof(cwd));
    strcat(cwd,"/");
    strcat(cwd,word.c_str());
    string returnVal(cwd);
    return returnVal;
}
