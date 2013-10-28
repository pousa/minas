/*
 * include/heap_atomic.h
 *
 * Definition of memory allocator inside a predefined heap
 * working for  NUMA architecture
 * and multithread environements
 *
 * Atomic methods for swap and compare
 *
 * Author: Martinasso Maxime
 *
 * (C) Copyright 2007 INRIA 
 * Projet: MESCAL / ANR NUMASIS
 * 
 */

#ifndef __ATOMIC_H
#define __ATOMIC_H


#ifdef __x86_64__

#define LOCK_PREFIX ""
struct __ha_xchg_dummy { unsigned long a[100]; };
#define __ha_xg(x) ((struct __ha_xchg_dummy *)(x))

static inline unsigned long ha_at_cmpchg(volatile void *ptr, unsigned long old,unsigned long new, int size) {
	unsigned long prev;
	switch (size) {
	case 1:
		__asm__ __volatile__(LOCK_PREFIX "cmpxchgb %b1,%2"
				     : "=a"(prev)
				     : "q"(new), "m"(*__ha_xg(ptr)), "0"(old)
				     : "memory");
		return prev;
	case 2:
		__asm__ __volatile__(LOCK_PREFIX "cmpxchgw %w1,%2"
				     : "=a"(prev)
				     : "r"(new), "m"(*__ha_xg(ptr)), "0"(old)
				     : "memory");
		return prev;
	case 4:
		__asm__ __volatile__(LOCK_PREFIX "cmpxchgl %k1,%2"
				     : "=a"(prev)
				     : "r"(new), "m"(*__ha_xg(ptr)), "0"(old)
				     : "memory");
		return prev;
	case 8:
		__asm__ __volatile__(LOCK_PREFIX "cmpxchgq %1,%2"
				     : "=a"(prev)
				     : "r"(new), "m"(*__ha_xg(ptr)), "0"(old)
				     : "memory");
		return prev;
	}
	return old;
}

#endif


#ifdef __ia64__


typedef unsigned char __ha_u8;
typedef unsigned short __ha_u16;
typedef unsigned int __ha_u32;
typedef unsigned long __ha_u64;


#define ha_ia64_cmpxchg1_acq(ptr, repl, old)                                            \
({                                                                                      \
        __ha_u64 ia64_intri_res;                                                        \
        asm volatile ("mov ar.ccv=%0;;" :: "rO"(old));                                  \
        asm volatile ("cmpxchg1.acq %0=[%1],%2,ar.ccv":                                 \
                              "=r"(ia64_intri_res) : "r"(ptr), "r"(repl) : "memory");   \
        ia64_intri_res;                                                                 \
})

#define ha_ia64_cmpxchg2_acq(ptr, repl, old)                                            \
({                                                                                      \
        __ha_u64 ia64_intri_res;                                                        \
        asm volatile ("mov ar.ccv=%0;;" :: "rO"(old));                                  \
        asm volatile ("cmpxchg2.acq %0=[%1],%2,ar.ccv":                                 \
                              "=r"(ia64_intri_res) : "r"(ptr), "r"(repl) : "memory");   \
        ia64_intri_res;                                                                 \
})

#define ha_ia64_cmpxchg4_acq(ptr, repl, old)                                            \
({                                                                                      \
        __ha_u64 ia64_intri_res;                                                        \
        asm volatile ("mov ar.ccv=%0;;" :: "rO"(old));                                  \
        asm volatile ("cmpxchg4.acq %0=[%1],%2,ar.ccv":                                 \
                              "=r"(ia64_intri_res) : "r"(ptr), "r"(repl) : "memory");   \
        ia64_intri_res;                                                                 \
})

#define ha_ia64_cmpxchg8_acq(ptr, repl, old)                                            \
({                                                                                      \
        __ha_u64 ia64_intri_res;                                                        \
        asm volatile ("mov ar.ccv=%0;;" :: "rO"(old));                                  \
        asm volatile ("cmpxchg8.acq %0=[%1],%2,ar.ccv":                                 \
                              "=r"(ia64_intri_res) : "r"(ptr), "r"(repl) : "memory");   \
        ia64_intri_res;                                                                 \
})

