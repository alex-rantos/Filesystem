#include <iostream>

#include "Stack.h"

using namespace std;

Stack::Stack() : head(NULL), last(NULL), size(0) {
	//cout << "A Stack has been created!" << endl;
}

Stack::~Stack() {
	int x;
	if (size > 0) {
		do x = PopHead();
		while (x > 0);
	}
	//cout << "A Stack has been destroyed!" << endl;
}

int Stack::GetSize() {
	return size;
}

int Stack::PopHead() {
	if (size == 1) {
		head->next = NULL;
		head->prev = NULL;
		last->prev = NULL;
		last->next = NULL;
		last = NULL;
		delete head;
		head = NULL;
	} else if (size > 1) {
		head = head->next;
		delete head->prev;
		head->prev = NULL;
	}
	return --size;
}

void Stack::Push(List *list) {
	if (size == 0) {
		head = new Node();
		last = head;
		head->seclist = list;
	}
	else {
		Node* tmp = new Node();
		tmp->prev = last;
		last->next = tmp;
		last = tmp;
		tmp->seclist = list;
		last->next = NULL;
	}
	size++;
}

void Stack::Delete_Last() {
	if (size == 0)
		cout << "Stack-stack is empty!Cannot delete Node with value : " <<  endl;
	else {
		Node *temp = last->prev;
		last = temp;
		temp = temp->next;
		delete temp;
		last->next = NULL;
	}
}
