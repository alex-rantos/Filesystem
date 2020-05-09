#ifndef LIST_H_
#define LIST_H_

#include "archive_structure.h"
extern int num_of_files;
extern bool was_dir;


class List {
	struct Node {
		File_metadata* data;
		List* list_of_files; // if file thats NULL if dir its the list of its files
		Node *next,*prev;
		Node(File_metadata *v) : data(v),list_of_files(NULL),next(NULL),prev(NULL) {};
	};
	Node *head,*last;
	int size;
public:
    List();
    ~List();

    int GetSize();
    void SetLastList(List *);

    int PopHead();
    void Push(File_metadata* );
    void Delete(File_metadata* );
    void PrintList();
};

#endif