/*
 *  This function doesn't exist, so you'll get a linker error
 *  if something tries to do an invalid cmpxchg().
 */ 
extern long ha_ia64_cmpxchg_called_with_bad_pointer (void);

#define ha_ia64_cmpxchg(sem,ptr,old,repl,size)                                          \
({                                                                                      \
        __ha_u64 _o_, _r_;                                                              \
                                                                                        \
        switch (size) {                                                                 \
              case 1: _o_ = (__ha_u8 ) (long) (old); break;                             \
              case 2: _o_ = (__ha_u16) (long) (old); break;                             \
              case 4: _o_ = (__ha_u32) (long) (old); break;                             \
              case 8: _o_ = (__ha_u64) (long) (old); break;                             \
              default: break;                                                           \
        }                                                                               \
        switch (size) {                                                                 \
              case 1:                                                                   \
                _r_ = ha_ia64_cmpxchg1_##sem((volatile __ha_u8 *)(void*) ptr, repl, _o_);                       \
                break;                                                                  \
                                                                                        \
              case 2:                                                                   \
               _r_ = ha_ia64_cmpxchg2_##sem((volatile __ha_u16 *)(void*)  ptr, repl, _o_);                      \
                break;                                                                  \
                                                                                        \
              case 4:                                                                   \
                _r_ = ha_ia64_cmpxchg4_##sem((volatile __ha_u32 *)(void*)  ptr, repl, _o_);                     \
                break;                                                                  \
                                                                                        \
              case 8:                                                                   \
                _r_ = ha_ia64_cmpxchg8_##sem((volatile __ha_u64 *)(void*)  ptr, repl, _o_);                     \
                break;                                                                  \
                                                                                        \
              default:                                                                  \
                _r_ = ha_ia64_cmpxchg_called_with_bad_pointer();                        \
                break;                                                                  \
        }                                                                               \
        (__typeof__(old)) _r_;                                                          \
})

#define ha_cmpxchg_acq(ptr,o,n) ha_ia64_cmpxchg(acq, (ptr), (o), (n), sizeof(*(ptr)))
//#define ha_cmpxchg_rel(ptr,o,n) ha_ia64_cmpxchg(rel, (ptr), (o), (n), sizeof(*(ptr)))

/* for compatibility with other platforms: */
#define ha_cmpxchg(ptr,o,n)     ha_cmpxchg_acq(ptr,o,n)

static inline unsigned long ha_at_cmpchg(volatile void *ptr, unsigned long old,unsigned long new, int size) {
		ha_cmpxchg(ptr,old,new);
}


#endif

#ifdef __i386__

#define LOCK_PREFIX ""
struct __ha_xchg_dummy { unsigned long a[100]; };
#define __ha_xg(x) ((struct __ha_xchg_dummy *)(x))

static inline unsigned long  ha_at_cmpxchg(volatile void *ptr, unsigned long old,unsigned long repl, int size) {
        unsigned long prev;
        switch (size) {
        case 1:
                __asm__ __volatile__(LOCK_PREFIX "cmpxchgb %b1,%2"
                                     : "=a"(prev)
                                     : "q"(repl), "m"(*__ha_xg(ptr)), "0"(old)
                                     : "memory");
                return prev;
        case 2:
                __asm__ __volatile__(LOCK_PREFIX "cmpxchgw %w1,%2"
                                     : "=a"(prev)
                                     : "q"(repl), "m"(*__ha_xg(ptr)), "0"(old)
                                     : "memory");
                return prev;
        case 4:
                __asm__ __volatile__(LOCK_PREFIX "cmpxchgl %1,%2"
                                     : "=a"(prev)
                                     : "q"(repl), "m"(*__ha_xg(ptr)), "0"(old)
                                     : "memory");
                return prev;
        default:
                abort();
        }
        return old;
}

#endif

#endif
