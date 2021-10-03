
/*
 * @author Amy Ma
 * @version 10.2.21
 * Completion time: 
 */

#include "PageTable.h"
#include <limits.h>

/*
Stores general information about a page table entry 
 */
struct page_table_entry {
    int isDirty;
    int isValid;
    int frameNumber;
};

/*
Stores general information about a page table object
 */
struct page_table {
    struct page_table_entry* table;
    struct queue* frames;
    int page_count;
    int frame_count;
    int fault_count;
    enum replacement_algorithm algorithm;
};

/*
 * Queue struct
 */
struct queue {
    int front, rear, size;
    int capacity;
    int* arr;
};

struct queue* init_queue(int capacity) {

    struct queue* queue = (struct queue*) malloc(sizeof (struct queue));

    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;
    int* array = (int*) malloc(sizeof (int) * queue->capacity);
    queue->arr = array;

    return queue;
}

int isFull(struct queue* queue) {
    if (queue->size == queue->capacity)
        return 1;
    else
        return 0;
}

int isEmpty(struct queue* queue) {
    if (queue->size == 0)
        return 1;
    else
        return 0;
}

void enqueue(struct queue* queue, int page) {

    printf("\t-- Adding page to queue..");
    if (isFull(queue)) {
        return;
    }

    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->arr[queue->rear] = page;
    queue->size = queue->size + 1;
    printf("Page %d added to the queue\n", page);

}

/*
 * removes item from queue
 */
void dequeue(struct queue* queue) {
    printf("Removing oldest item from queue..");
    if (isEmpty(queue))
        return INT_MIN;
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;
    printf("Removed\n");
}

/*
 * Function to replace oldest page in the queue with new page
 */
int FIFOreplace(struct queue* queue, int page) {

    printf("FIFO Replacement\n");
    if (isEmpty(queue)) {
        return INT_MIN;
    }

    int delpage = queue->arr[queue->front];
    queue->arr[queue->front] = page; // replaces current front with new content
    queue->rear = (queue->rear + 1) % queue->capacity;
    ; // rear (newest) is equal to the front (oldest)
    queue->front = (queue->front + 1) % queue->capacity; // changes front to next oldest

    printf("Page %d replaced with page %d successfully\n", delpage, page);
    return delpage;
}

/* 
 * Returns the newest page in the queue
 */
int newestPage(struct queue* queue) {
    if (isEmpty(queue)) {
        return INT_MIN;
    }

    //returns frame containing newest page in the queue
    //return queue->rear; 
    return queue->arr[queue->rear];
}

/* 
 * Returns the oldest page in the queue
 */
int oldestPage(struct queue* queue) {

    if (isEmpty(queue)) {
        return INT_MIN;
    }
    //returns frame containing newest page in the queue
    //return queue->front; 
    return queue->arr[queue->front];

}

/*
Initializes page table
 */
struct page_table* page_table_create(int page_count, int frame_count, enum replacement_algorithm algorithm, int verbose) {

    printf("Initializing page table\n");
    struct page_table* newTable = (struct page_table*) malloc(sizeof (struct page_table));

    newTable->table = (struct page_table_entry*) malloc(page_count * sizeof (struct page_table_entry));
    newTable->page_count = page_count;

    for (int i = 0; i < newTable->page_count; i++) {
        newTable->table[i].isValid = 0;
    }

    newTable->frames = init_queue(frame_count);
    newTable->frame_count = frame_count;
    newTable->fault_count = 0;
    newTable->algorithm = algorithm;
    printf("Page table created\n");

    return newTable;
}

/*
Shuts down the page table
 */
void page_table_destroy(struct page_table** pt) {
    printf("Destroying page table..\n");

    /*for(int i = 0; i < (*pt)->page_count; i++){
        free((*pt)->table[i]);
    }*/
    for (int i = 0; i < (*pt)->frames->size; i++) {
        dequeue((*pt)->frames);
    }

    free((*pt)->table);
    free((*pt)->frames);
    free(pt);
    pt = NULL;

    printf("Page table destroyed\n");
}

/*
Simulates the program accessing a particular page
 */
void page_table_access_page(struct page_table *pt, int page) {
    printf("Accessing page in page table..\n");
    
    if (isEmpty(pt->frames) == 1) {
        enqueue(pt->frames, page);
        pt->fault_count = pt->fault_count + 1;
        //printf("\t--- fault count check: %d\n", pt->fault_count);
        pt->table[page].frameNumber = pt->frames->rear;
        pt->table[page].isValid = 1;
        printf("\t-- Added to empty queue\n");
    } else {
        int i;
        for (i = 0; i < pt->frames->size; i++) {
            if (pt->frames->arr[i] == page) {
                printf("\t-- Page %d already exists in frame\n", page);
                return;
            }
        }
        if (isFull(pt->frames) == 0) {
            enqueue(pt->frames, page);
            pt->fault_count = pt->fault_count + 1;
            //printf("\t--- fault count check: %d\n", pt->fault_count);
            pt->table[page].frameNumber = pt->frames->rear;
            pt->table[page].isValid = 1;
            printf("\t-- Page %d added to frame %d\n", page, pt->frames->rear);
        }
        else if (isFull(pt->frames) == 1) {
            // FIFO implementation
            if (pt->algorithm == FIFO) {
                printf("\t- Entering FIFO\n");
                int replacePage = FIFOreplace(pt->frames, page);
                pt->table[replacePage].isValid = 0;
                //printf("page %d valid: %d\n", replacePage, pt->table[replacePage].isValid);
                pt->table[page].isValid = 1;
                pt->table[page].frameNumber = pt->table[replacePage].frameNumber;
                //printf("new page %d valid: %d\n", page, pt->table[page].isValid);
                pt->fault_count = pt->fault_count + 1;
                //printf("\t--- fault count check: %d\n", pt->fault_count);
            }
            
            
            // LRU implementation
            if (pt->algorithm == LRU) {

    }
        }
    }

    

    // MFU implementation
    if (pt->algorithm == MFU) {

    }

    printf("Page access complete\n");

}

/*
Displays page table replacement algorithm, number of page faults, and the current contents of the page table
 */
void page_table_display(struct page_table* pt) {
    char * mode;
    int i;
    if (pt->algorithm == FIFO) {
        mode = "FIFO";
    } else if (pt->algorithm == LRU) {
        mode = "LRU";
    } else if (pt->algorithm == MFU) {
        mode = "MFU";
    } else {
        printf("Invalid mode.");
        return;
    }
    printf("\n====Page Table====\n");
    printf("Mode: %s\n", mode);
    printf("Page faults: %d\n", pt->fault_count);
    printf("Page\tFrame\t| dirty\tvalid\n");
    for (i = 0; i < pt->page_count; i++) {
        printf("%5d\t%5d\t| %5d\t%5d\n",
                i,
                pt->table[i].frameNumber,
                pt->table[i].isDirty,
                pt->table[i].isValid);
    }


}

/*
Displays the current contents of the page table
 */
void page_table_display_contents(struct page_table *pt);


