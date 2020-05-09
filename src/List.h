#ifndef LIST_H_
#define LIST_H_

#include "archive_structure.h"


class List {
	struct Node {
		File_metadata* data;
		List *dir_files;
		Node *next,*prev;
		Node(File_metadata *v) : data(v),dir_files(NULL),
				next(NULL),prev(NULL) {};
	};
	Node *head,*last;
	int size;
public:
    List();
    ~List();

    int GetSize();
    File_metadata* PopValue(int );       // returns the key of int-th-argument

    Node* PopNode(int);
    int PopHead(); 	           // deletes head
    void Push(File_metadata* );          // inserting a bank account to the end of the list
    void Delete(File_metadata* );
    void PrintList(); 	     // Ektipwnei ola ta value tou HList
};

#endif 
