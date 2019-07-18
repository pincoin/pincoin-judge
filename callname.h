#if __WORDSIZE == 64
#define REG(reg) reg.orig_rax
#else
#define REG(reg) reg.orig_eax
#endif


const char* callname(long call);
