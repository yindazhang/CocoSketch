#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>      /* open */ 
#include <unistd.h>     /* exit */
#include <sys/ioctl.h>  /* ioctl */
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/mman.h>
#include <errno.h>
#include "util.h"
#include "tuple.h"
#include "ringbuffer.h"

// The number of ringbuffer
// **Must** be (# pmd threads + 1)
#define MAX_RINGBUFFER_NUM 4
 

static inline char* ip2a(uint32_t ip, char* addr) {
    sprintf(addr, "%.3d.%.3d.%.3d.%.3d", (ip & 0xff), ((ip >> 8) & 0xff), ((ip >> 16) &
            0xff), ((ip >> 24) & 0xff));
    return addr;
}

void print_tuple(FILE* f, tuple_t* t) {
    char ip1[30], ip2[30];

    fprintf(f, "%s(%u) <-> %s(%u) %u %d\n",
            ip2a(t->key.src_ip, ip1), ntohs(t->key.src_port),
            ip2a(t->key.dst_ip, ip2), ntohs(t->key.dst_port),
            t->key.proto, ntohs(t->size)
            );
}

long long int counter = 0;
bool KEEP_RUNNING;
void handler(int sig) {
	if(sig == SIGINT){
		KEEP_RUNNING = false;
		return;
	}
	printf("%lld\n", counter);
	counter = 0;
	alarm(1);
}

int main(int argc, char *argv[]) {
    tuple_t t;
    LOG_MSG("Initialize the ringbuffer.\n");

    /* link to shared memory (datapath) */
	ringbuffer_t * rbs[MAX_RINGBUFFER_NUM];
	for (int i = 0; i < MAX_RINGBUFFER_NUM; ++i) {
		char name[30] = {0};
		sprintf(name, "/rb_%d", i);
		printf("name=%s\n", name);
		rbs[i] = connect_ringbuffer_shm(name, sizeof(tuple_t));
		// printf("%p\n", rbs[i]);
	}
	printf("connected.\n");	fflush(stdout);

    /* print number of pkts received per seconds */
	signal(SIGALRM, handler);
 	signal(SIGINT, handler);
	alarm(1);
	
	/* create your measurement structure (sketch) here !!! */

	/* begin polling */
	int idx = 0;
	KEEP_RUNNING = true;
	while (KEEP_RUNNING) {
            if (t.flag == TUPLE_TERM) {
                break;
            } 
            else {
	    	 while (read_ringbuffer(rbs[(idx) % MAX_RINGBUFFER_NUM], &t) < 0 && KEEP_RUNNING) {
                	 idx = (idx + 1) % MAX_RINGBUFFER_NUM;
        	 }
            	counter++;
            	// print_tuple(stdout, &t);
            	// Insert to sketch here
       	    }
        }	 
	// printf("totally insert %d packets\n", counter);
	return 0;
}
