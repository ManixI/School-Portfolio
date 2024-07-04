#ifndef MAIN_H
#define MAIN_H

#include <time.h>
// page in memory
typedef struct Memory_Entry {
    struct Memory_Entry* next;
    struct Memory_Entry* prev;
    int id;
    int is_modified;
    int last_accessed; // action num at whih this was last accessed
} Memory_Entry;

// head of list of everything in memory
typedef struct Memory {
    int max_size;
    int current_size;
    Memory_Entry* head;
    Memory_Entry* tail;
} Memory;

// entry in page actions to take
typedef struct Page_Action_Entry {
    struct Page_Action_Entry* next;
    struct Page_Action_Entry* prev;
    int read;       // read is 1, write is 0
    int page_target;
} Page_Action_Entry;

// list of page actions to take
typedef struct Page_Actions {
    Page_Action_Entry *head;
    Page_Action_Entry *tail;
} Page_Actions;



#endif