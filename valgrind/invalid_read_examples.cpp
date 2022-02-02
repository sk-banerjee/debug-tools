#include <iostream>
#include <cstring>
#include <ctype.h>

extern int* end;

int main(int argc, char **argv) {
    int i = 0xff;
    int *p = &i;
    int example = 0;

    if (argc == 2) {
        if(isdigit(argv[1][0])) {
            example = atoi(argv[1]);
        }
    } else {
        std::cout << "Will fall through all examples.\n"
             << "May abort on SIGSEGV without runing all" << std::endl;
    }

    if (example <= 1) {
        /* Example - 1 Invalid read
                     Due to Pointer pointing below SP.*/
        std::cout << "Valid read: " << *p << std::endl;
        // Pointer made to point to below SP
        p -= 128; // On x86-64 Ubuntu 20.04 good enoiugh to point just below SP
        std::cout << "Invalid read: " << *p << std::endl;

        // Pointer made to point further deeper below SP
        // This causes SIGSEGV under Valgrind
        p -= 4096;
        std::cout << "Invalid read: " << *p << std::endl;
    }

    /* Example - 2 Invalid read
                   Due to a read after free()/delete */
    if (example <= 2) {
        p = new int[10];
        memset(p, 0x20, 10 * sizeof(int));
        std::cout << "Valid read: " << std::hex << p[0] << std::endl;
        delete[] p;
        std::cout << "Invalid read: " << std::hex << p[0] << std::endl;
    }

    /* Example - 3 Invalid read
                   Due to a read from possibly unallocated page */
    if (example <= 3) {
        p = new int[10];
        memset(p, 0x21, 10 * sizeof(int));
        std::cout << "Valid read: " << std::hex << p[0] << std::endl;
        delete[] p;
        p = end - 1;
        std::cout << "Invalid read: " << std::hex << p[0] << std::endl;
    }

    /* Example - 4 Invalid read
                   Due to a read from zero page */
    if (example <= 4) {
        p = new int[10];
        memset(p, 0x22, 10 * sizeof(int));
        std::cout << "Valid read: " << std::hex << p[0] << std::endl;
        delete[] p;
        p = (int *) 0x32;
        std::cout << "Invalid read: " << std::hex << p[0] << std::endl;
    }

    return 0;
}
