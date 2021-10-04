
/*
 * Purpose: A file containing a set of functions to load a data file.
 * @author Amy Ma
 * @version 10.3.21
 * Completion time: 30 min
 */

#include "DataLoader.h"

/*
Loads the data file and returns a struct containing its information
*/
struct test_scenario* load_test_data(char* filename) {
    
    //printf("Loading test scenario...\n");
    struct test_scenario* testData = (struct test_scenario*) malloc(sizeof(struct test_scenario));
    
    //printf("\t- Opening file..");
    FILE* readFile = fopen(filename, "r");
    //printf("File opened.\n");
    //printf("\t- Attempting read..\n");
    fscanf(readFile, "%d", &testData->page_count);
    //printf("\t-- Page count: %d\n", testData->page_count);
    fscanf(readFile, "%d", &testData->frame_count);
    //printf("\t-- Frame count: %d\n", testData->frame_count);
    fscanf(readFile, "%d", &testData->refstr_len);
    //printf("\t-- Ref str length: %d\n", testData->refstr_len);
    
    int i;   
    for(i = 0; i < testData->refstr_len; i++){
        fscanf(readFile, "%d", &testData->refstr[i]);
        //printf("\t-- Ref str at page %d: %d\n", i, testData->refstr[i]);
    }
       
    //printf("Test scenario loaded. \n");
    //printf("\t-Closing file..");
    fclose(readFile);
    //printf("File closed\n");
    return testData;
}
