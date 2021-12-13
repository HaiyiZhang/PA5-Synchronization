#include <pthread.h>
#include <stdlib.h>

#include "csesem.h"

/* This definition of struct CSE_Semaphore is only available _inside_
 * your semaphore implementation.  This prevents calling code from
 * inadvertently invalidating the internal representation of your
 * semaphore.  See csesem.h for more information.
 *
 * You may place any data you require in this structure. */
struct CSE_Semaphore {
    int value;
    pthread_mutex_t *mutex;
    pthread_cond_t *cond;

};

/* This function must both allocate space for the semaphore and perform
 * any initialization that is required for safe operation on the
 * semaphore.  Te user should be able to immediately call csesem_post()
 * or csesem_wait() after this routine returns. */




CSE_Semaphore csesem_create(int count) {
    CSE_Semaphore sem = calloc(1, sizeof(struct CSE_Semaphore));

    pthread_mutex_t *M = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(M, NULL);

    pthread_cond_t *C = malloc(sizeof(pthread_cond_t));
    pthread_cond_init(C,NULL);

    sem->value = count;

    sem->mutex = M;
    sem->cond = C;

    return sem;
}

void csesem_post(CSE_Semaphore sem) {
    pthread_mutex_lock(sem->mutex);
    sem->value++;
    pthread_cond_signal(sem->cond);


    pthread_mutex_unlock(sem->mutex);
}

void csesem_wait(CSE_Semaphore sem) {
    pthread_mutex_lock(sem->mutex);
   
    while(sem->value==0){
        pthread_cond_wait(sem->cond, sem->mutex);
    }
    sem->value--;
    

    pthread_mutex_unlock(sem->mutex);

}

/* This function should destroy any resources allocated for this
 * semaphore; this includes mutexes or condition variables. */
void csesem_destroy(CSE_Semaphore sem) {
    pthread_mutex_destroy(sem->mutex);
    pthread_cond_destroy(sem->cond);
    free(sem->mutex);
    free(sem->cond);
    free(sem);
}
