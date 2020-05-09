#include <iostream>

#include "List.h"

using namespace std;

List::List() : head(NULL), last(NULL), size(0) {
	//cout << "A list has been created!" << endl;
}

List::~List() {
	int x;
	if (size > 0) {
		do x = PopHead();
		while (x > 0);
	}
	//cout << "A list has been destroyed!" << endl;
}

int List::GetSize() {
	return size;
}

int List::PopHead() {
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

void List::Push(File_metadata *v) {
	if (size == 0) {
		head = new Node(v);
		last = head;
	}
	else {
		Node* tmp = new Node(v);
		tmp->prev = last;
		last->next = tmp;
		last = tmp;
	}
	size++;
}

void List::Delete(File_metadata* v) {
	if (size == 0)
		cout << "List is empty!Cannot delete Node with value : " << v->file_name << endl;
	else {
		Node *current = head;
		bool found = false;
		while (current != NULL) {
			if (current->data == v) {
				if (current == head) { //if current == head
					if (size == 1) {
						head->next = NULL;
						head->prev = NULL;
						last->prev = NULL;
						last->next = NULL;
						last = NULL;
						delete head; // delete current
						head = NULL;
						found = true;
					}
					else if (size > 1) {
						head = head->next;
						delete head->prev;
						head->prev = NULL;
						found = true;
					}
				}
				else if (current == last) { //if current == last
					last = last->prev;
					delete last->next; // delete current
					last->next = NULL;
					found = true;
				}
				else { //if its neither head nor last
					Node* tmp = new Node(NULL);
					tmp->prev = current->prev;
					tmp->next = current->next;
					current->next->prev = tmp->prev;
					current->prev->next = tmp->next;
					delete tmp;
					found = true;
				}
				--size;
				break;
			}
			current = current->next;
		}
		if (found == false) {
			//cout << "Coulnt find value : |" << v->GetValue() << "| in the List!"<< endl;
		}

	}
}

void List::PrintList() {
	if (size == 0) {
		cout << "List : Empty!" << endl;
		return;
	}
	Node *cur = head;
	do {
		if (cur->next == NULL) // Just 4 formatting
			cout << cur->data->file_name;
		else
			cout << cur->data->file_name << " -> ";
		cur = cur->next;
	} while (cur != NULL);
	cout << endl;
}
/*
Node* List::PopNode(int i) {
	if (i > size) {
		cout << "Wrong input in List::PopNode" <<endl;
		return NULL;
	}
	int j = 1;
	Node *temp = head;
	do {
		if (j == i) {
			return temp;
		}
		else {
			temp = temp -> next;
		}
		j++;
	} while (j <= i);
	return NULL;
}*/

File_metadata* List::PopValue(int i) {
	if (i > size) {
		cout << "Wrong input in List::PopNode" <<endl;
		return NULL;
	}
	int j = 1;
	Node *temp = head;
	do {
		if (j == i) {
			return temp->data;
		}
		else {
			temp = temp -> next;
		}
		j++;
	} while (j <= i);
	return NULL;
}
