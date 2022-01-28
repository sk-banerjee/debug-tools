#include <iostream>
#include <cstring>
#include <thread>

using namespace std;

class Node {
    int data;
    char elements[16];
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

bool do_terminate = true;

void print_hello() {
    string s = "Hello World";
    char* p = strdup(s.c_str()); /* 12 bytes in 1 blocks are definitely lost */
    *(p + 5) = 0;
    cout << p;
    p += 6;
    cout << " " << p << endl;
}

void print_world() {
    string s = "Aloha World";
    char* p = strdup(s.c_str()); /* 12 bytes in 1 blocks are possibly lost */
    *(p + 5) = 0;
    cout << p;
    p += 6;
    cout << " " << p << endl; 
    if (do_terminate)
        terminate(); // This call leads to possibly lost line in valgrind logs.
}

int main(int argc, char **argv) {
    if (argc > 1)
        do_terminate = false;
    /* No nodes from list 1 leak are freed */
    llist *linked_list_1 = new llist; /* 8 bytes in 1 blocks are still reachable */
    for (int i = 100; i <= 104; i++) {
        linked_list_1->llist_insert_at_head(i); /* 160 bytes in 5 blocks are still reachable*/
    }
    linked_list_1->llist_print();

    /* Head from this leak is freed */
    llist *linked_list_2 = new llist; 
    for (int i = 110; i <= 119; i++) {
        linked_list_2->llist_insert_at_head(i); /* 288 bytes in 9 blocks are indirectly lost */
                                                /* 320 (32 direct, 288 indirect) bytes in 1 blocks are definitely lost */
    }
    linked_list_2->llist_print();
    delete linked_list_2;

    /* A mid node from this list is lost track*/
    llist *linked_list_3 = new llist;  
    for (int i = 120; i <= 134; i++) {
        linked_list_3->llist_insert_at_head(i); /* 32 bytes in 1 blocks are definitely lost */
    }
    linked_list_3->llist_print();
    linked_list_3->llist_simulate_leak_at_index(10);
    linked_list_3->llist_print();
    linked_list_3->llist_free();
    linked_list_3->llist_print();
    delete linked_list_3;

    thread thread_one (print_hello);

    thread thread_two (print_world); /* 16 bytes in 1 blocks are still reachable*/
                                     /* 288 bytes in 1 blocks are possibly lost */
    thread_one.join();
    thread_two.join();

    return 0;
}