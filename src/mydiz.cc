/*
 * mydiz.cc
 *
 *  Created on: Feb 2, 2017
 *      Author: alex
 */

#include <iostream>
#include <fstream>
#include <sstream>  // istringstream
#include <cstddef>
#include <string>
#include <cerrno> // errno

#include <stdio.h>
#include <stdlib.h> // strtol();
#include <unistd.h> // fork()/close();
#include <fcntl.h> // open();
#include <string.h> // memset();
#include <sys/types.h>
#include <sys/stat.h> // [mkfifo();]
#include <dirent.h>
#include <signal.h>
#include <sys/wait.h>

#include "Stack.h"
#include "InodeList.h"
#include "helper_funcs.h"
#include "archive_structure.h"

using namespace std;

int main(int argc ,char **argv) {
	cout << "Program(mydiz) #" << getpid() << " has just started!" << endl;
	cout << sizeof(File_metadata) << " " << sizeof(Archive_metadata) << endl;
	/* Getting arguments */
	if (argc < 4) {
		cerr << "ERROR : Wrong arguments (" << argc << ") for mydiz.cc!" << endl;
		cerr << "EXAMPLE : /mydiz {-c|-a|-x|-m|-d|-p|-j} "
				"<archive-file> <list-of-files/dirs" << endl;
		exit(EXIT_FAILURE);
	}
	int i = 1;

	char *flag = Strduplicate(argv[i++]);
	// Checking for second flag
	char *flag2 = (char*) malloc(2);
	if (argc > 1) {
		char tmp[NAME_SIZE];
        strcpy(tmp,argv[i++]);
        if (tmp[0] == '-')
            strcpy(flag2,tmp);
        else {
        	i--;
            free(flag2);
            flag2 = NULL;
            //cout << "No second flag inserted" << endl;
        }
	}

	char *archive_file = Strduplicate(argv[i++]);
	char *file = Strduplicate(argv[i++]);
	/*List *mainlist = new List();
	Stack *stack = new Stack();
	stack->Push(mainlist);
	while (i <= argc) {
		if (Check_path(argv[i]) == E_FILE) {
			File_metadata *fmd = new File_metadata(argv[i]);
			mainlist->Push(fmd);
		} else {
			Recursive_dir_helper(argv[i],stack);
		}
		i++;
	}*/
	
	switch(flag[1]) {
	case 'c':
		cout << "Creating archive-file : " << archive_file << endl;
		Create_archive(archive_file);
		break;
	case 'a':
		cout << "Appending files from list to archive-file..." << endl;
		Import_archive(archive_file,file);
		break;
	case 'j':
		cout << "Compressing files..." << endl;
		Compress_file(archive_file,file);
		break;
	case 'x':
		cout << "Extracting files from archive-file..." << endl;
		Extract_archive(archive_file,file);
		break;
	case 'm':
		cout << "Printing metadata of files..." << endl;
		Metadata_print(archive_file);
		break;
	case 'p':
		cout << "Printing the hierarchy of archive's files..." << endl;
		List_archive(archive_file);
		break;
	case 'q':
		cout << "Checking if list's files are in archive-file..." << endl;
		Query_archive(archive_file,file);
		break;
	case 'd':
		cout << "Deleting list's files from archive-file..." << endl;
		//break;
	default:
		cout << "Inserted unknown flag!" << endl;
	}

	if(flag != NULL)
		delete[] flag;
	if(flag2 != NULL)
		delete[] flag2;

	cout << "Program(mydiz) #" << getpid() << " has terminated successfully!" << endl;
    exit(EXIT_SUCCESS);
}
