
#ifndef __MA_LINUX_H
#define __MA_LINUX_H

#define ALL 999
#define OS  0
#define LOCAL 1
#define INTERLEAVE 2


/*******************************************************************************
 LINUX based functions
 - to map process and threads to the machine processing units
 - to set the memory policy for memory allocation
*******************************************************************************/
void ma_linux_set_proc_core(int core);
int ma_linux_proc_core();

void ma_linux_set_my_core(int core);
int ma_linux_my_core();

int ma_linux_get_nodeid();
void ma_linux_set_mempol(int mempol);

#endif
