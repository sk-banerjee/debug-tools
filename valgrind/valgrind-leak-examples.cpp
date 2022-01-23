#include <iostream>
#include <sys/mman.h>
#include <cstring>
#include <valgrind/valgrind.h>

using namespace std;

#define MMAP_BUF_LEN   (2048)
#define ALLOC_BUF_LEN  (4096)

void * my_alloc(size_t sz, int prot, int flags) {
    void * ptr = mmap(NULL, sz, prot, flags, 0, 0);
    if (ptr == MAP_FAILED) {
        cout << "mmap() failed" << endl;
        return NULL;
    }
    VALGRIND_MALLOCLIKE_BLOCK(ptr, sz, 0, 0);
    return ptr;
}

int my_free(void *ptr, size_t sz) {
    int err = munmap(ptr, sz);
    if (err != 0) {
        cout << "munmap() failed" << endl;
        return -1;
    }
    VALGRIND_FREELIKE_BLOCK(ptr, 0);
    return 0;
}

int main(int argc, char **argv) {
    void *ptr = nullptr;
    bool keep_looping = true;
    bool simulate_leaks = true;

    while (keep_looping) {
        ptr = my_alloc( MMAP_BUF_LEN, PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANONYMOUS);
        if (ptr)
            memset(ptr, '0', MMAP_BUF_LEN);
        if (!simulate_leaks)
            my_free(ptr, MMAP_BUF_LEN);

        ptr = my_alloc( MMAP_BUF_LEN, PROT_READ | PROT_WRITE,
                        MAP_SHARED | MAP_ANONYMOUS);
        if (ptr)
            memset(ptr, '1', MMAP_BUF_LEN);
        if (!simulate_leaks)
            my_free(ptr, MMAP_BUF_LEN);

        ptr = malloc(ALLOC_BUF_LEN);
        if (ptr)
            memset(ptr, '2', ALLOC_BUF_LEN);
        if (!simulate_leaks)
            free(ptr);

        char *p = new char[ALLOC_BUF_LEN];
        if (p)
            memset(p, '3', ALLOC_BUF_LEN);
        if (!simulate_leaks)
            delete[] p;

        cout << "Continue more iterations (y/n): ";
        char user_resp[10];
        cin >> user_resp;
        keep_looping = (user_resp[0] == 'y');

        if(keep_looping) {
            cout << "Simulate leaks (y/n): ";
            cin >> user_resp;
            simulate_leaks = (user_resp[0] == 'y');
        }
    }
    return 0;
}