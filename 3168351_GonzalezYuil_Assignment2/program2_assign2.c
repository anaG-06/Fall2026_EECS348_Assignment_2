/*
Program name: program2_assign2
Program description: an email organizer that impliments priority using a maxheap. 
Inputs: txt file containing commands
Outputs: email information, number of unread emails  
Collaborators: N/A
Other sources: Copilot
Author: Ana Gonzalez Yuil
Creation Date: 9.14.26
Revision Date: 9.17.26
Revisions: added detailed comments
*/

//all code below from copilot
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_EMAILS 1000
#define MAX_LINE 512

//myC: Email struct holds sender, subject, and dates with size limit on each
typedef struct {
    char sender[50];
    char subject[200];
    char date[11];   // MM-DD-YYYY
} Email;
// myC: Maxheap struct holds an Email array and its size
typedef struct {
    Email heap[MAX_EMAILS];
    int size;
} MaxHeap;

/*--------------------------------------------------
    Utility Functions
--------------------------------------------------*/

//myC: function below determines sender priority by comparing the sender str to the accepted sender types 
int getSenderPriority(const char *sender) {
    if (strcmp(sender, "Boss") == 0)
        return 5;
    if (strcmp(sender, "Subordinate") == 0)
        return 4;
    if (strcmp(sender, "Peer") == 0)
        return 3;
    if (strcmp(sender, "ImportantPerson") == 0)
        return 2;
    if (strcmp(sender, "OtherPerson") == 0)
        return 1;

    return 0;
}

/*
    Converts MM-DD-YYYY into YYYYMMDD integer
    for easy date comparison.
*/
int convertDate(const char *date) {
    int month, day, year;

    sscanf(date, "%d-%d-%d", &month, &day, &year);

    return year * 10000 + month * 100 + day;
}

/*
    Returns:
    >0 if e1 has higher priority
    <0 if e2 has higher priority
    0 if equal
*/
int compareEmails(Email e1, Email e2) {

    int p1 = getSenderPriority(e1.sender); //myC: priority of first email
    int p2 = getSenderPriority(e2.sender); //myC: priority of second email

    if (p1 != p2) //myC: if priorities differ, retunr diff
        return p1 - p2;

    //myC: otherwise, return the difference of their dates
    int d1 = convertDate(e1.date);
    int d2 = convertDate(e2.date);

    return d1 - d2;
}

// myC: basic swapping funciton. to be used for MaxHeap functions
void swap(Email *a, Email *b) {
    Email temp = *a;
    *a = *b;
    *b = temp;
}

/*--------------------------------------------------
    Heap Functions
--------------------------------------------------*/


void heapifyUp(MaxHeap *h, int index) {

    while (index > 0) {

        int parent = (index - 1) / 2;
        
        //myC: if current email is higher pri than parent email
        if (compareEmails(h->heap[index],
                          h->heap[parent]) > 0) {

            swap(&h->heap[index],
                 &h->heap[parent]); //myC: swap 

            index = parent; //myC: move to index of former parent to follow email
        }
        else {
            break;
        }
    }
}

void heapifyDown(MaxHeap *h, int index) {

    while (1) { //myc: continously downheap until none of the conditions are met (fully ordered)

        int left = 2 * index + 1; //myc: calculates left of current index
        int right = 2 * index + 2;//myc: calculates right of current index
        int largest = index;

        // myC: if left index is less than the size of MaxHeap (max index) and 
        // left pri is greater than current pri
        if (left < h->size &&
            compareEmails(h->heap[left],
                          h->heap[largest]) > 0) {

            largest = left; //myC: the new highest pri is the email to the left
        }

        //myc: if right index is less than size of MaxHeap (max index) and 
        // right pri is greater than current pri
        if (right < h->size &&
            compareEmails(h->heap[right],
                          h->heap[largest]) > 0) {

            largest = right; //myc: new highest pri is the email to the right
        }

        if (largest != index) {//myc: if current is not the largest pri
            swap(&h->heap[index],
                 &h->heap[largest]); //myc: swap the current and largest pri emails

            index = largest;// assign largest index value to index
        }
        else {
            break;
        }
    }
}

