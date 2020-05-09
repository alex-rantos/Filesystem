/*
 * archive_structure.cpp
 *
 *  Created on: Feb 18, 2017
 *      Author: alex
 */

#include <bitset>

#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#include "archive_structure.h"
#include "helper_funcs.h"

using namespace std;

void Print_bits(size_t const size, void const * const ptr) {
	unsigned char *b = (unsigned char*) ptr;
	unsigned char byte;

	for (int i = size - 1; i >= 0; i--) {
		for (int j = 7; j >= 0; j--) {
			byte = (b[i] >> j) & 1;
			std::bitset<2> x(byte);
			cout << x;
		}
	}
	cout << endl;
}

bool File_exists(const char* filename) {
	if (!access(filename, F_OK)) {
		return true;
	} else
		return false;
}

void Block::UpdateMetadata(int allocated_blocks,const char* archive,int block_number,int whence,int size) {
	if (size == sizeof(Archive_metadata)) {
		Archive_metadata *amd = new Archive_metadata;
		this->ReadFile(amd,sizeof(*amd),archive,block_number,SEEK_SET);
		amd->archive_size += allocated_blocks;
		WriteFile_metadata(amd,sizeof(*amd),archive,block_number,SEEK_SET);
		delete amd;
	} else if (size == sizeof(File_metadata)) {
		File_metadata *fmd = (File_metadata *) malloc(sizeof(File_metadata));
		this->ReadFile(fmd,size,archive,block_number,SEEK_END);
		fmd->block_sum = allocated_blocks;
		WriteFile_metadata(fmd,size,archive,block_number,SEEK_END);
		free(fmd);
	} else Puser_error("UpdateMetadata::Wrong size-identifier",size);
}

void Block::ReadFile(void *ptr,int ptr_size,const char* filename,int block_number,int whence) {
	FILE *file = fopen(filename,"r+b");
	if (file != NULL) {

		if (fseek(file,block_number*BLOCK_SIZE,whence) != 0) // set cursor@certain block number of file
			Psystem_error("fseek setting @ block_num",block_number);
		//int result =
				fread(this->data,sizeof(char),BLOCK_SIZE,file);
		//if (result != BLOCK_SIZE)
		//	Puser_error("Readfile::Read only",result);
		fclose(file);

		this->CopyFromBlock(ptr,ptr_size);
	} else Psystem_error("Create_archive::Can't open",filename);
}

void Block::WriteFile_metadata(void *ptr,int ptr_size,const char* archive,int block_number,int whence) {
	FILE *file = fopen(archive,"a+b"); // append read/write && binary
	if (file != NULL) {
		if (fseek(file,block_number*BLOCK_SIZE,whence) != 0) // set cursor@certain block number of file
			Psystem_error("fseek setting @ block_num",block_number);
		this->CopyToBlock(ptr,ptr_size);
		int n = fwrite(this->data,sizeof(char),BLOCK_SIZE,file);
		if (n != BLOCK_SIZE)
			Puser_error("Written only",n);
		fclose(file);
	} else
		Psystem_error("Create_archive::Can't open",archive);
}

void Block::WriteFile_data(const char *fileToCopy,const char* archive,int f_size) {
	FILE *cfile = fopen(fileToCopy,"r+b");
	if (cfile != NULL) {
        FILE *file = fopen(archive,"a+b"); // append read/write && binary
        if (file == NULL) {
        	Psystem_error("WriteFromFile::Can't open",archive);
            fclose(cfile);
            return;
        }
		int counter = 0;
		int fsize = f_size;
		while (fsize > 0) {
			fread(data,sizeof(char),BLOCK_SIZE,cfile);

    		int n = fwrite(this->data,sizeof(char),BLOCK_SIZE,file);
    		if (n != BLOCK_SIZE) {
    			Puser_error("Write only",n);
    		}
			counter++;
			fsize -= BLOCK_SIZE;
            Reset();
		}
		cout << "Written " << counter << " blocks @" << archive << endl;
        fclose(file);

        // Update file_metadata block_sum block
		this->UpdateMetadata(counter,archive,counter + 1,SEEK_END,sizeof(File_metadata));
		// Update archive_header last_block_avail
		this->UpdateMetadata(counter,archive,0,SEEK_SET,sizeof(Archive_metadata));
	} else
		Psystem_error("WriteFromFile::Can't open",fileToCopy);
    fclose(cfile);
}

void Block::ExtractFile(const char *_archive,const char* filename,int size,int block_number,int whence) {
	FILE *cfile = fopen(_archive,"r+b");
	if (cfile != NULL) {
		if (fseek(cfile,block_number*BLOCK_SIZE,whence) != 0)
			Psystem_error("fseek setting @ block_num",block_number);
        FILE *file = fopen(filename,"a+b"); // append read/write && binary
        if (file == NULL) {
        	Psystem_error("WriteFromFile::Can't open",filename);
            fclose(cfile);
            return;
        }
		while (size > 0) {
			fread(data,sizeof(char),BLOCK_SIZE,cfile);
    		if (size < BLOCK_SIZE)
    			fwrite(this->data,sizeof(char),size,file);
    		else
    			fwrite(this->data,sizeof(char),BLOCK_SIZE,file);
    		size -= BLOCK_SIZE;
            Reset();
		}
        fclose(file);
	} else
		Psystem_error("ExtractFIle::Can't open",filename);
    fclose(cfile);
}

