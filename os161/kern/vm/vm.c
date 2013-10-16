
#ifdef UW
/* This was added just to see if we can get things to compile properly and
 * to provide a bit of guidance for assignment 3 */

#include "opt-vm.h"
#if OPT_VM

#include <types.h>
#include <lib.h>
#include <addrspace.h>
#include <vm.h>

void
vm_bootstrap(void)
{
	/* May need to add code. */
}

#if 0 
/* You will need to call this at some point */
static
paddr_t
getppages(unsigned long npages)
{
   /* Adapt code form dumbvm or implement something new */
	 (void)npages;
	 panic("Not implemented yet.\n");
   return (paddr_t) NULL;
}
#endif

/* Allocate/free some kernel-space virtual pages */
vaddr_t 
alloc_kpages(int npages)
{
   /* Adapt code form dumbvm or implement something new */
	 (void)npages;
	 panic("Not implemented yet.\n");
   return (vaddr_t) NULL;
}

void 
free_kpages(vaddr_t addr)
{
	/* nothing - leak the memory. */

	(void)addr;
}

void
vm_tlbshootdown_all(void)
{
	panic("Not implemented yet.\n");
}

void
vm_tlbshootdown(const struct tlbshootdown *ts)
{
	(void)ts;
	panic("Not implemented yet.\n");
}

int
vm_fault(int faulttype, vaddr_t faultaddress)
{
  /* Adapt code form dumbvm or implement something new */
	(void)faulttype;
	(void)faultaddress;
	panic("Not implemented yet.\n");
  return 0;

}
#endif /* OPT_VM */

#endif /* UW */

