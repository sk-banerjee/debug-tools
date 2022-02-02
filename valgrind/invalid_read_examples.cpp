#include <iostream>
#include <cstring>
#include <thread>

using namespace std;

int main() {
    int i = 0xff;
    int *p = &i;
    cout << "Valid read: " << *p << endl;
    // Pointer made to point to below SP
    p -= 128;
    cout << "Invalid read: " << *p << endl;
    // Pointer made to point further deeper below SP
    // This causes SIGSEGV under Valgrind
    p -= 4096;
    cout << "Invalid read: " << *p << endl;
    return 0;
}