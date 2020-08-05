#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <pthread.h>
#include <signal.h>
#include <limits.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

pthread_mutex_t mutex;
pthread_cond_t  not_empty;
pthread_t main_id;
int files_found;
long no_of_searching_threads;
int counter_index;
char search_term[256];
int start_search;
int stop_search;
int sigint;

struct Threads{
    pthread_t* thread_array;
    int* not_sleeping_array;
    int* counter;
};

struct Threads threads_info;

//The que implementation is from GeeksToGeeks.com

// A linked list (LL) node to store a queue entry
struct QNode {
    char path_name[PATH_MAX];
    struct QNode* next;
};

// The queue, front stores the front node of LL and rear stores the
// last node of LL
struct Queue {
    struct QNode *front, *rear;
};

// A utility function to create a new linked list node.
struct QNode* newNode(char* path)
{
    struct QNode* temp = (struct QNode*)malloc(sizeof(struct QNode));
    temp->next = NULL;
    strcpy(temp->path_name, path);
    return temp;
}

// A utility function to create an empty queue
struct Queue* createQueue()
{
    struct Queue* q = (struct Queue*)malloc(sizeof(struct Queue));
    q->front = q->rear = NULL;
    return q;
}

// The function to add a key k to q
void enQueue(struct Queue* q, char* path)
{
    // Create a new LL node
    struct QNode* temp = newNode(path);

    // If queue is empty, then new node is front and rear both
    if (q->rear == NULL) {
        q->front = q->rear = temp;
        return;
    }

    // Add the new node at the end of queue and change rear
    q->rear->next = temp;
    q->rear = temp;
}

// Function to remove a key from given queue q
void deQueue(struct Queue* q)
{
    // If queue is empty, return NULL.
    if (q->front == NULL)
        return;

    // Store previous front and move front one node ahead
    struct QNode* temp = q->front;

    q->front = q->front->next;

    // If front becomes NULL, then change rear also as NULL
    if (q->front == NULL)
        q->rear = NULL;
    free(temp);
}


void freeQue(struct Queue* q){
    while(q->front != NULL){
        // Store previous front and move front one node ahead
        struct QNode* temp = q->front;

        q->front = q->front->next;

        // If front becomes NULL, then change rear also as NULL
        if (q->front == NULL)
            q->rear = NULL;
        free(temp);
    }
    free(q);
}


void *thread_search(void *t) {
    int count_not_sleeping;
    int i;
    char cur_path[PATH_MAX];
    char cur_file_path[PATH_MAX];
    struct dirent *sd;
    int index = -1;
    struct stat cur_stat;
    DIR *cur_dir;
    char *sub_string;
    struct Queue *queue = (struct Queue *) t;
    int error_retval1 = 1;
    pthread_mutex_lock(&mutex);
    index = counter_index;
    threads_info.counter[index] = 0;
    counter_index++;
    pthread_mutex_unlock(&mutex);
    while(1) {
        //this part of the code is protected by a mutex lock because we don't want the queue to be accesses by more than one thread simultaneously
        pthread_mutex_lock(&mutex);
        //going to sleep when queue is empty or not all threads have been created yet. Also stop_search has to be 0
        while ((queue->front == NULL || start_search == 0) && stop_search == 0) {
            //each thread has it's own not_sleeping flag. not_sleeping is 1 iff thread isn't sleeping
            threads_info.not_sleeping_array[index] = 0;
            pthread_cond_wait(&not_empty, &mutex);
        }
        if(stop_search == 0) {
            threads_info.not_sleeping_array[index] = 1;
        }
        else{
            pthread_cond_broadcast(&not_empty);
            pthread_mutex_unlock(&mutex);
            pthread_exit(0);
        }
        //stop search is 0 -> keep searching (also queue isn't empty)
        strcpy(cur_path, queue->front->path_name);
        deQueue(queue);
        pthread_mutex_unlock(&mutex);
        //opening the directory, skipping if unable to open due to access rights and exiting if another error occurred
        cur_dir = opendir(cur_path);
        if (cur_dir == NULL) {
            if (errno != EACCES) {
                perror("Error in searching thread: opendir call");
                pthread_exit(&error_retval1);
            }
        }
        //if opendir was successful we iterate over the directory
        else {
            while ((sd = readdir(cur_dir)) != NULL) {
                //skipping "." and ".."
                if (strcmp(sd->d_name, ".") != 0 && strcmp(sd->d_name, "..") != 0) {
                    strcpy(cur_file_path, cur_path);
                    if (strcmp(sd->d_name, "/") != 0) {
                        strcat(cur_file_path, "/");
                    }
                    strcat(cur_file_path, sd->d_name);
                    //checking if dir or something else
                    if (lstat(cur_file_path, &cur_stat) != 0) {
                        perror("Error in searching thread: lstat call");
                        pthread_exit(&error_retval1);
                    }
                    //if dir, adding the directory to the queue while locking and unlocking the mutex
                    if (S_ISDIR(cur_stat.st_mode)) {
                        pthread_mutex_lock(&mutex);
                        enQueue(queue, cur_file_path);
                        pthread_cond_signal(&not_empty);
                        pthread_mutex_unlock(&mutex);
                        //if not dir, checking if file name contains the search term
                    } else {
                        sub_string = strstr(sd->d_name, search_term);
                        //if file contains search term increases the thread counter of finds by 1 and printing the full path name from root directory
                        if (sub_string != NULL) {
                            threads_info.counter[index]++;
                            printf("%s\n", cur_file_path);
                        }
                    }
                }
            }
            closedir(cur_dir);
        }
        count_not_sleeping = 0;
        //counting non sleeping threads while using the same mutex as before to make sure states don't change while counting
        pthread_mutex_lock(&mutex);
        if(!stop_search) {
            for (i = 0; i < no_of_searching_threads; i++) {
                if (threads_info.not_sleeping_array[i] == 1) {
                    count_not_sleeping++;
                }
            }
            //if all other threads are sleeping and queue is empty we should halt the search (stop_search is 1 iff we should stop)
            stop_search = (queue->front == NULL && count_not_sleeping <= 1);
        }
        //if stop_search is 1, we cancel all the searching threads and exit
        if (stop_search) {
            pthread_cond_broadcast(&not_empty);
            pthread_mutex_unlock(&mutex);
            pthread_exit(0);
        }
        pthread_mutex_unlock(&mutex);
        //repeating if stop_search is 0
    }

}

