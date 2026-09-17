/*
Program name: program1_assign2
Program description: an email organizer that impliments priority using a maxheap. 
Inputs: txt file containing commands
Outputs: email information, number of unread emails 
Collaborators: N/A
Other sources: Gemini
Author: Ana Gonzalez Yuil
Creation Date: 9.14.26
Revision Date: 9.16.26
Revisions: N/A
*/

//all code below from Gemini
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_EMAILS 1000
#define MAX_LINE 256

// Structure to represent an Email
typedef struct {
    char sender[50];
    char subject[150];
    char date[11];
    int year;
    int month;
    int day;
    int sender_priority;
} Email;

// Structure to represent the Array-Based Max Heap
typedef struct {
    Email data[MAX_EMAILS]; //my comment: Email array with the name data and max size of 1000, serves as our "list"
    int size;
} MaxHeap;

// Function to determine sender priority (higher number = higher priority)
// my comment: use of strcmp to compare two strings; if the sender matches any of the 5 sender types, a priority is assigned accordingly
int get_sender_priority(const char *sender) {
    if (strcmp(sender, "Boss") == 0) return 5;
    if (strcmp(sender, "Subordinate") == 0) return 4;
    if (strcmp(sender, "Peer") == 0) return 3;
    if (strcmp(sender, "ImportantPerson") == 0) return 2;
    if (strcmp(sender, "OtherPerson") == 0) return 1;
    return 0; // Default/Unknown
}

// Compare two emails. Returns > 0 if email 'a' has higher priority than 'b',
// < 0 if lower, and 0 if equal.
int compare_emails(Email a, Email b) {
    // 1. Compare sender priority first
    if (a.sender_priority != b.sender_priority) {
        return a.sender_priority - b.sender_priority;
    }
    
    // 2. If sender priority is the same, compare dates (Newest date = higher priority)
    if (a.year != b.year) {
        return a.year - b.year;
    }
    if (a.month != b.month) {
        return a.month - b.month;
    }
    return a.day - b.day;
}

// Swap two email elements
void swap(Email *a, Email *b) { //my comment: pointers used to reference first value of array
    Email temp = *a;
    *a = *b;
    *b = temp;
}

// Insert an email into the max heap
void max_heap_insert(MaxHeap *heap, Email e) {
    if (heap->size >= MAX_EMAILS) { //my comment: heap->size is equal to (*heap).size
        printf("Error: Heap capacity exceeded.\n");
        return;
    }
    
    int i = heap->size++; //my comment: increment variable is initializd to heap size
    heap->data[i] = e;
    
    // Bubble up
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (compare_emails(heap->data[i], heap->data[parent]) > 0) {
            //my comment: if the current email has greater priority than its parent, swap positions
            swap(&heap->data[i], &heap->data[parent]);
            i = parent;
        } else {
            break; //my comment: exit when all is in order
        }
    }
}

// Heapify down helper for extraction
void max_heapify_down(MaxHeap *heap, int i) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    
    //my comment: the two if statements below compare the top of the heap to its two adjacent emails and compares priority, 
    //priority is reassigned if needed
    if (left < heap->size && compare_emails(heap->data[left], heap->data[largest]) > 0) {
        largest = left;
    }
    if (right < heap->size && compare_emails(heap->data[right], heap->data[largest]) > 0) {
        largest = right;
    }
    
    if (largest != i) {//my comment: if left or right email has higher priority than largest (highest priority email),
                       // swap them and check if downheap needed again with new largest email 
        swap(&heap->data[i], &heap->data[largest]);
        max_heapify_down(heap, largest);
    }
}

// Extract the highest priority email (READ command)
int max_heap_extract(MaxHeap *heap, Email *out) {
    if (heap->size <= 0) {
        return 0; // Empty heap
    }
    
    *out = heap->data[0];
    heap->data[0] = heap->data[--heap->size];
    max_heapify_down(heap, 0);
    return 1;
}

// Peek at the highest priority email without removing it (NEXT command)
int max_heap_peek(MaxHeap *heap, Email *out) {
    if (heap->size <= 0) {
        return 0;
    }
    *out = heap->data[0];
    return 1;
}

// Process command file inputs
void process_commands(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        return;
    }
    
    MaxHeap heap;
    heap.size = 0;
    
    char line[MAX_LINE]; //my comment: holds command line from file, max size 256
    while (fgets(line, sizeof(line), file)) { //my comment: while?
        // Remove trailing newline/carriage return characters
        line[strcspn(line, "\r\n")] = 0;
        
        // my comment: if-elseif block checks what kind of command 'line' is
        if (strncmp(line, "EMAIL", 5) == 0) {
            // Parse EMAIL command: EMAIL <sender category>, <subject line>, <MM-DD-YYYY>
            char *content = line + 6; // Skip "EMAIL "
            char *comma1 = strchr(content, ','); //my comment: returns a pointer to first instance of char (,)
            char *comma2 = strrchr(content, ','); //my commenr: returns a pointer to last instance of char (,)
            
            if (comma1 && comma2 && comma1 != comma2) {
                Email new_email;
                
                // Extract Sender
                int len_sender = comma1 - content;
                strncpy(new_email.sender, content, len_sender);
                new_email.sender[len_sender] = '\0';
                
                // Extract Subject (trim surrounding spaces)
                char *subj_start = comma1 + 1;
                while (*subj_start == ' ') subj_start++;
                int len_subj = comma2 - subj_start;
                while (len_subj > 0 && subj_start[len_subj - 1] == ' ') len_subj--;
                strncpy(new_email.subject, subj_start, len_subj);
                new_email.subject[len_subj] = '\0';
                
                // Extract Date
                char *date_start = comma2 + 1;
                while (*date_start == ' ') date_start++;
                strcpy(new_email.date, date_start);
                
                // Parse Date numbers for comparison (MM-DD-YYYY)
                sscanf(new_email.date, "%d-%d-%d", &new_email.month, &new_email.day, &new_email.year);
                
                // Set Priority weight based on sender
                new_email.sender_priority = get_sender_priority(new_email.sender);
                
                // Insert into Max Heap
                max_heap_insert(&heap, new_email);
            }
        } 
        else if (strcmp(line, "NEXT") == 0) {
            Email next_email;
            if (max_heap_peek(&heap, &next_email)) {
                printf("Next Email:\n");
                printf("Sender: %s\n", next_email.sender);
                printf("Subject: %s\n", next_email.subject);
                printf("Date: %s\n", next_email.date);
            } else {
                printf("Next Email:\nNo unread emails.\n");
            }
        } 
        else if (strcmp(line, "READ") == 0) {
            Email read_email;
            max_heap_extract(&heap, &read_email);
        } 
        else if (strcmp(line, "COUNT") == 0) {
            printf("Unread Emails Count: %d\n", heap.size);
        }
    }
    
    fclose(file);
}

int main(int argc, char *argv[]) { //NEEDS SOME FORM OF USER INPUT
    if (argc < 2) {
        printf("Usage: %s <test.txt>\n", argv[0]);
        return 1;
    }
    
    process_commands(argv[1]);
    
    return 0;
}