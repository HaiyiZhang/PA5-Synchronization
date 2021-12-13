#include <pthread.h>
#include <stdlib.h>

#include "csesem.h"
#include "pcq.h"

/* This structure must contain everything you need for an instance of a
 * PCQueue.  The given definition is ABSOLUTELY NOT COMPLETE.  You will
 * have to add several items to this structure. */

struct CSE_Semaphore {
    int value;
    pthread_mutex_t *mutex;
    pthread_cond_t *cond;

};
CSE_Semaphore sem_init(int value){
    CSE_Semaphore sem = calloc(1, sizeof(CSE_Semaphore));

    pthread_mutex_t *M = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(M, NULL);

    pthread_cond_t *C = malloc(sizeof(pthread_cond_t));
    pthread_cond_init(C,NULL);
    sem->value = value;
    sem->mutex = M;
    sem->cond = C;

    return sem;
}


struct PCQueue {
    int slots;
    int front;
    int rear;
    CSE_Semaphore mutex;
    CSE_Semaphore aslots;
    CSE_Semaphore items;
    void **queue;

};

/* The given implementation performs no error checking and simply
 * allocates the queue itself.  You will have to create and initialize
 * (appropriately) semaphores, mutexes, condition variables, flags,
 * etc. in this function. */
PCQueue pcq_create(int slots) {

    PCQueue pcq;
    if(slots<=0){
        return NULL;
    }
    pcq = calloc(1, sizeof(*pcq));
    pcq->slots = slots;
    pcq->front = pcq->rear = 0;
    pcq->mutex = sem_init(1);
    pcq->aslots = sem_init(slots);
    pcq->items = sem_init(0);
    
    pcq->queue = calloc(slots, sizeof(void *));


    return pcq;
}

/* This implementation does nothing, as there is not enough information
 * in the given struct PCQueue to even usefully insert a pointer into
 * the data structure. */
void pcq_insert(PCQueue pcq, void *data) {

    if(pcq->slots == 0){
        return;
    }


    csesem_wait(pcq->aslots);
    csesem_wait(pcq->mutex);
    pcq->queue[(++pcq->rear)%(pcq->slots)]=data;
    csesem_post(pcq->mutex);
    csesem_post(pcq->items);
    
}

/* This implementation does nothing, for the same reason as
 * pcq_insert(). */
void *pcq_retrieve(PCQueue pcq) {
 
    if(pcq==NULL){
        return NULL;
    }

    else if(pcq->slots == 0){
        return NULL;
    }

    else{
        void *item;
        csesem_wait(pcq->items);
        csesem_wait(pcq->mutex);
        item = (pcq->queue[(++pcq->front)%(pcq->slots)]);
        csesem_post(pcq->mutex);
        csesem_post(pcq->aslots);
        return item;
    }
    
    return NULL;
}

/* The given implementation blindly frees the queue.  A minimal
 * implementation of this will need to work harder, and ensure that any
 * synchronization primitives allocated here are destroyed; a complete
 * and correct implementation will have to synchronize with any threads
 * blocked in pcq_create() or pcq_destroy().
 *
 * You should implement the complete and correct clean teardown LAST.
 * Make sure your other operations work, first, as they will be tightly
 * intertwined with teardown and you don't want to be debugging it all
 * at once!
 */
void pcq_destroy(PCQueue pcq) {
    csesem_post(pcq->mutex);
    csesem_post(pcq->aslots);
    csesem_post(pcq->items);
    csesem_destroy(pcq->mutex);
    csesem_destroy(pcq->items);
    csesem_destroy(pcq->aslots);
    free(pcq->queue);
    free(pcq);
}
