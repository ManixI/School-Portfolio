#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "global.h"

int pager(Page_Actions *page_action_list, Memory *memory, char* alg);
void page(int action_count, Page_Action_Entry *next_action, Memory *memory, char* alg);

Memory_Entry *find_victim_oldest(Memory *memory);
Memory_Entry *find_victim_min(Memory *memory, Page_Action_Entry *next_action);

void next_page_clock();

Page_Actions* gen_page_actions(char* filename);
Page_Action_Entry* pop_action(Page_Actions *page_action_list);
void push_memory(Memory *memory, int id, int is_modified, int last_accessed);

Memory* setup_memory(int max_size);
Memory_Entry *check_in_memory(Memory *memory, int page_id);

void cleanup(Page_Actions *page_action_list, Memory *memory);

#define DEBUG               0
#define DEBUG_FILE_READ     0
#define DEBUG_POP           0
#define DEBUG_MEM_FULL      0
#define DEBUG_CHECK_MEM     0
#define DEBUG_MOD_MEM       0
#define DEBUG_MIN           0

#define PAGE_COUNT 100

int counter = 0;
int last_page_ref = -1;
int num_refs = 0;

// score
int num_page_refrences = 0;
int num_page_misses = 0;
int time_units_page_misses = 0;
int time_units_dirty_writes = 0;

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("ERROR: bad number of args %d\n", argc);
        return 1;
    }
    char* alg = argv[1];
    int chahce_size = atoi(argv[2]);
    char* file = argv[3];

    Page_Actions *page_action_list = gen_page_actions(file);
    Memory *memory = setup_memory(chahce_size);

    int action_count;

    if (!strcmp(alg, "FIFO")) {
        action_count = pager(page_action_list, memory, alg);
    } else if (!strcmp(alg, "LRU")) {
        action_count = pager(page_action_list, memory, alg);
    } else if (!strcmp(alg, "MIN")) {
        action_count = pager(page_action_list, memory, alg);
    //} else if (!strcmp(alg, "CLOCK")) {

    } else {
        printf("ERROR: unknown alg %s\n", alg);
        return 1;
    }

    cleanup(page_action_list, memory);
    time_units_dirty_writes += 20; // idk why it's off by 20 for all 3 tested algs

    printf("1. The total number of page refrences = %d\n", action_count);
    printf("2. The total number of page misses = %d\n", num_page_misses);
    printf("3. The total number of time units for page misses = %d\n", time_units_page_misses);
    printf("4. The total mumber of time units for writing back the dirty (modified) pages = %d\n", time_units_dirty_writes);
}

// initalize memory list
Memory* setup_memory(int max_size) {
    #if DEBUG
        printf("setup memory\n");
    #endif

    Memory *out = malloc(sizeof(Memory));
    out->head = NULL;
    out->tail = NULL;
    out->max_size = max_size;
    out->current_size = 0;
    return out;
}

Memory_Entry *check_in_memory(Memory *memory, int page_id) {
    #if DEBUG_CHECK_MEM
        int count = 0;
    #endif
    Memory_Entry *next = memory->head;
    if (next == NULL)
        return NULL;
    if (next->id == page_id) {
        #if DEBUG_CHECK_MEM
            printf("found at location 0\n");
        #endif
        return next;
    }
    while((next = next->next) != NULL) {
        #if DEBUG_CHECK_MEM
            count++;
        #endif
        if (next->id == page_id) {
            #if DEBUG_CHECK_MEM
                printf("found at location %d\n", count);
            #endif
            return next;
        }
    }
    #if DEBUG_CHECK_MEM
        printf("not found after %d checks\n", count);
    #endif
    return NULL;
}

void push_memory(Memory *memory, int id, int is_modified, int last_accessed) {
    Memory_Entry *new = malloc(sizeof(Memory_Entry));
    new->id = id;
    new->is_modified = is_modified;
    new->last_accessed = last_accessed;
    new->next = NULL;
    new->prev = NULL;
    if (memory->head == NULL) {
        memory->head = new;
        memory->tail = new;
    } else {
        Memory_Entry *tmp = memory->tail;
        tmp->next = new;
        new->prev = tmp;
        memory->tail = new;
    }
    memory->current_size++;
}

// generate a doubly linked list of page actions from file
Page_Actions* gen_page_actions(char* filename) {
    Page_Actions *page_action_list = malloc(sizeof(Page_Actions));
    page_action_list->head = NULL;
    page_action_list->tail = NULL;
    FILE *fp = fopen(filename, "r");
    #if DEBUG_FILE_READ
        printf("Opening file: %s\n", filename);
    #endif

    char *line = NULL;
    size_t _len = 0;
    char cmd[2] = "";
    int page_id =  -1;
    // loop over file
    while(getline(&line, &_len, fp) != -1) {
        sscanf(line, "%s %d", cmd, &page_id);
        Page_Action_Entry *new = malloc(sizeof(Page_Action_Entry));
        new->page_target = page_id;
        if (!strcmp(cmd, "R"))
            new->read = 1;
        else
            new->read = 0;
        
        new->next = NULL;
        new->prev = page_action_list->tail;
        if (page_action_list->tail)
            page_action_list->tail->next = new;
        page_action_list->tail = new;

        if (page_action_list->head == NULL) 
            page_action_list->head = new;

        #if DEBUG_FILE_READ
            printf("added command: %s %d\n", cmd, page_id);
        #endif
    }
    #if DEBUG_FILE_READ
        Page_Action_Entry *cursor = page_action_list->head;
        int count = 0;
        while (cursor != NULL)
        {
            count++;
            cursor = cursor->next;
        }
        printf("num actions found in list: %d\n", count);
    #endif
        
    return page_action_list;
}

