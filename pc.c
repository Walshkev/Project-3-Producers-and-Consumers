#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include "eventbuf.h"
#include <fcntl.h>

// Global variables
int num_producers;
int num_consumers;
int num_events;
int max_outstanding;
sem_t *spaces;
sem_t *items;
sem_t *buf_lock;

int quitting_time = 0;
struct eventbuf *eb;



// Add the missing include statement for the 'fcntl' library

sem_t *sem_open_temp(const char *name, int value)
{
    sem_t *sem;

    // Create the semaphore
    if ((sem = sem_open(name, O_CREAT, 0600, value)) == SEM_FAILED)
        return SEM_FAILED;

    // Unlink it so it will go away after this process exits
    if (sem_unlink(name) == -1) {
        sem_close(sem);
        return SEM_FAILED;
    }

    return sem;
}

// Producer function
void *producer(void *arg) {
    int producer_id = *(int *)arg;
    int event_number=0;

    for (event_number = 0; event_number < num_events; event_number++) {
        // Wait for space in the event buffer
        sem_wait(spaces);
        
        sem_wait(buf_lock);
        eventbuf_add(eb, ((producer_id * 100) + event_number));
        sem_post(buf_lock);
        printf("P%d: adding event %d\n", producer_id, ((producer_id * 100) + event_number));

        
        // Signal waiting consumer threads
        sem_post(items);

    }

    printf("P%d: exiting\n", producer_id);
    return NULL;
   
}

// Consumer function
void *consumer(void *arg) {
    int consumer_id = *(int *)arg;
    int event;

    while (1) {
        // Wait for an event to be available
        sem_wait(items);
        if ( quitting_time ){
            printf("C%d: exiting %d\n", consumer_id, event);
            return NULL;
        }
        sem_wait(buf_lock);
        event = eventbuf_get(eb);
        sem_post(buf_lock);
        

        printf("C%d: got event %d\n", consumer_id, event);
        sem_post(spaces);
    }

    // Print that the consumer is exiting
    printf("P%d: exiting\n", consumer_id);
    return NULL; 

   
}

int main(int argc, char *argv[]) {
    // Parse command line arguments
    if (argc != 5) {
        printf( "we need 5 arguments including the file \n");
        return 1;
    }

    num_producers = atoi(argv[1]);
    num_consumers = atoi(argv[2]);
    num_events = atoi(argv[3]);
    max_outstanding = atoi(argv[4]);

    // Create the event buffer

    eb = eventbuf_create();

    // Create semaphores
    spaces = sem_open_temp("spaces", max_outstanding);
    items = sem_open_temp("items", 0);
    buf_lock = sem_open_temp("buf_lock", 1);


    // Create producer threads
    pthread_t producer_threads[num_producers];
    int producer_ids[num_producers];
    for (int i = 0; i < num_producers; ++i) {
        producer_ids[i] = i;
        pthread_create(&producer_threads[i], NULL, producer, &producer_ids[i]);
    }

    // Create consumer threads
    pthread_t consumer_threads[num_consumers];
    int consumer_ids[num_consumers];
    for (int i = 0; i < num_consumers; ++i) {
        consumer_ids[i] = i;
        pthread_create(&consumer_threads[i], NULL, consumer, &consumer_ids[i]);
    }

    
    for (int i = 0; i < num_producers; ++i) {
        pthread_join(producer_threads[i], NULL);
    }

    // Notify all consumer threads that they're done
    quitting_time = 1;
    for (int i = 0; i < num_consumers; ++i) {
        sem_post(items);
    }

    // Wait for all consumer threads to complete
    for (int i = 0; i < num_consumers; ++i) {
        pthread_join(consumer_threads[i], NULL);
    }

    // Free the event buffer

    eventbuf_free(eb);

    // Close and unlink semaphores
    sem_close(spaces);
    sem_close(items);
   
    sem_unlink("spaces");
    sem_unlink("items");


    return 0;
}