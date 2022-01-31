#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sys/mman.h>
#include <valgrind/valgrind.h>

using namespace std;

void mismatched_alloc_free() {
    char *ptr = (char *) malloc(10);
    memset(ptr, 'X', 10);
    delete ptr; // Mismatched free() / delete / delete []

    ptr = new char[10];
    memset(ptr, 'Y', 10);
    free(ptr); // Mismatched free() / delete / delete []
}

void mismatched_mmap_free(size_t sz, int prot, int flags) {
    void * ptr = mmap(NULL, sz, prot, flags, 0, 0);
    if (ptr == MAP_FAILED) {
        cout << "mmap() failed" << endl;
        return;
    }
    VALGRIND_MALLOCLIKE_BLOCK(ptr, sz, 0, 0);
    free(ptr); //Mismatched free() / delete / delete []
}

void simulate_invalid_read() {
    char *ptr = new char[10];
    memset(ptr, 'Y', 10);
    cout << *(ptr + 6) << " ";    // valid read
    cout << *(ptr + 12) << endl; // invalid read
    delete ptr;  // This is also mismatched as correct way is delete[] ptr
}

void simulate_invalid_write() {
    char *ptr = new char[10];
    memset(ptr, 'Y', 10);
    *(ptr + 6) = ' ';  // valid write
    *(ptr + 12) = ' '; // invalid write
    delete[] ptr;
}

void jump_move_depends_uninitialised() {
    int i; // this uninitialized value casues valgrind to report
    if (i > 0) // Conditional jump or move depends on uninitialised value(s)
        cout << "Hello World!" << endl;
    else if (i == 0)
        cout << "Aloha World!" << endl;
    else
        cout << "Bonjour World!" << endl;
}

void use_uninitialised_in_syscall() {
    ofstream outfile ("new.txt", ofstream::binary);
    char *ptr = new char[10];
    outfile.write (ptr, 10); //Syscall param write(buf) points to uninitialised byte(s)
    delete[] ptr;
    outfile.close();
}

void illegal_free() {
    char i = 'x';
    char *ptr = &i;
    delete ptr; // Invalid free() / delete / delete[] / realloc()
}

void src_dest_overlap() {
    char *ptr = new char[100];
    memcpy(ptr, "Hello Aloha", strlen("Hello Aloha"));
    memcpy(ptr, &ptr[6], 10);
    delete[] ptr;
}

int main() {
    mismatched_alloc_free();
    simulate_invalid_read();
    simulate_invalid_write();
    jump_move_depends_uninitialised();
    use_uninitialised_in_syscall();
    illegal_free();
    src_dest_overlap();
    mismatched_mmap_free(4096, PROT_READ | PROT_WRITE,
                         MAP_SHARED | MAP_ANONYMOUS);
    return 0;
}