int Block::SearchFile(const char *archive,const char* target) {
	Archive_metadata *amd = new Archive_metadata();
	this->ReadFile(amd,sizeof(*amd),archive,0,SEEK_SET); // Get archive_header
	int max_blocks_to_search = amd->archive_size;
	int counter = 1;
	File_metadata *fmd = (File_metadata *) malloc(sizeof(File_metadata));
	// Search whole archive files's metadata
	while(counter <= max_blocks_to_search) {
		this->ReadFile(fmd,sizeof(*fmd),archive,counter,SEEK_SET); // Get File_metadata
		if (strncmp(fmd->file_name,target,strlen(target)) == 0) {
			free(fmd);
			return counter;
		}
		counter += fmd->block_sum + 1; // sum data blocks + 1 for file_metadata block
	}
	free(fmd);
	return -1;
}

void Print_metadata(Archive_metadata *amd) {
	cout << amd->file_name << " is " << amd->archive_size * BLOCK_SIZE << " bytes" << endl;
	cout << "User:" << amd->uid << " GroupID:" << amd->gid << endl;
	cout << "File permissions : ";Print_permissions(amd->st_mode);
}
void Print_metadata(File_metadata *fmd) {
	cout << fmd->file_name << " is " << (fmd->block_sum + 1) * BLOCK_SIZE << " bytes" << endl;
	cout << "User:" << fmd->info.st_uid << " GroupID:" << fmd->info.st_gid << endl;
	cout << "File permissions : "; Print_permissions(fmd->info.st_mode);
}

void Print_permissions(mode_t mode) {
	cout << ((S_ISDIR(mode)) ? "d" : "-")
			<< ((mode & S_IRUSR) ? "r" : "-") << ((mode & S_IWUSR) ? "w" : "-")
			<< ((mode & S_IXUSR) ? "x" : "-") << ((mode & S_IRGRP) ? "r" : "-")
			<< ((mode & S_IWGRP) ? "w" : "-") << ((mode & S_IXGRP) ? "x" : "-")
			<< ((mode & S_IROTH) ? "r" : "-") << ((mode & S_IWOTH) ? "w" : "-")
			<< ((mode & S_IXOTH) ? "x" : "-") << endl;
}

void Create_archive(const char* filename) {
	Archive_metadata *amd = new Archive_metadata;
	strncpy(amd->file_name,filename,strlen(filename));
	amd->archive_size = 1;
	amd->st_mode = 01006744; // Regular-file with permissions : rwxr--r--
	amd->uid = getuid();
	amd->gid = getgid();
	amd->atime = amd->ctime = amd->mtime = time(NULL);
	Block *block = new Block();
	block->WriteFile_metadata(((void*)amd),sizeof(Archive_metadata),filename,0,SEEK_SET);
	delete amd;
	delete block;
}

void Import_archive(const char* archive,const char* filename) {
	if (File_exists(archive)) {
		Archive_metadata *amd = new Archive_metadata();
		Block block;
		block.ReadFile(amd,sizeof(*amd),archive,0,SEEK_SET); // Get archive_header
		File_metadata *fmd = new File_metadata(filename);
		block.WriteFile_metadata(fmd,sizeof(*fmd),archive,0,SEEK_END); // write metadata @ the end
		block.UpdateMetadata(1,archive,0,SEEK_SET,sizeof(Archive_metadata)); // update archive_header that 1 block was allocated
		// Copies file to archive.
		block.WriteFile_data(filename,archive,File_size(filename));
		delete amd;
	} else Puser_error("Import_archive||EMPTY_ARCHIVE:",archive);
}

void Extract_archive(const char* archive,const char* filename) {
	Block block;
	int block_num = block.SearchFile(archive,filename);
	if (block_num == -1) {
		cout << "File :" << filename << " was not found in archive" << endl;
		return;
	}
	File_metadata *fmd = (File_metadata *) malloc(sizeof(File_metadata));
	block.ReadFile(fmd,sizeof(*fmd),archive,block_num,SEEK_SET);
	int size = fmd->info.st_size; // size of file size
	block.ExtractFile(archive,filename,size,block_num,SEEK_SET);
	free(fmd);

}

void Query_archive(const char* archive,const char* filename){
	Block block;
	if (block.SearchFile(archive,filename)) {
		cout << "File:" << filename << "has been found in archive" << endl;
	} else {
		cout << "File:" << filename << "has not been found in archive" << endl;
	}
}

void Metadata_print(const char* archive) {
	Archive_metadata *amd = new Archive_metadata();
	Block block;
	block.ReadFile(amd,sizeof(*amd),archive,0,SEEK_SET); // Get archive_header
	Print_metadata(amd);
	int max_blocks_to_search = amd->archive_size;
	int counter = 1;
	File_metadata *fmd = (File_metadata *) malloc(sizeof(File_metadata));
	// Search whole archive files's metadata
	while(counter <= max_blocks_to_search) {
		block.ReadFile(fmd,sizeof(*fmd),archive,counter,SEEK_SET); // Get File_metadata
		Print_metadata(fmd);
		counter += fmd->block_sum + 1; // sum data blocks + 1 for file_metadata block
	}
	free(fmd);
}


void List_archive(const char* archive) {

}

void Compress_file(const char* archive,const char* filename) {
	int ret;
	pid_t proc, pid;
	proc = fork();
	if (proc == 0) {
		ret = execlp("gzip", "gzip", "-9", "-k", filename, NULL);
		if (ret < 0) {
			Psystem_error("execlp-gzip");
			exit(EXIT_FAILURE);
		}
	} else {
		while ((pid = waitpid(-1, NULL, 0))) {
			if (errno == ECHILD)
				Psystem_error("Compress file waitpid");
		}
		char compressedFile[NAME_SIZE];
		strcpy(compressedFile,filename);
		strcat(compressedFile,".gz");
		File_metadata *fmd = new File_metadata(compressedFile);
		Block block;
		block.WriteFile_metadata(fmd,sizeof(*fmd),archive,0,SEEK_END);
		block.WriteFile_data(compressedFile,archive,File_size(compressedFile));
	}

}
