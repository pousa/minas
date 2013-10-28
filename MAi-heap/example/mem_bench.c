#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <numa.h>
#include <pthread.h>
#include <time.h>
#include <heap_alloc.h>
#include <heap_alloc_numa.h>

#define MAJOR_COUNT 5000
#define MINOR_COUNT 10

// adjust the CORES_COUNT, WORKERS_PER_CORE and MAX_BLOCK_FACTOR to your hardware capabilities
#define MAX_BLOCK_FACTOR 19
#define WORKERS_PER_CORE 1
#define CORES_COUNT 16
#define WORKERS_COUNT ( CORES_COUNT * WORKERS_PER_CORE )

// the following macro and function are used to get the CPU cycles count
#define rdtsc(low,high) __asm__ __volatile__("rdtsc" : "=a" (low), "=d" (high))
static inline unsigned long long get_rdtsc_sys()
{
    unsigned long low, high;
    rdtsc(low, high);
    return ((unsigned long long)high << 32) | (unsigned long long)low;
}

// According to POSIX specification, rand() is not guarenteed to be thread safe, 
// so the code will get a reasonable long list of random numers from the main thread
// and that list will later be used by all worker threads
char rand_nums[WORKERS_COUNT * MAJOR_COUNT];

// thread specific data
struct TArgs
{
    int no;     // thread sequence #
    int node;   // NUMA node that it should run on
    
    pthread_t tid; // thread id
    unsigned long long tdiff; // used to return the clock count that each thread spent doing the work
};

struct TArgs targs[8];

// the is the code to test/access the memory.  It touches it in reverse order, every 64 bytes
// to minimize the cache impact (at least on K8)
void AccessMemory( char* pdata, size_t blocksize )
{
#if 1 
    char* pval = pdata + blocksize - 2;
    char* pend = pdata;
    while( pval >= pend )
    {
        pval[0] = pval[1];
        pval-=64;
    }
#endif
}

// worker thread code
void* ThreadFunction( void* p )
{
    struct TArgs * pargs = (struct TArgs*)(p);
    
    // bind to specific NUMA node
    numa_run_on_node( pargs->node );
    
    // create a local random list
    char * rand_list = (char*)mai_malloc(MAJOR_COUNT);
    if( rand_list == NULL )
    {
        printf( "Unable to allocated random list!\n" );
        return NULL;
    }
    memcpy( rand_list, rand_nums + ( MAJOR_COUNT * pargs->no ), MAJOR_COUNT );

    // get the start tick count
    unsigned long long tstart = get_rdtsc_sys();
    
    // repeat the tests
    int repcount = 0;
    for(repcount = 0; repcount < MAJOR_COUNT; repcount++ )
    {
        // allocate random size memory block
        int sizeclass = rand_list[repcount];
        size_t block_size = 1 << sizeclass;
        char * pdata = (char *)mai_malloc( block_size );
        if( pdata == NULL )
        {
            printf( "thread %d failed to allocate %d bytes of memory!\n", pargs->no, block_size );
            continue;
        }
        
        // access the memory
         int q;
        for(q = 0; q < MINOR_COUNT; q++ )
            AccessMemory( pdata, block_size );

        // free the memory
       mai_free( pdata );
    }
    
    // get the end tick count
    unsigned long long tend = get_rdtsc_sys();
    
    // calculate the tick count needed to complete the tests
    pargs->tdiff = tend - tstart;
    
    // free the random list
   mai_free( rand_list );
}

// used to crate the worker threads
void CreateWorkerThread( int n, int node )
{
    targs[n].no = n;
    targs[n].node = node;
    if( 0 != pthread_create( &targs[n].tid, NULL, &ThreadFunction, &targs[n] ) )
    {
        printf( "pthread_create failed!\n" );
    }
}

// display information about total/free memory on each numa node
void DumpMemoryStats()
{
	int nodes = numa_max_node(),n;
    for(n = 0; n <= nodes; n++ )
    {
        long long freememsize;
        long long memsize = numa_node_size64( n, &freememsize );
        
        printf( "node %d has %lld bytes of memory (%lld bytes free)\n", n, memsize, freememsize );
    }
}

int main()
{
    int available = numa_available();
    if( available == -1 )
    {
        printf( "NUMA is not available on this system!\n" );
        return -1;
    }
    
    printf( "numa_available() returned %d\n", available );
    mai_init(BIND);
    int nodes = numa_max_node();
    printf( "There are %d NUMA nodes in this system:\n\n", nodes + 1 );
    DumpMemoryStats();
    
    // initialize the random number list
    int x;
    for(x = 0; x < WORKERS_COUNT * MAJOR_COUNT; x++ )
    {
        rand_nums[x] = 6 + rand() % MAX_BLOCK_FACTOR;   
    }
    
    time_t time_start = time(NULL);
    
    // start worker threads
    printf( "\nStarting workers...\n" );
    int thread_no = 0,node,m;
    for(node = 0; node < CORES_COUNT; node++ )
    {
        for(m = 0; m < WORKERS_PER_CORE; m++ )
        {
            CreateWorkerThread( thread_no, node );
            thread_no++;
        }
    }
    
    // wait until all threads complete
    printf( "Waiting for workers to complete...\n" );
    int n;
    for(  n = 0; n < WORKERS_COUNT; n++ )
    {
        if( 0 != pthread_join( targs[n].tid, NULL ) )
        {
            printf( "pthread_join failed!\n" );
        }
    }
    
    time_t time_end = time(NULL);
    
    printf( "All worker threads completed!\n\n" );
    
    DumpMemoryStats();
    
    double seconds = difftime( time_end, time_start );
    unsigned long long min = 0xffffffffffffffffll;
    unsigned long long max = 0;
    unsigned long long avg = 0;
    
    for(  n = 0; n < WORKERS_COUNT; n++ )
    {
	if( targs[n].tdiff < min )
		min = targs[n].tdiff;
	if( targs[n].tdiff > max )
		max = targs[n].tdiff;
	avg = avg + targs[n].tdiff;
    }

    printf( "\n%f seconds\t( avg: %llu,\tmin: %llu,\tmax: %llu ticks)\n", seconds, avg, min, max );
    
    return 0;
}

