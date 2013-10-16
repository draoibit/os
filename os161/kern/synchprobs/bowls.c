/*
 * bowls.c
 *
 * 26-11-2007 : KMS: maintain shared state for cat/mouse problem
 * 21-04-2009 : KMS: added sleep functions
 * 21-04-2009 : KMS: removed debug flag - always print simulation state
 * 05-01-2013 : TBB: added volatile declarations and fixed some indentations
 * 22-08-2013 : TBB: made cat and mouse eating and sleeping time optional parameters
 * 05-09-2013 : TBB: reformatted output for easier debugging and grading
 *
 */


/*
 * 
 * Includes
 *
 */
#include <types.h>
#include <lib.h>
#include <synch.h>
#include <clock.h>
#include <synchprobs.h>

/*
 *
 * shared simulation state
 * 
 * note: this is state should be used only by the 
 *  functions in this file, hence the static declarations
 *
 */

/* a character array with one entry for each bowl
 *  bowl[i-1] = 'c' if a cat is eating at the ith bowl
 *  bowl[i-1] = 'm' if a mouse is eating at the ith bowl
 *  bowl[i-1] = '-' otherwise */

/* The elements within the array can be changed by multiple 
 * threads so the contents are volatile.
 */
static volatile char *bowls;

/* the number of bowls : 
 * initialized by one thread not changed by others: not volatile */
static int num_bowls;

/* how many cats are currently eating? 
 * modified by multiple threads, so volatile 
 */
static int volatile eating_cats_count;

/* how many mice are currently eating? 
 * modified by different threads, so volatile 
 */
static int volatile eating_mice_count;

/* semaphore used to provide mutual exclusion
 * for reading and writing the shared simulation state 
 * The actual mutex is created/initizliaed by one thread and not 
 * modified by others: not volatile 
 */
static struct semaphore *mutex;


/*
 *
 * functions
 *
 */


/*
 * print_state()
 * 
 * Purpose:
 *   displays the simulation state
 *
 * Arguments:
 *   none
 *
 * Returns:
 *   nothing
 *
 * Notes:
 *   this assumes that it is being called from within
 *   a critical section - it does not provide its own
 *   mutual exclusion
 */
static void
print_state()
{
  int i;

  kprintf(" Eating Cats: %3d  Eating Mice: %3d   ",eating_cats_count,
    eating_mice_count);
  for(i=0;i<num_bowls;i++) {
    kprintf("%c",bowls[i]);
  }
  return;
}


/*
 * initialize_bowls()
 * 
 * Purpose:
 *   initializes simulation of cats and mice and bowls
 *
 * Arguments:
 *   unsigned int bowlcount:  the number of food bowls to simulate
 *
 * Returns:
 *   0 on success, 1 otherwise
 */
int
initialize_bowls(unsigned int bowlcount)
{
  unsigned int i;

  if (bowlcount == 0) {
    kprintf("initialize_bowls: invalid bowl count %d\n",bowlcount);
    return 1;
  }

  bowls = kmalloc(bowlcount*sizeof(char));
  if (bowls == NULL) {
    panic("initialize_bowls: unable to allocate space for %d bowls\n",bowlcount);
  }
  /* initialize bowls */
  for(i=0;i<bowlcount;i++) {
    bowls[i] = '-';
  }
  eating_cats_count = eating_mice_count = 0;
  num_bowls = bowlcount;

  /* intialize mutex semaphore */
  mutex = sem_create("bowl mutex",1);
  if (mutex == NULL) {
    panic("initialize_bowls: sem_create failed\n");
  }
  
  return 0;
}


/*
 * cleanup_bowls()
 * 
 * Purpose:
 *   Releases resources created by initialize_bowls.
 *
 * Arguments:
 *   None
 *
 * Returns:
 *   Nothing
 */
void
cleanup_bowls()
{
  if (mutex != NULL) {
    sem_destroy( mutex );
    mutex = NULL;
  }
  if (bowls != NULL) {
    kfree( (void *) bowls );
    bowls = NULL;
  }
}

/*
 * cat_eat()
 *
 * Purpose:
 *   simulates a cat eating from a bowl, and checks to
 *   make sure that none of the simulation requirements
 *   have been violated.
 *
 * Arguments:
 *   unsigned int bowlnumber: which bowl the cat should eat from
 *
 * Returns:
 *   nothing
 *
 */

