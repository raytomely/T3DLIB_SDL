#ifndef CPUID_H
#define CPUID_H

#ifdef YES_WIN32 //_WIN32
#include <limits.h>
#include <intrin.h>
typedef unsigned __int32  uint32_t;

#else
#include <stdint.h>
#endif

class CPUID {
  uint32_t regs[4];

public:
  void load(unsigned i) {
#ifdef YES_WIN32 //_WIN32
    __cpuid((int *)regs, (int)i);

#else
    asm volatile
      ("cpuid" : "=a" (regs[0]), "=b" (regs[1]), "=c" (regs[2]), "=d" (regs[3])
       : "a" (i), "c" (0));
    // ECX is set to zero for CPUID function 4
#endif
  }

  const uint32_t &EAX() const {return regs[0];}
  const uint32_t &EBX() const {return regs[1];}
  const uint32_t &ECX() const {return regs[2];}
  const uint32_t &EDX() const {return regs[3];}
};


inline void cpu_info(void)
{
    CPUID cpuid;
    cpuid.load(0);
    printf("CPU: %.4s%.4s%.4s ",
        (const char*)&cpuid.EBX(),
        (const char*)&cpuid.EDX(),
        (const char*)&cpuid.ECX()
    );
    char brand[0x30];
    cpuid.load(0x80000002); memcpy(brand,&cpuid.EAX(),16);
    cpuid.load(0x80000003); memcpy(brand+16,&cpuid.EAX(),16);
    cpuid.load(0x80000004); memcpy(brand+32,&cpuid.EAX(),16);
    printf("%.48s\n",brand);
    cpuid.load(1);
    // tests bit 23 of ECX for popcnt instruction support
    printf("MMX - %s\n", cpuid.EDX() & (1 << 23) ? "yes" : "no");
    printf("SSE - %s\n", cpuid.EDX() & (1 << 25) ? "yes" : "no");
    printf("SSE2 - %s\n", cpuid.EDX() & (1 << 26) ? "yes" : "no");
    printf("SSE3 - %s\n", cpuid.ECX() & (1 << 0) ? "yes" : "no");
    printf("SSSE3 - %s\n", cpuid.ECX() & (1 << 9) ? "yes" : "no");
    printf("SSE4.1 - %s\n", cpuid.ECX() & (1 << 19) ? "yes" : "no");
    printf("SSE4.2 - %s\n", cpuid.ECX() & (1 << 20) ? "yes" : "no");
    printf("AES - %s\n", cpuid.ECX() & (1 << 25) ? "yes" : "no");
    printf("AVX - %s\n", cpuid.ECX() & (1 << 28) ? "yes" : "no");
    printf("HT - %s\n", cpuid.EDX() & (1 << 28) ? "yes" : "no");
    printf("FXSR (FXSAVE and FXRSTOR) - %s\n", cpuid.EDX() & (1 << 24) ? "yes" : "no");
    //printf("IA64 (emulating x86) - %s\n", cpuid.EAX() & (1 << 30) ? "yes" : "no");
    //printf("Hypervisor? - %s\n", cpuid.ECX() & (1 << 31) ? "yes" : "no");
    printf("popcnt - %s\n", cpuid.ECX() & (1 << 23) ? "yes" : "no");
}

inline bool check_MMX_support(void)
{
    CPUID cpuid;
    cpuid.load(1);
    return cpuid.EDX() & (1 << 23);
}

inline bool check_SSE_support(void)
{
    CPUID cpuid;
    cpuid.load(1);
    return ((cpuid.EDX() & (1 << 25)) && (cpuid.EDX() & (1 << 24)));
}

#endif // CPUID_H
