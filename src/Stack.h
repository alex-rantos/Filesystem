#ifndef Stack_H_
#define Stack_H_

#include "InodeList.h"


class Stack {
	struct Node {
		List *seclist;
		Node *next,*prev;
		Node() : seclist(NULL),next(NULL),prev(NULL) {
		};
	};
	Node *head,*last;
	int size;
public:
    Stack();
    ~Stack();

    int GetSize();
    List* GetLastList() {return last->seclist;};
    int PopHead(); 	           // deletes head
    void Push(List *);          // inserting a bank account to the end of the Stack
    void Delete_Last();
};

#endif