void
cat_eat(unsigned int bowlnumber, int eat_time)
{

  /* check the argument */
  if ((bowlnumber == 0) || ((int)bowlnumber > num_bowls)) {
    panic("cat_eat: invalid bowl number %d\n",bowlnumber);
  }

  /* check and update the simulation state to indicate that
   * the cat is now eating at the specified bowl */
  P(mutex);   // start critical section

  /* first check whether allowing this cat to eat will
   * violate any simulation requirements */
  if (bowls[bowlnumber-1] == 'c') {
    /* there is already a cat eating at the specified bowl */
    panic("cat_eat: attempt to make two cats eat from bowl %d!\n",bowlnumber);
  }
  if (eating_mice_count > 0) {
    /* there is already a mouse eating at some bowl */
    panic("cat_eat: attempt to make a cat eat while mice are eating!\n");
  }
  KASSERT(bowls[bowlnumber-1]=='-');
  KASSERT(eating_mice_count == 0);

  /* now update the state to indicate that the cat is eating */
  eating_cats_count += 1;
  bowls[bowlnumber-1] = 'c';

  /* print a summary of the current state */
  kprintf("cat_eat   (bowl %3d) start:  ",bowlnumber);
  print_state();
  kprintf("\n");

  V(mutex);  // end critical section

  /* simulate eating by introducing a delay
   * note that eating is not part of the critical section */
  clocksleep(eat_time);

  /* update the simulation state to indicate that
   * the cat is finished eating */
  P(mutex);  // start critical section

  KASSERT(eating_cats_count > 0);
  KASSERT(bowls[bowlnumber-1]=='c');
  eating_cats_count -= 1;
  bowls[bowlnumber-1]='-';

  /* print a summary of the current state */
  kprintf("cat_eat   (bowl %3d) finish: ",bowlnumber);
  print_state();
  kprintf("\n");
  
  V(mutex);  // end critical section

  return;
}

/*
 * cat_sleep()
 *
 * Purpose:
 *   simulates a cat sleeping
 *
 * Arguments: none
 *
 * Returns: nothing
 *
 */
void
cat_sleep(int sleep_time)
{
  /* simulate sleeping by introducing a delay */
  clocksleep(sleep_time);
  return;
}


/*
 * mouse_eat()
 *
 * Purpose:
 *   simulates a mouse eating from a bowl, and checks to
 *   make sure that none of the simulation requirements
 *   have been violated.
 *
 * Arguments:
 *   unsigned int bowlnumber: which bowl the mouse should eat from
 *
 * Returns:
 *   nothing
 *
 */

void
mouse_eat(unsigned int bowlnumber, int eat_time)
{
  /* check the argument */
  if ((bowlnumber == 0) || ((int)bowlnumber > num_bowls)) {
    panic("mouse_eat: invalid bowl number %d\n",bowlnumber);
  }

  /* check and updated the simulation state to indicate that
   * the mouse is now eating at the specified bowl. */
  P(mutex);  // start critical section

  /* first check whether allowing this mouse to eat will
   * violate any simulation requirements */
  if (bowls[bowlnumber-1] == 'm') {
    /* there is already a mouse eating at the specified bowl */
    panic("mouse_eat: attempt to make two mice eat from bowl %d!\n",bowlnumber);
  }
  if (eating_cats_count > 0) {
    /* there is already a cat eating at some bowl */
    panic("mouse_eat: attempt to make a mouse eat while cats are eating!\n");
  }
  KASSERT(bowls[bowlnumber-1]=='-');
  KASSERT(eating_cats_count == 0);

  /* now update the state to indicate that the mouse is eating */
  eating_mice_count += 1;
  bowls[bowlnumber-1] = 'm';

  /* print a summary of the current state */
  kprintf("mouse_eat (bowl %3d) start:  ",bowlnumber);
  print_state();
  kprintf("\n");

  V(mutex);  // end critical section

  /* simulate eating by introducing a delay
   * note that eating is not part of the critical section */
  clocksleep(eat_time);

  /* update the simulation state to indicate that
   * the mouse is finished eating */
  P(mutex); // start critical section

  KASSERT(eating_mice_count > 0);
  eating_mice_count -= 1;
  KASSERT(bowls[bowlnumber-1]=='m');
  bowls[bowlnumber-1]='-';

  /* print a summary of the current state */
  kprintf("mouse_eat (bowl %3d) finish: ",bowlnumber);
  print_state();
  kprintf("\n");

  V(mutex);  // end critical section
  return;
}

/*
 * mouse_sleep()
 *
 * Purpose:
 *   simulates a mouse sleeping
 *
 * Arguments: none
 *
 * Returns: nothing
 *
 */
void
mouse_sleep(int sleep_time)
{
  /* simulate sleeping by introducing a delay */
  clocksleep(sleep_time);
  return;
}

