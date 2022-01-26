#include <iostream>

using namespace std;

class Node {
    int data;
    Node *next;

public:
    Node() : data(0), next(nullptr) { }
    Node(int d) : data(d), next(nullptr) { }
    Node(int d, Node *nxt) : data(d), next(nxt) { }
    int get_data() {return this->data; }
    Node *get_next() { return this->next; }
    void set_next(Node *addr) { this->next = addr; }
};

class llist {
    Node *head;

public:
    llist() : head(nullptr) { }
    void llist_insert_at_head(int v);
    void llist_simulate_leak_at_index(int idx);
    void llist_free();
    void llist_print();
};

void llist::llist_insert_at_head(int v) {
    Node *newNode = new Node(v, head);
    head = newNode;
}

void llist::llist_simulate_leak_at_index(int idx) {
    int i = 0;
    for (auto t = head; t; t = t->get_next(), i++) {
        if ( i == idx) {
            t->set_next(t->get_next()->get_next());
        }
    }
}

void llist::llist_free() {
    while(head) {
        auto to_free = head;
        head = head->get_next();
        delete to_free;
    }
}

void llist::llist_print( ) {
    for(auto t = head; t; t = t->get_next()) {
        cout << "  " << t->get_data() << "  ";
    }
    cout << endl;
}

int main() {
    llist *linked_list_1 = new llist;  /* No nodes from this leak are freed */
                                       /* Valgrind: 88 (8 direct, 80 indirect) bytes in 1 blocks are definitely */
    for (int i = 100; i <= 104; i++) {
        linked_list_1->llist_insert_at_head(i); /* Valgrind: 80 bytes in 5 blocks are indirectly lost */
    }
    linked_list_1->llist_print();

    llist *linked_list_2 = new llist;  /* Head node from this leak is freed */
    for (int i = 110; i <= 119; i++) {
        linked_list_2->llist_insert_at_head(i); /* Valgrind: 144 bytes in 9 blocks are indirectly lost*/
                                                /* Valgrind: 160 (16 direct, 144 indirect) bytes in 1 blocks are definitely lost*/
    }
    linked_list_2->llist_print();
    delete linked_list_2;

    llist *linked_list_3 = new llist;  /* Mid node from this list is lost track*/
    for (int i = 120; i <= 134; i++) {
        linked_list_3->llist_insert_at_head(i); /* Valgrind: 16 bytes in 1 blocks are definitely lost */
    }
    linked_list_3->llist_print();
    linked_list_3->llist_simulate_leak_at_index(10);
    linked_list_3->llist_print();
    linked_list_3->llist_free();
    linked_list_3->llist_print();
    delete linked_list_3;

    return 0;
}