//My awesome system calls
//I'm fairly sure I need the top four
//I've been trying to get these system calls
//compiled into the kernel all day and I'm
//starting to feel extremely frustrated
#include <types.h>
#include <copyinout.h>
#include <syscall.h>
#include <lib.h>
#include <kern/unistd.h>
#include <kern/errno.h>
#include <array.h>
#include <cpu.h>
#include <spl.h>
#include <spinlock.h>
#include <wchan.h>
#include <thread.h>
#include <threadlist.h>
#include <threadprivate.h>
#include <proc.h>
#include <current.h>
#include <synch.h>
#include <addrspace.h>
#include <mainbus.h>
#include <vnode.h>
#include "opt-synchprobs.h"



int sys_printparty(void){
	kprintf("Welcome to the party, Rory\n");
	return 0;
}

int sys_helloalison(void){
        kprintf("Hello Alison\n");
        return 0;
}

void sys__exit(int code){
  	thread_exit();
  	return 0;
}

//int sys_printstring(char *s, size_t len){
// return kprintf(s);
//}
