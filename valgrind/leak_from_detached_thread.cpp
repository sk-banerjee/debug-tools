#include <iostream>
#include <unistd.h>
#include <thread>
#include <cstddef>

using namespace std;

bool keep_looping = true;
bool user_input_recevied = false;

void thread1_func() {
    cout << "T1 entry" << endl;
    byte *p = new byte[97]; // Allocating 97 bytes for easy correlation.
    int *q = (int *)p;
    *q = 0;
    while(keep_looping) {
        *q += 1;
        sleep(1);
    }
    cout << "\nT1 exit" << endl;
}

void thread2_func() {
    cout << "T2 entry" << endl;
    byte *p = new byte[89]; // Allocating 89 bytes for easy correlation.
    int *q = (int *)p;
    *q = 0;
    while(!user_input_recevied) {
        *q += 1;
        sleep(1);
    }
    cout << "\nT2 exit" << endl;
}

int main() {
    cout << "Two threads are spawned T1/T2.\n"
         << "   T1: is detahced\n"
         << "   T2: is joinable\n"
         << " Enter 0 to exit the detached thread T1 before main() returns.\n"
         << " Enter 1 to let thread T1 run, it's resources will be reaped by OS."
         << endl;

    thread t1 (thread1_func);
    thread t2 (thread2_func);
    t1.detach();

    cin >> keep_looping;
    user_input_recevied = true;

    cout << "Thread T1 " << (keep_looping ? "is still running." : "should exit.") << endl;

    cout << "Main thread sleeping for 3 seconds before return" << endl;
    sleep(3);

    t2.join();
}