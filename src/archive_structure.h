/*
 * archive_structure.h
 *
 *  Created on: Feb 18, 2017
 *      Author: alex
 */

#ifndef ARCHIVE_STRUCTURE_H_
#define ARCHIVE_STRUCTURE_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BLOCK_SIZE 512
#define NAME_SIZE 324
#define ERROR_BUFFER_SIZE 255

typedef struct stat File_info;

inline void Psystem_error(const char *message) {
	perror(message);
	exit(EXIT_FAILURE);
}
inline void Psystem_error(const char *message,const char *variable) {
	char errorbuf[ERROR_BUFFER_SIZE];
	sprintf(errorbuf,"ERROR:\t%s :{%s} ",message,variable);
	perror(errorbuf);
	exit(EXIT_FAILURE);
}
inline void Psystem_error(const char *message,int variable) {
	char errorbuf[ERROR_BUFFER_SIZE];
	sprintf(errorbuf,"ERROR:\t%s :{%d} ",message,variable);
	perror(errorbuf);
	exit(EXIT_FAILURE);
}
inline void Puser_error(const char *message,const char *detail) {
	fprintf(stderr, "ERROR:\t%s :{%s} \n", message, detail);
	//exit(1);
}
inline void Puser_error(const char *message,int detail) {
	fprintf(stderr, "ERROR:\t%s :{%d} bytes.\n", message, detail);
	//exit(1);
}

class Block {
	char data[BLOCK_SIZE];
public:
	Block() { Reset();};
	~Block() { Reset();};

	void CopyFromBlock(void *ptr,int size) {memcpy(ptr,this->data,size);Reset();}
	void CopyToBlock(void *ptr,int size) { Reset();memcpy(this->data,ptr,size);}
	void Reset() { memset(data,0,BLOCK_SIZE); }

	// Update archive-metadata last_avail_block by adding how many blocks has been allocated
	void UpdateMetadata(int allocated_blocks,const char* archive,int block_number,int whence,int size);
	// Reads block_number-th block from archive file according to whence-flag
	// and saves the metadata to ptr by ptr_size.
	void ReadFile(void *ptr,int ptr_size,const char* filename,int block_number,int whence);
	// Writes ptr-metadata to corresponding block starting from whence
	void WriteFile_metadata(void *ptr,int ptr_size,const char* filename,int block_number,int whence);

	/* Copies data block by block from filename to archive
	 * Updates corresponding file_metadata and archive_header
	 * for the allocated blocks */
	void WriteFile_data(const char *fileToCopy,const char* archive,int f_size);
	void ExtractFile(const char *fileToCopy,const char* archive,int size,int block_number,int whence);

	// Search file target in archive and returns its metadata block number if exists
	// Otherwise -1
	int SearchFile(const char* archive,const char* target);
};

struct File_metadata {
	char      file_name[NAME_SIZE]; /* name of file_name */
	int       block_sum;           /* sum of blocks needed to store data
	 	 	 	 	 	 	 	   * if directory it's the sum of its files */
	File_info info;              /* stat-information about file */
    File_metadata(const char *name): block_sum(-1) {
        strcpy(file_name,name);
        if (stat(name,&info) == -1)
        	Psystem_error("File_metadata constructor,stat failure");
    }
};

struct Archive_metadata {
	char          file_name[NAME_SIZE]; /* Name of archive-file */
    unsigned long archive_size;        /* size of whole archive-file in blocks */

    uid_t     uid;       /* user ID of owner */
    gid_t     gid;      /* group ID of owner */
    mode_t    st_mode; /* Type of file & permissions */

    time_t    atime;   /* time of last access */
    time_t    mtime;  /* time of last modification */
    time_t    ctime; /* time of last status change */
};
void Print_Metadata(Archive_metadata *amd);
void Print_Metadata(File_metadata *amd);
void Print_permissions(mode_t mode);

void Create_archive(const char* filename); // Create archive-file and initialize its header
void Import_archive(const char* archive,const char* filename);  // flag : -a
void Extract_archive(const char* archive,const char* filename);// flag : -x
void Query_archive(const char* archive,const char* filename); // flag : -q
void Metadata_print(const char* archive);                    // flag : -m
void List_archive(const char* archive);                     // flag : -p
void Compress_file(const char* archive,const char* filename);                  // flag : -j

bool File_exists(const char* filename); // Returns if file exists
// Print bits of any kind of structure
void Print_bits(size_t const size,void const *ptr);

#endif /* ARCHIVE_STRUCTURE_H_ */
