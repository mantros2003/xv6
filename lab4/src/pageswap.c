#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"

int free_swap_slots[NSWAPSLOTS];

void swapinit() {
    for (int i = 0; i < NSWAPSLOTS; i++) {
        free_swap_slots[i] = 1;
    }
}

void mark_swap_slot_free(uint blockno) {
    int slot_index = (blockno - SWAP_SLOT_START) / 8;
    free_swap_slots[slot_index] = 1;
}

void mark_swap_slot_used(uint blockno) {
    int slot_index = (blockno - SWAP_SLOT_START) / 8;
    free_swap_slots[slot_index] = 0;
}

pte_t* get_victim_page(struct proc *p) {
    pde_t *pde = p -> pgdir;
    for (int retries = 0; retries < 10; retries++) {
      /* -------------------------------------------------------------- */
      /* TODO: Implement this function */
      /* Your code here */
      /* -------------------------------------------------------------- */
      clear_proc_access_bits(p);
    }
    panic("No victim page found");
}

void swap_out_page(pte_t *page) {
    /* -------------------------------------------------------------- */
    /* TODO: Implement this function */
    /* Your code here */
    /* -------------------------------------------------------------- */
}

void swap_out_victim_page(void)
{
    struct proc *victim_proc = get_victim_proc();
    victim_proc -> rss -= PGSIZE;

    pte_t *victim_page = get_victim_page(victim_proc);

    swap_out_page(victim_page);
}

void swap_in_page(void) {
    uint cr2 = rcr2();
    struct proc *p = myproc();
    myproc() -> rss += PGSIZE;

    /* -------------------------------------------------------------- */
    /* TODO: Implement this function */
    /* Your code here */
    /* -------------------------------------------------------------- */
}
