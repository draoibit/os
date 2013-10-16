
/* TBB: Created because newer compilers want prototypes for these functions */

/* For the catmouse problem */
/* this must be called before any calls to cat_eat or mouse_eat */
int initialize_bowls(unsigned int bowlcount);

void cleanup_bowls( void );
void cat_eat(unsigned int bowlnumber, int eat_time);
void mouse_eat(unsigned int bowlnumber, int eat_time);
void cat_sleep(int sleep_time);
void mouse_sleep(int sleep_time);

