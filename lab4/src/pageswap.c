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
    pte_t *pte;
    for (int retries = 0; retries < 10; retries++) {
        /* -------------------------------------------------------------- */
        /* TODO: Implement this function */
        /* Your code here */
        /* -------------------------------------------------------------- */
        for (uint va = 0; va < p->sz; va += PGSIZE) {
        pte = walkpgdir(pde, (void*) va, 0);
            if (pte && (*pte & PTE_P) && (*pte & PTE_U)) {
                // Check if the Accessed bit is NOT set
                if ((*pte & PTE_A) == 0) {
                    return pte; // We found our victim!
                }
            }
        }
        clear_proc_access_bits(p);
    }
    panic("No victim page found");
}

void swap_out_page(pte_t *page) {
    /* -------------------------------------------------------------- */
    /* TODO: Implement this function */
    /* Your code here */
    /* -------------------------------------------------------------- */
    int swap_idx = -1;
    
    // Find a free swap slot in the global array
    for (int i = 0; i < NSWAPSLOTS; i++) {
        if (free_swap_slots[i] == 1) {
            swap_idx = i;
            break;
        }
    }
    
    if (swap_idx == -1) {
        panic("swap_out_page: out of swap space");
    }
    
    uint blockno = SWAP_SLOT_START + (swap_idx * 8);
    
    // Mark the slot as used so it doesn't get overwritten
    mark_swap_slot_used(blockno);

    // Extract the physical address from the top 20 bits of the PTE
    uint pa = PTE_ADDR(*page);
    // Convert physical address to kernel virtual address for the bio.c functions
    char *kva = (char *)P2V(pa);
    
    // Write the page from RAM to the disk (1 is the ROOTDEV device ID)
    move_page_memory_to_disk(1, kva, blockno);
    
    // 6. Free the physical RAM frame
    kfree(kva);
    
    // Update the Page Table Entry
    // Extract the current lower 12 bits (flags)
    uint flags = PTE_FLAGS(*page);
    
    // Clear the Present bit (since it's no longer in RAM)
    flags &= ~PTE_P; 
    
    // Set the Swapped Out bit (so the OS knows it's on disk)
    flags |= PTE_SO; 
    
    // Construct the new PTE:
    // Shift the block number into the top 20 bits, and attach our modified flags
    *page = (blockno << PTXSHIFT) | flags;
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

    uint va = PGROUNDDOWN(cr2);

    // 3. Locate the Page Table Entry (PTE) for this virtual address
    pte_t *pte = walkpgdir(p->pgdir, (void*)va, 0);

    // 4. Safety check: Ensure the PTE exists and is marked as Swapped Out
    if (!pte || !(*pte & PTE_SO)) {
        panic("swap_in_page: page fault is not due to a swapped-out page");
    }

    // 5. Extract the disk block number where the page is stored
    // The block number was hidden in the top 20 bits of the PTE during swap-out
    uint blockno = *pte >> PTXSHIFT;

    // 6. Allocate a fresh physical memory frame
    char *mem = kalloc();
    if (mem == 0) {
        panic("swap_in_page: out of memory"); 
        // Note: In your assignment framework, if kalloc fails here, 
        // it means memory is completely full AND swap space is completely full.
    }

    // 7. Read the page data from the disk back into RAM
    // 'mem' is a kernel virtual address, which bio.c expects
    // Device '1' is the standard ROOTDEV in xv6
    move_page_disk_to_memory(1, mem, blockno);

    // 8. Update the Page Table Entry
    uint flags = PTE_FLAGS(*pte); // Extract the lower 12 bits (flags)
    flags &= ~PTE_SO;             // Turn OFF the Swapped Out flag
    flags |= PTE_P;               // Turn ON the Present flag

    // Combine the new physical address with the updated flags
    // kalloc returns a virtual address, so we must convert it to physical using V2P
    *pte = V2P(mem) | flags;

    // 9. Free up the slot on the disk so it can be used again later
    mark_swap_slot_free(blockno);

    // 10. Flush the Translation Lookaside Buffer (TLB)
    // We modified a page table the CPU is actively using. Reloading cr3 
    // forces the hardware to clear its cache and see our new memory mapping.
    lcr3(V2P(p->pgdir));
}
