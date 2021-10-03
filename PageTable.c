
/*
 * @author Amy Ma
 * @version 10.2.21
 * Completion time: 
 */

#include "PageTable.h"


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
    int* frames;
    int page_count;
    int frame_count;
    int fault_count;
    enum replacement_algorithm algorithm;
};

/*
Initializes page table
*/
struct page_table* page_table_create(int page_count, int frame_count, enum replacement_algorithm algorithm, int verbose){
    
    printf("Initializing page table\n");
    struct page_table* newTable = (struct page_table*) malloc(sizeof(struct page_table));
    
    struct page_table_entry* pages = (struct page_table_entry*) malloc(page_count * sizeof(struct page_table_entry*));
    for (int i = 0; i < page_count; i++){
        pages[i].isValid = 0;
    }
    int* frames = (int*) malloc(frame_count * sizeof(int*));
    // initialize frames to be empty/have no pages
    for (int i = 0; i < frame_count; i++){
       frames[i] = NULL;
    }
    
    newTable->frames = frames;
    newTable->frame_count = frame_count;
    newTable->table = pages;
    newTable->page_count = page_count;
    newTable->fault_count = 0;
    newTable->algorithm = algorithm;
    printf("Page table created\n");
    
    return newTable;
}

/*
Shuts down the page table
*/
void page_table_destroy(struct page_table** pt){
    printf("Destroying page table..\n");
    
    printf("Page table destroyed\n");
}

/*
Simulates the program accessing a particular page
*/
void page_table_access_page(struct page_table *pt, int page) {
    printf("Accessing page in page table..\n");
    
    // FIFO implementation
    if(pt->algorithm == FIFO){
        printf("\t- Entering FIFO\n");
        int swap = 1;
        for(int i = 0; i < pt->frame_count; i++){
            // if there is space in frames
            if (pt->frames[i] == NULL){
                pt->frames[i] = page;
                pt->fault_count = (pt->fault_count)++;
                pt->table[page].frameNumber = i;
                pt->table[page].isValid = 1;
                pt->table[page].isDirty = 0;
                swap = 0;
                break;
            }
            // if a page currently exists within a frame
            if (pt->frames[i] == page){
                swap = 0;
                break;
            }
        }
        
        if (swap == 1) {
            
        }
        
        
    }
    
    // LRU implementation
    if(pt->algorithm == LRU){
        
    }
    
    // MFU implementation
    if(pt->algorithm == MFU){
        
    }
    
    printf("Page access complete\n");
    
}

/*
Displays page table replacement algorithm, number of page faults, and the current contents of the page table
*/
void page_table_display(struct page_table* pt);

/*
Displays the current contents of the page table
*/
void page_table_display_contents(struct page_table *pt);


