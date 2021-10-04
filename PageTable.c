
/*
 * @author Amy Ma
 * @version 10.3.21
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
    int use_count;
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
    int front, rear, size, top;
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
    queue->top = -1;

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

/*
 * Adds item to queue
 */
void enqueue(struct queue* queue, int page) {

    //printf("\t-- Adding page to queue..");
    if (isFull(queue)) {
        return;
    }

    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->arr[queue->rear] = page;
    queue->size = queue->size + 1;
    //printf("Page %d added to the queue\n", page);
}

/*
 * Adds item to stack
 */
void push(struct queue* queue, int page) {

    //printf("\t-- Pushing page to stack..");
    if (isFull(queue)) {
        return;
    }

    queue->arr[++queue->top] = page;
    queue->size = queue->size + 1;
    //printf("Page %d pushed to stack top @ %d\n", page, queue->top);

}

void swap(struct queue* queue, int page) {
    
    int swaps;
    
    for (int i = 0; i < queue->size; i++) {
        if (queue->arr[i] == page) {
            //printf("Performing stack swap\n");
            swaps = queue->size - (i + 1);
        }
    }
    
    for (int i = 0; i < swaps; i++) {
        //printf("Swap run #%d\n", i);
        int temp = queue->arr[i];
        queue->arr[i] = queue->arr[i + 1];
        queue->arr[i + 1] = temp;
        //printf("Swapped arr[%d] and arr[%d] to %d, %d\n", 
                //i, (i + 1), queue->arr[i+1], queue->arr[i]);
    }
    //printf("Bottom: ");
    /*for (int i = 0; i < queue->size; i++){
        printf(" %d", queue->arr[i]);
    }*/
    
    //printf("\nNew Top: %d\n", queue->arr[queue->top]);
}

/*
 * removes item from queue
 */