void insertEmail(MaxHeap *h, Email email) {//myc: arguments: dereferenced heap pointer, email Struct

    if (h->size >= MAX_EMAILS) {//myc: if the size of heap is greater than or eq to max amt of emails
        printf("Heap is full.\n"); //myc: notify that heap if full, do not add to heap
        return;//myc: exit function 
    }

    h->heap[h->size] = email; //myc: add email to end of MaxHeap array
    heapifyUp(h, h->size); //myc: heapify email as needed

    h->size++;//myc: increment heap size
}

Email removeMax(MaxHeap *h) {

    Email emptyEmail = {"", "", ""}; //myc: create empty email placeholder

    if (h->size == 0) //myc: if heap is empty, return placeholder, avoids garbage return
        return emptyEmail;

    //myc: otherwise
    Email maxEmail = h->heap[0]; //myc:create Email to store highest pri Email

    h->size--; //myc: decrement heap size

    h->heap[0] = h->heap[h->size]; //myc: remove and replace top email

    heapifyDown(h, 0); //myc:heapify down as needed to restore order

    return maxEmail; //myc: return highest pri email
}

Email getMax(MaxHeap *h) { //myc: equivalent to generic peek method for MaxHeaps 

    Email emptyEmail = {"", "", ""}; //myc: create empty Email placeholder

    if (h->size == 0) //myc: if heap is empty, return placeholder, avoids garbage return
        return emptyEmail;

    return h->heap[0]; //myc: otherwise return highest pri email w/o removing 
}

/*--------------------------------------------------
    Command Handlers
--------------------------------------------------*/

void processEMAIL(MaxHeap *heap, char *data) {

    Email email;

    char *token;

    //myC: EMAIL line is split using "," as a delimiter

    token = strtok(data, ",");
    if (token == NULL) return;
    strcpy(email.sender, token); //myc: copies sender string to Email struct sender 

    token = strtok(NULL, ",");
    if (token == NULL) return;
    strcpy(email.subject, token); //myc: copies subject string to Email struct subject 

    token = strtok(NULL, ",");
    if (token == NULL) return; 

    while (*token == ' ')
        token++;

    strcpy(email.date, token); //myc: copies date string to Email struct date

    insertEmail(heap, email); //myc: add fully structured email to heap
}

void processNEXT(MaxHeap *heap) {

    if (heap->size == 0) { //myc: if heap is empty
        printf("No unread emails.\n"); //myc: notify user and return, no next email to show
        return;
    }

    //myc: otherwise
    Email top = getMax(heap); //myc: get top pri email

    //myc: print with proper formatting
    printf("Next Email:\n");
    printf("Sender: %s\n", top.sender);
    printf("Subject: %s\n", top.subject);
    printf("Date: %s\n", top.date);
}

void processREAD(MaxHeap *heap) {

    if (heap->size == 0) { //myc: if heap is empty
        printf("No unread emails.\n"); //myc: notify user and return, no next email to process
        return;
    }

    removeMax(heap); //myc: remove top email
}

void processCOUNT(MaxHeap *heap) {
    printf("Unread Emails: %d\n", heap->size); //myc: return size of heap 
}

/*--------------------------------------------------
    Main
--------------------------------------------------*/

// int main(int argc, char *argv[]) {

int main(){

    // if (argc != 2) {
    //     printf("Usage: %s test.txt\n", argv[0]);
    //     return 1;
    // }

    // //printf and scanf added by Ana
    // char filename; 

    // printf("filename: ");
    // scanf("%d", &filename);

    FILE *fp = fopen("test.txt" , "r"); //<----- myc: manually add file name here

    if (fp == NULL) { //myc: if file given does not exist 
        printf("Unable to open file.\n");
        return 1;
    }

    MaxHeap heap; //myc: declare heap for current file being read
    heap.size = 0; //myc: initialize heap size

    char line[MAX_LINE]; //myc: size buffer for reading file lines

    while (fgets(line, sizeof(line), fp)) {

        line[strcspn(line, "\n")] = '\0';

        //myc: if statement block compares command names to determine which function to call
        if (strncmp(line, "EMAIL ", 6) == 0) {

            char data[MAX_LINE];
            strcpy(data, line + 6);

            processEMAIL(&heap, data);
        }
        else if (strcmp(line, "NEXT") == 0) {

            processNEXT(&heap);
        }
        else if (strcmp(line, "READ") == 0) {

            processREAD(&heap);
        }
        else if (strcmp(line, "COUNT") == 0) {

            processCOUNT(&heap);
        }
    }

    fclose(fp);
    return 0;
}