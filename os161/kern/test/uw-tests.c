
/*
 * UW - Synchronization test code.
 * Tim Brecht July, 2013
 */

#include <types.h>
#include <synch.h>
#include <thread.h>
#include <test.h>

#define NAME_LEN (20)

static struct lock *testlock;
static struct semaphore *donesem;

#define NTESTLOOPS    (5000)
#define NTESTTHREADS  (8)
#define START_VALUE   (0)
static volatile int test_value = (START_VALUE);
static int use_locks = 1;

static
void
cleanitems(void)
{
	kprintf("cleanitems: Destroying sems and locks\n");
	lock_destroy(testlock);
	sem_destroy(donesem);
}

static
void
inititems(void)
{
	if (testlock==NULL) {
		testlock = lock_create("testlock");
		if (testlock == NULL) {
			panic("synchtest: lock_create failed\n");
		}
	}

	if (donesem==NULL) {
		donesem = sem_create("donesem", 0);
		if (donesem == NULL) {
			panic("synchtest: sem_create failed\n");
		}
	}
}

/* This thread adds values to a global variable. */
static
void
add_thread(void *junk, unsigned long num)
{
	int i;
	(void) num;
	(void) junk;

	for (i=0; i<NTESTLOOPS; i++) {
		if (use_locks) {
			lock_acquire(testlock);
		}

      /* This loop is unrolled to possibly avoid optimizations
       * and to provide more instructions that could be interrupted.
       * This may or may not be necessary.
       */
		  test_value = test_value + 1;
		  test_value = test_value + 1;
		  test_value = test_value + 1;
		  test_value = test_value + 1;
		  test_value = test_value + 1;

		if (use_locks) {
			lock_release(testlock);
		}

	}
	V(donesem);
	thread_exit();
}

/* This thread substract values from a global variable. */
static
void
sub_thread(void *junk, unsigned long num)
{
	int i;
	(void)num;
	(void)junk;

	for (i=0; i<NTESTLOOPS; i++) {
		if (use_locks) {
			lock_acquire(testlock);
		}

      /* This loop is unrolled to avoid optimizations
       * and to provide more instructions that could be interrupted.
       * This may or may not be necessary.
       */
		  test_value = test_value - 1;
		  test_value = test_value - 1;
		  test_value = test_value - 1;
		  test_value = test_value - 1;
		  test_value = test_value - 1;

		if (use_locks) {
			lock_release(testlock);
		}

	}
	V(donesem);
	thread_exit();
}

int
uwlocktest1(int nargs, char **args)
{
	int i, result;
  char name[NAME_LEN];

	(void)nargs;
	(void)args;

	inititems();
	kprintf("Starting uwlocktest1...\n");

	for (i=0; i<NTESTTHREADS; i++) {
    snprintf(name, NAME_LEN, "add_thread %d", i);
		result = thread_fork(name, NULL, add_thread, NULL, i);
		if (result) {
			panic("uwlocktest1: thread_fork failed: %s\n",
			      strerror(result));
		}
	}

	for (i=0; i<NTESTTHREADS; i++) {
    snprintf(name, NAME_LEN, "sub_thread %d", i);
		result = thread_fork(name, NULL, sub_thread, NULL, i);
		if (result) {
			panic("uwlocktest1: thread_fork failed: %s\n",
			      strerror(result));
		}
	}

	for (i=0; i<NTESTTHREADS*2; i++) {
		P(donesem);
	}

	kprintf("value of test_value = %d should be %d\n", test_value, START_VALUE);
	if (test_value == START_VALUE) {
  	kprintf("TEST SUCCEEDED\n");
  } else {
  	kprintf("TEST FAILED\n");
  }
	KASSERT(test_value == START_VALUE);

	cleanitems();
	kprintf("uwlocktest1 done.\n");

	return 0;
}