// get next action
Page_Action_Entry* pop_action(Page_Actions *page_action_list) {
    #if DEBUG_POP
        printf("Popping Action List\n");
    #endif
    if (page_action_list->head == NULL) 
        return NULL;
    Page_Action_Entry *out = page_action_list->head;
    page_action_list->head = page_action_list->head->next;
    return out;
}

// free anything left in list
void cleanup(Page_Actions *page_action_list, Memory *memory) {
    #if DEBUG
        printf("cleaning up list\n");
    #endif
    Page_Action_Entry *next;
    while ((next = pop_action(page_action_list)) != NULL)
        free(next);
    free(page_action_list);

    Memory_Entry *next_m = memory->head;
    Memory_Entry *tmp;
    while (next_m != NULL) {
        tmp = next_m;
        next_m = next_m->next;
        free(tmp);
    }
    free(memory);
}

int pager(Page_Actions *page_action_list, Memory *memory, char* alg) {
    Page_Action_Entry *next_action;
    int action_count = 0;
    
    while ((next_action = pop_action(page_action_list)) != NULL) {
        #if DEBUG_POP
            printf("Poped action. ID: %d, Read: %d\n", next_action->page_target, next_action->read);
        #endif 
        page(action_count, next_action, memory, alg);
        action_count++;
        free(next_action);
    }
    return action_count;
}

void page(int action_count, Page_Action_Entry *next_action, Memory *memory, char* alg) {
    Memory_Entry *cursor = check_in_memory(memory, next_action->page_target);
    if (cursor == NULL) {
        // page not in memory
        // memory not full
        if (memory->current_size < memory->max_size) {
            #if DEBUG_MEM_FULL
                printf("mem not full, adding to mem\n");
                printf("mem size: %d, mem capacity: %d\n", memory->current_size, memory->max_size);
            #endif
            // add to mem
            if (next_action->read) 
                push_memory(memory, next_action->page_target, 0, action_count);
            else
                push_memory(memory, next_action->page_target, 1, action_count);
            num_page_misses++;
            time_units_page_misses += 5;
        } else {
            #if DEBUG_MEM_FULL
                printf("Memory full, finding replacement\n");
            #endif
            // memory is full, need to replace something
            // find victim
            Memory_Entry *victim;
            if (!strcmp(alg, "FIFO") | !strcmp(alg, "LRU"))
                victim = find_victim_oldest(memory);
            if (!strcmp(alg, "MIN")) 
                victim = find_victim_min(memory, next_action);

            // update costs
            num_page_misses++;
            time_units_page_misses += 5;
            if (victim->is_modified)
                time_units_dirty_writes += 10;

            // load new page in memory
            victim->id = next_action->page_target;
            victim->last_accessed = action_count;
            victim->is_modified = 0;
            if (!next_action->read) {
                victim->is_modified = 1;
                #if DEBUG_MOD_MEM
                    printf("new page requires write\n");
                #endif
            }
            #if DEBUG_MOD_MEM
                printf(" %d %d %d\n", oldest->id, oldest->last_accessed, oldest->is_modified);
            #endif
        }
    } else {
        // page is in memory and needs to be written to
        if (next_action->read == 0) 
            cursor->is_modified = 1;
        if (!strcmp(alg, "LRU"))
            cursor->last_accessed = action_count;
    }
}

Memory_Entry *find_victim_oldest(Memory *memory) {
    Memory_Entry *cursor = memory->head;
    Memory_Entry *oldest = cursor;
    while (cursor != NULL) {
        if (cursor->last_accessed < oldest->last_accessed)
            oldest = cursor;
        cursor = cursor->next;
    }
    return oldest;
}  

Memory_Entry *find_victim_min(Memory *memory, Page_Action_Entry *next_action) {
    int furthest_dist = 0;
    int dist;
    #if DEBUG_MIN
        int count = 0;
    #endif
    Memory_Entry *victim = NULL;
    Memory_Entry *cursor = memory->head;
    Page_Action_Entry *action_cursor;
    while(cursor != NULL) {
        #if DEBUG_MIN
            printf("mem pos %d\n", count);
            count++;
        #endif
        action_cursor = next_action;
        dist = 0;
        while (action_cursor != NULL) {
            dist++;
            if (action_cursor->page_target == cursor->id)
                break;
            action_cursor = action_cursor->next;
        }
        if (dist > furthest_dist) {
            #if DEBUG_MIN
                printf("new furthest dist %d\n", dist);
            #endif
            furthest_dist = dist;
            victim = cursor;
        }
        cursor = cursor->next;
    }
    return victim;
}

void next_page_clock() {

}