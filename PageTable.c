/*
Stores general information about a page table entry 
*/
struct page_table_entry {};

/*
Stores general information about a page table object
*/
struct page_table {};

/*
Initializes page table
*/
struct page_table* page_table_create(int page_count, int frame_count, enum replacement_algorithm algorithm, int verbose);

/*
Shuts down the page table
*/
void page_table_destroy(struct page_table** pt);

/*
Simulates the program accessing a particular page
*/
void page_table_access_page(struct page_table *pt, int page);

/*
Displays page table replacement algorithm, number of page faults, and the current contents of the page table
*/
void page_table_display(struct page_table* pt);

/*
Displays the current contents of the page table
*/
