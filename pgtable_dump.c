/* Add the MMIO peripheral to virtual machine file in hw/arm/virt.c
 * The following functions will DUMP QEMU's Page Tables for a particular process.
 * The mapping is between QEMU guest's virtual address to host virtual address.
 * Target machine is AARCH64 (Cortex -A53) running Kernel > 3.10
 */
 



#include "hw/sysbus.h"
/******************* DUMP TTB ********************/

/* FOR TTB DUMP */

#define pHYS_MASK_SHIFT (48)
#define vA_BITS 39
#define pHYS_OFFSET 0x40000000
#define pAGE_OFFSET 0xFFFFFFFFFFFFFFFF << (vA_BITS -1)
#define vA(x)       ((uint64_t)((x) - pHYS_OFFSET) | pAGE_OFFSET)
#define PTRS_PER_PGD 512

uint64_t pHYS_MASK = (1UL << pHYS_MASK_SHIFT)-1;
uint64_t pAGE_MASK = (int32_t)~((1UL <<12)-1);



/* L1 PGD */

void dump_ttb_pgd(uint64_t pgd)
{
        int x=0;
        uint64_t total_len = 512 * sizeof(uint64_t);
        uint64_t *pgd_map = cpu_physical_memory_map((hwaddr)pgd, &total_len, 0);
        for(x=0;x<512;x++)  {
                if(pgd_map[x] & 1) { /*Valid 1 GB Section */
                    printf("\nPGD[%d]: %"PRIx64", Addr:%"PRIx64"\n", x, (uint64_t)pgd_map[x], pgd + (x<<3));
                    if(pgd_map[x] & 2) { /*Descriptor for larger block */
                        dump_ttb_pmd(pgd_map[x]);
                            }
                }
        }

}



/* DUMP PMD */

void dump_ttb_pmd(uint64_t pgd)
{
        int x=0;
        uint64_t total_len = 512 * sizeof(uint64_t);
        pgd  = pgd & pHYS_MASK & pAGE_MASK;
        uint64_t *pgd_map = cpu_physical_memory_map((hwaddr)pgd, &total_len, 0);
        printf("\n");
        for(x=0;x<512;x++)  {
                    if(pgd_map[x] & 1) {
                        printf("PMD[%d]: %"PRIx64", Addr: %"PRIx64"\n", x, (uint64_t)pgd_map[x],pgd + (x<<3));
                        if(pgd_map[x] & 2) { /*Descriptor for larger block */
                                dump_ttb_pte(pgd_map[x]);
                        }
                }
        }

}

/* DUMP PTE */
void dump_ttb_pte(uint64_t pgd)
{
        int x=0;
        uint64_t total_len = 512 * sizeof(uint64_t);
        pgd  = pgd & pHYS_MASK & pAGE_MASK;
        uint64_t *pgd_map = cpu_physical_memory_map ((hwaddr)pgd, &total_len, 0);
        printf("\n");
        for(x=0;x<512;x++)  {
                if(pgd_map[x] & 1) {
                    printf("PTE[%d]: %"PRIx64"\n", x, pgd + (x<<3));
                }
        }

}
