#include "types.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "spinlock.h"
#include "types.h"
#include "param.h"

struct 
{
    uint8 ref_cnt;
    struct spinlock lock;
} cows[(PHYSTOP - KERNBASE) >> 12];

void increfcnt(uint64 pa)
{
    if(pa < KERNBASE)
    {
        return;
    }
    pa = (pa - KERNBASE) >> 12;
    acquire(&cows[pa].lock);
    cows[pa].ref_cnt++;
    release(&cows[pa].lock);
}

void decrefcnt(uint64 pa)
{
    if(pa < KERNBASE)
    {
        return;
    }
    pa = (pa - KERNBASE) >> 12;
    acquire(&cows[pa].lock);
    cows[pa].ref_cnt--;
    release(&cows[pa].lock);
}

uint8 refCount(uint64 pa)
{
    if(pa < KERNBASE)
    {
        return -1;
    }
    pa = (pa - KERNBASE) >> 12;
    return cows[pa].ref_cnt;
}

int cowhandler(pagetable_t p, uint64 va)
{
    va = PGROUNDDOWN(va);
    if(va >= MAXVA)
        return -1;
        
    pte_t *pte = walk(p, va, 0);
    
    if (pte == 0 || (*pte & PTE_U) == 0 || (*pte & PTE_V) == 0 || (*pte & PTE_COW) == 0)
        return -1;   
    
    char *mem;
    uint64 pa = PTE2PA(*pte);
    if((mem = kalloc()) == 0)
       //no more memery kill process
        return -1; 
        
    memmove((void*)mem, (void*)pa, PGSIZE);
    uint flags = (PTE_FLAGS(*pte) & (~PTE_COW)) | PTE_W;
    kfree((void*)pa);
    *pte = PA2PTE(mem) | flags;
    return 0;
}