static void handler(int signum){
    int i;
    if(stop_search == 0){
        stop_search = 1;
    }
    //if main thread sends SIGINT to all the other threads and sums up the sums of files found by each thread
    if(pthread_self() == main_id){
        sigint = 1;
        if(threads_info.thread_array != NULL){
            for(i = 0; i < no_of_searching_threads; i++){
                pthread_kill(threads_info.thread_array[i], SIGINT);
            }
        }
        //waiting for the threads to die
        for(i=0; i<no_of_searching_threads; i++)
        {
            pthread_join(threads_info.thread_array[i], NULL);
        }
        for(i=0; i < no_of_searching_threads; i++){
            files_found += threads_info.counter[i];
        }
        printf("Search stopped, found %d files\n", files_found);
    }
    if(pthread_mutex_trylock(&mutex) == EBUSY){
        pthread_mutex_unlock(&mutex);
    }
    //exits thread in both cases
    pthread_exit(0);
}

int main(int argc, char* argv[] ) {
    DIR* dir;
    struct Queue* queue;
    int i;
    int thread_creation_ret_val;
    main_id = pthread_self();
    files_found = 0;
    struct sigaction sig;
    sig.sa_handler = handler;
    sig.sa_flags = SA_RESTART;
    start_search = 0;
    stop_search = 0;
    sigint = 0;
    if(sigaction(SIGINT, &sig, NULL) != 0){
        perror("");
        exit(1);
    }
    if(argc != 4){
        fprintf(stderr,"Wrong no. of arguments in main");
        exit(1);
    }
    strcpy(search_term, argv[2]);
    dir = opendir(argv[1]);
    if(dir == NULL){
        perror("");
        exit(1);
    }
    closedir(dir);
    no_of_searching_threads = strtol(argv[3],NULL, 10);
    if(no_of_searching_threads == 0){
        perror("Invalid thread no.\n");
        exit(1);
    }
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init (&not_empty, NULL);
    pthread_mutex_lock(&mutex);
    queue = createQueue();
    enQueue(queue, argv[1]);
    threads_info.thread_array = (pthread_t*)calloc(no_of_searching_threads,sizeof(pthread_t));
    threads_info.counter = (int*)calloc(no_of_searching_threads,sizeof(int));
    counter_index = 0;
    threads_info.not_sleeping_array = (int*)calloc(no_of_searching_threads,sizeof(int));
    pthread_mutex_unlock(&mutex);
    //creating threads
    for(i = 0; i < no_of_searching_threads; i++){
        thread_creation_ret_val = pthread_create(&threads_info.thread_array[i], NULL, thread_search, (void*)queue);
        if(thread_creation_ret_val != 0){
            perror("failed to create a thread");
            pthread_mutex_destroy(&mutex);
            pthread_cond_destroy(&not_empty);
            freeQue(queue);
            free(threads_info.not_sleeping_array);
            free(threads_info.thread_array);
            free(threads_info.counter);
            exit(1);
        }
    }
    //setting start_search to 1 to indicate that searching threads can start searching and waking them up
    start_search = 1;
    pthread_cond_broadcast(&not_empty);
    //waiting for all the searching threads to finish
    for(i=0; i<no_of_searching_threads; i++)
    {
        if(pthread_join(threads_info.thread_array[i], NULL) != 0){
            perror("");
        }
    }
    //summing the counts of finds
    for(i=0; i < no_of_searching_threads; i++){
        files_found += threads_info.counter[i];
    }
    //checking if sigint was called just in-case before printing
    if(sigint == 0) {
        printf("Done searching, found %d files\n", files_found);
    }
    pthread_mutex_unlock(&mutex);
    pthread_mutex_destroy(&mutex);
    //pthread_cond_destroy(&not_empty);
    freeQue(queue);
    free(threads_info.not_sleeping_array);
    free(threads_info.thread_array);
    free(threads_info.counter);
    exit(0);
}
