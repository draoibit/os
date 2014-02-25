//This is a fourth thread test that prints that runs k < 10 threads with each
//thread kprinting a single integer digit to the console.

#include <types.h>
#include <lib.h>
#include <thread.h>
#include <synch.h>
#include <test.h>

void print_alison_numbers (void *, unsigned long);

int test_thread_fork(int argc, char ** args) {
 //   while (1) { will make thread counter unsafe
    int numThread = atoi(args[1]);
    int counter;
    //create threads
    for(int i=0;i<numThread;i++) {
	numThread = i + numThread;	
        thread_fork("", NULL, print_alison_numbers, NULL, (unsigned long) i);
    }
//}	 
}
void print_alison_numbers(void *data1, unsigned long data2){
	 kprintf("This is thread_%d", (int) data2++); 
}
