#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{


   struct thread_data* thread_d = (struct thread_data*) thread_param;
   usleep ((useconds_t) thread_d->wait_to_obtain_ms * 1000);
    int rc = pthread_mutex_lock(thread_d->mutex);
    if ( rc != 0 ) {
        ERROR_LOG("pthread_mutex_lock failed with %d\n", rc);
    }
    usleep ((useconds_t) thread_d->wait_to_release_ms * 1000);
    rc = pthread_mutex_unlock(thread_d->mutex);
    if ( rc != 0 ) {
        ERROR_LOG("pthread_mutex_unlock failed with %d\n", rc);
    }
    if (0 == rc) {
        thread_d->thread_complete_success = true;
    }
    return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex, int wait_to_obtain_ms, int wait_to_release_ms)
{
    struct thread_data* thread_d = malloc(sizeof(struct thread_data));
    if (thread_d == NULL) {
        ERROR_LOG("error when allocating memory for thread structure");
        return false;
    }

    thread_d->mutex = mutex;
    thread_d->wait_to_obtain_ms = wait_to_obtain_ms;
    thread_d->wait_to_release_ms = wait_to_release_ms;
    thread_d->thread_complete_success = false;

    int rc = pthread_create(thread, NULL, threadfunc, thread_d);
    if ( rc != 0 ) {
        ERROR_LOG("pthread_create failed with error %d creating thread id %p\n", rc, thread);
        return false;
    }

    return true;
}

