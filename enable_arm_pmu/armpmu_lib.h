#ifndef ARMPMU_LIB_H
#define ARMPMU_LIB_H

static inline uint32_t
rdtsc32(void)
{
        uint32_t r = 0;
        asm volatile("mov %eax, %ebx" );
        //asm volatile("mrc p15, 0, %0, c9, c13, 0" : "=r"(r) );
        return r;
}

#endif /* ARMPMU_LIB_H */