int dequeue(struct queue* queue) {
    //printf("Removing oldest item from queue..");
    if (isEmpty(queue))
        return INT_MIN;
    int item = queue->arr[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;
    return item;
    //printf("Removed\n");
}

/*
 * removes item from stack
 */
int pop(struct queue* queue){
    if(isEmpty(queue))
        return INT_MIN;
    return queue->arr[queue->top--];
}


int LRUreplace(struct queue* queue, int page){
    //printf("\t++ LRU Replacement\n");

    int bottom, tempTop, bottomPage;
    
    /*tempTop = queue->top;
    for (int i = 0; i < queue->size; i++) {
        bottom = queue->arr[tempTop--];
    }*/
    bottom = 0;
    //printf("\t+++ bottom FRAME: %d\n", bottom);
    bottomPage = queue->arr[bottom];
    //printf("\t+++ current bottom page: %d\n", queue->arr[bottom]);
    queue->arr[bottom] = page;
   // printf("\t+++ new bottom page: %d\n", queue->arr[bottom]);
    int swaps = queue->size - (bottom + 1);
    for (int i = bottom; i < swaps; i++) {
        //printf("\t+++ Swap run #%d\n", i);
        int temp = queue->arr[i];
        queue->arr[i] = queue->arr[i + 1];
        queue->arr[i + 1] = temp;
        //printf("\t++++ Swapped arr[%d] and arr[%d] to %d, %d\n", 
               //i, (i + 1), queue->arr[i+1], queue->arr[i]);
    }
    //printf("Bottom: ");
    for (int i = bottom; i < queue->size; i++){
        //printf(" %d", queue->arr[i]);
    }
    
    //printf("\n\t+++ new top: %d\n", queue->arr[queue->top]);
    return bottomPage;
}

int MFUreplace(struct queue* queue, int page){
    //printf("\t++ MFU Replacement\n");

    int bottom, tempTop, bottomPage;
    
    tempTop = queue->top;
    for (int i = 0; i < queue->size; i++) {
        bottom = queue->arr[tempTop--];
    }
    //printf("bottom page: %d\n", bottom);
    
    bottomPage = queue->arr[bottom];
    queue->arr[bottom] = page;
    int swaps = queue->size - (bottom + 1);
    for (int i = 0; i < swaps; i++) {
        //printf("Swap run #%d\n", i);
        int temp = queue->arr[i];
        queue->arr[i] = queue->arr[i + 1];
        queue->arr[i + 1] = temp;
        //printf("Swapped arr[%d] and arr[%d] to %d, %d\n", 
                //i, (i + 1), queue->arr[i+1], queue->arr[i]);
    }
    //printf("new top: %d\n", queue->arr[queue->top]);
    return bottom;
}



/*
 * Function to replace oldest page in the queue with new page
 */
int FIFOreplace(struct queue* queue, int page) {

    //printf("\t++ FIFO Replacement\n");

    int delpage = queue->arr[queue->front];
    queue->arr[queue->front] = page; // replaces current front with new content
    queue->rear = (queue->rear + 1) % queue->capacity; // rear (newest) is equal to the front (oldest)
    queue->front = (queue->front + 1) % queue->capacity; // changes front to next oldest

    //printf("Page %d replaced with page %d successfully\n", delpage, page);
    return delpage;
}


/*
Initializes page table
 */
struct page_table* page_table_create(int page_count, int frame_count, enum replacement_algorithm algorithm, int verbose) {

    //printf("Initializing page table\n");
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
    //printf("Page table created\n");

    return newTable;
}

/*
Shuts down the page table
 */
void page_table_destroy(struct page_table** pt) {
    printf("Destroying page table..\n");
    
    /*if ((*pt)->algorithm == FIFO) {
        
        for (int i = 0; i < (*pt)->frames->size; i++) {
        free((pt)->frames->arr[dequeue((pt)->frames)]);    
        }
        
    } else {
        for (int i =0; i < (*pt)->frames->size; i++){
            free(*pt->frames->arr[pop((*pt)->frames)]);   
        }
    }
   */
    free(pt);
    pt = NULL;

    printf("Page table destroyed\n");
}

/*
Simulates the program accessing a particular page
 */
void page_table_access_page(struct page_table *pt, int page) {
    //printf("Accessing page in page table..\n");

    if (isEmpty(pt->frames) == 1) {
        if (pt->algorithm == FIFO) {
            enqueue(pt->frames, page);
            pt->table[page].frameNumber = pt->frames->rear;
            //printf("\t-- Added to empty queue\n");
        } else {
            push(pt->frames, page);
            pt->table[page].frameNumber = pt->frames->top;
            //printf("\t-- Added to empty stack\n");
        }

        pt->fault_count = pt->fault_count + 1;
        //printf("\t--- fault count check: %d\n", pt->fault_count);        
        pt->table[page].isValid = 1;
        pt->table[page].use_count = 0;

    } else {
        for (int i = 0; i < pt->frames->size; i++) {
            if (pt->frames->arr[i] == page) {
                //printf("\t-- Page %d already exists in frame\n", page);
                pt->table[page].use_count = pt->table[page].use_count + 1;
                //printf("Use count for %d: %d\n", page, pt->table[page].use_count);
                // Update for LRU, updates current item to "newest" and 
                //makes adjustments for front = rear case
                if (pt->algorithm == LRU) {
                    swap(pt->frames, page);
                }
                return;
            }
        }

        if (isFull(pt->frames) == 0) {
            if (pt->algorithm == FIFO) {
                enqueue(pt->frames, page);
                pt->table[page].frameNumber = pt->frames->rear;
                //printf("\t-- Page %d added to frame %d\n", page, pt->frames->rear);
            }
            else {
                push(pt->frames, page);
                pt->table[page].frameNumber = pt->frames->top;
                //printf("\t-- Page %d added to frame %d\n", page, pt->frames->top);
            }
            pt->fault_count = pt->fault_count + 1;
            //printf("\t--- fault count check: %d\n", pt->fault_count);
            pt->table[page].isValid = 1;
            pt->table[page].use_count = 0;
            
        } else if (isFull(pt->frames) == 1) {

            // FIFO implementation
            if (pt->algorithm == FIFO) {
                //printf("\t- Entering FIFO\n");
                int replacePage = FIFOreplace(pt->frames, page);
                pt->table[replacePage].isValid = 0;
                //printf("page %d valid: %d\n", replacePage, pt->table[replacePage].isValid);
                pt->table[page].isValid = 1;
                pt->table[page].frameNumber = pt->table[replacePage].frameNumber;
                pt->table[page].use_count = 0;
                //printf("new page %d valid: %d\n", page, pt->table[page].isValid);
                pt->fault_count = pt->fault_count + 1;
                //printf("\t--- fault count check: %d\n", pt->fault_count);
            }

            // LRU implementation

            if (pt->algorithm == LRU) {
                //printf("\t- Entering LRU\n");
                int replacePage = LRUreplace(pt->frames, page);
                //printf("\t+ Replace page number: %d\n", replacePage);
                pt->table[replacePage].isValid = 0;
                pt->table[page].isValid = 1;
                pt->table[page].frameNumber = pt->table[replacePage].frameNumber;
                pt->table[page].use_count = 0;
                pt->fault_count = pt->fault_count + 1;
            }
            
            // MFU implementation
            if (pt->algorithm == MFU) {
                if (pt->algorithm == MFU) {
                //printf("\t- Entering MFU\n");
                int replacePage = MFUreplace(pt->frames, page);
                //printf("\t+ Replace page number: %d\n", replacePage);
                pt->table[replacePage].isValid = 0;
                pt->table[page].isValid = 1;
                pt->table[page].frameNumber = pt->table[replacePage].frameNumber;
                pt->table[page].use_count = 0;
                pt->fault_count = pt->fault_count + 1;
            }

            }

            //printf("Page access complete\n");

        }
    }
}

/*
Displays page table replacement algorithm, number of page faults, and the current contents of the page table
 */
void page_table_display(struct page_table * pt) {
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
void page_table_display_contents(struct page_table * pt);


