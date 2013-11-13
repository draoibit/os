#include <types.h>
#include <lib.h>

	void hello(void);
   void hello(void) {  
		kprintf("I am lord and master here.\n");
	}	 
	int main(void) {
		hello();
  	return 0;
}
