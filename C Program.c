#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define NAME_SIZE 50
#define SERVICE_SIZE 60
#define PRIORITY_SIZE 10
#define STATUS_SIZE 20

// Structure used to store student request information
typedef struct Student {
int id;
char name[NAME_SIZE];
char serviceType[SERVICE_SIZE];
char priority[PRIORITY_SIZE];
char status[STATUS_SIZE];
struct Student *next;
} Student;

// Queue structure for managing waiting students
typedef struct Queue {
Student *front;
Student *rear;
int count;
} Queue;

// Linked List structure for storing completed services
typedef struct LinkedList {
Student *head;
int count;
} LinkedList;

// Stack structure for supporting undo operations
typedef struct Stack {
Student *top;
int count;
} Stack;

// Global data structures used throughout the system
Queue serviceQueue;
LinkedList serviceHistory;
Stack recentServices;

/* ==========================================
HELPER FUNCTIONS
========================================== */

// Clear remaining characters from input buffer
void flushInput() {
int ch;

while ((ch = getchar()) != '\n' && ch != EOF);
}

// Read a line of text safely
void readText(char *buffer, int size) {
fgets(buffer, size, stdin);
buffer[strcspn(buffer, "\n")] = '\0';
}

// Creates a copy of a student record with a specified status
Student *cloneStudent(Student *source, const char *status) {

Student *record = malloc(sizeof(Student));

if (record == NULL) {
return NULL;
}

*record = *source;

strcpy(record->status, status);

record->next = NULL;

return record;
}

// Allow user to choose service type
void chooseService(char *serviceType) {

int option;

printf("\nSelect Service Type:\n");
printf("1. Course Registration Help\n");
printf("2. Exam Slip Printing\n");
printf("3. Fee Payment Inquiry\n");
printf("4. IT Account Support\n");
printf("5. General Academic Inquiry\n");
printf("Choice: ");

scanf("%d", &option);

flushInput();

switch(option) {

case 1:
strcpy(serviceType, "Course Registration Help");
break;

case 2:
strcpy(serviceType, "Exam Slip Printing");
break;

case 3:
strcpy(serviceType, "Fee Payment Inquiry");
break;

case 4:
strcpy(serviceType, "IT Account Support");
break;

default:
strcpy(serviceType, "General Academic Inquiry");
}
}

// Allow user to choose priority level
void choosePriority(char *priority) {

int option;

printf("\nPriority:\n");
printf("1. Normal\n");
printf("2. Urgent\n");
printf("Choice: ");

scanf("%d", &option);

flushInput();

strcpy(priority, option == 2 ? "Urgent" : "Normal");
}

// Collect all information for a new student request
Student getStudentInput() {

Student record;

printf("Student ID: ");
scanf("%d", &record.id);

flushInput();

printf("Student Name: ");
readText(record.name, NAME_SIZE);

chooseService(record.serviceType);
choosePriority(record.priority);

strcpy(record.status, "Waiting");

record.next = NULL;

return record;
}

/* ==========================================
QUEUE OPERATIONS
Used to manage students waiting for service
FIFO (First In First Out)
========================================== */

// Initialize queue before use
void initQueue(Queue *queue) {

queue->front = NULL;
queue->rear = NULL;
queue->count = 0;
}

// Check whether queue contains any records
bool isQueueEmpty(Queue *queue) {
return queue->front == NULL;
}

// Add a new student request to the rear of queue
void enqueue(Queue *queue, Student *record) {

Student *newRecord =
cloneStudent(record, "Waiting");

if (newRecord == NULL) {

printf("Memory allocation failed.\n");

return;
}

// If queue is empty, front and rear
// point to the same node
if (isQueueEmpty(queue)) {

queue->front = newRecord;
queue->rear = newRecord;
}
else {

// Insert at rear and update rear pointer
queue->rear->next = newRecord;
queue->rear = newRecord;
}

queue->count++;
}

// Remove the next student from the front of queue
Student *dequeue(Queue *queue) {

if (isQueueEmpty(queue)) {
return NULL;
}

Student *deletedRecord =
queue->front;

queue->front =
queue->front->next;

// Queue becomes empty after removal
if (queue->front == NULL) {
queue->rear = NULL;
}

deletedRecord->next = NULL;

queue->count--;

return deletedRecord;
}

// Display all students waiting for service
void showWaitingQueue(Queue *queue) {

if (isQueueEmpty(queue)) {

printf("\nWaiting queue is empty.\n");

return;
}

printf("\n===== WAITING QUEUE =====\n");

int position = 1;

for (Student *cursor = queue->front;
cursor != NULL;
cursor = cursor->next) {

printf("%d. %d | %s | %s | %s\n",
position++,
cursor->id,
cursor->name,
cursor->serviceType,
cursor->priority);
}

printf("Total waiting students: %d\n",
queue->count);
}

/* ==========================================
STACK OPERATIONS
Used for undo functionality
LIFO (Last In First Out)
========================================== */

// Initialize stack before use
void initStack(Stack *stack) {

stack->top = NULL;
stack->count = 0;
}

// Check whether stack contains records
bool isStackEmpty(Stack *stack) {
return stack->top == NULL;
}

// Push completed service onto stack
void push(Stack *stack, Student *record) {

Student *newRecord =
cloneStudent(record, "Completed");

if (newRecord == NULL) {
return;
}

newRecord->next = stack->top;
stack->top = newRecord;

stack->count++;
}

// Remove latest completed service
Student *pop(Stack *stack) {

if (isStackEmpty(stack)) {
return NULL;
}

Student *deletedRecord =
stack->top;

stack->top =
stack->top->next;

deletedRecord->next = NULL;

stack->count--;

return deletedRecord;
}

// View latest completed service without removing it
Student *peek(Stack *stack) {

if (isStackEmpty(stack)) {
return NULL;
}

return stack->top;
}
/* ==========================================
LINKED LIST OPERATIONS
Stores completed service records
========================================== */

// Initialize linked list before use
void initList(LinkedList *list) {

list->head = NULL;
list->count = 0;
}

// Insert completed service at beginning of linked list
void addServiceRecord(LinkedList *list, Student *record) {

Student *newRecord =
cloneStudent(record, "Completed");

if (newRecord == NULL) {
return;
}

// Insert new node at the head of the list
newRecord->next = list->head;
list->head = newRecord;

list->count++;
}

// Search completed service using student ID
Student *findServiceRecord(LinkedList *list,
int studentId) {

for (Student *cursor = list->head;
cursor != NULL;
cursor = cursor->next) {

if (cursor->id == studentId) {
return cursor;
}
}

return NULL;
}

// Remove completed record during undo operation
Student *deleteServiceRecord(LinkedList *list,
int studentId) {

Student *cursor = list->head;
Student *prevNode = NULL;

while (cursor != NULL) {

if (cursor->id == studentId) {

// Removing first node
if (prevNode == NULL) {
list->head = cursor->next;
}
else {
prevNode->next = cursor->next;
}

cursor->next = NULL;

list->count--;

return cursor;
}

prevNode = cursor;
cursor = cursor->next;
}

return NULL;
}

// Display all completed service records
void showCompletedHistory(LinkedList *list) {

if (list->head == NULL) {

printf("\nCompleted services list is empty.\n");

return;
}

printf("\n===== COMPLETED SERVICES =====\n");

int index = 1;

for (Student *cursor = list->head;
cursor != NULL;
cursor = cursor->next) {

printf("%d. %d | %s | %s | %s\n",
index++,
cursor->id,
cursor->name,
cursor->serviceType,
cursor->status);
}

printf("Total completed services: %d\n",
list->count);
}

/* ==========================================
SYSTEM FUNCTIONS
Connect Queue, Linked List and Stack
========================================== */

// Register a new student service request
void registerRequest() {

printf("\n===== NEW STUDENT REQUEST =====\n");

Student record =
getStudentInput();

enqueue(&serviceQueue, &record);

printf("\nRequest registered successfully.\n");
}

// Serve next student and move record to history
void processNextStudent() {

Student *servedStudent =
dequeue(&serviceQueue);

if (servedStudent == NULL) {

printf("\nNo students waiting for service.\n");

return;
}

// Move served student to completed history
addServiceRecord(&serviceHistory,
servedStudent);

// Save completed service for future undo
push(&recentServices,
servedStudent);

printf("\nStudent served successfully.\n");

printf("ID: %d\n",
servedStudent->id);

printf("Name: %s\n",
servedStudent->name);

free(servedStudent);
}

// Search completed service by student ID
void locateCompletedRecord() {

if (serviceHistory.head == NULL) {

printf("\nCompleted services list is empty.\n");

return;
}

int studentId;

printf("Enter Student ID: ");

scanf("%d", &studentId);

flushInput();

Student *targetRecord =
findServiceRecord(&serviceHistory,
studentId);

if (targetRecord == NULL) {

printf("\nStudent record not found.\n");

return;
}

printf("\nRecord Found\n");

printf("ID: %d\n",
targetRecord->id);

printf("Name: %s\n",
targetRecord->name);

printf("Service: %s\n",
targetRecord->serviceType);

printf("Status: %s\n",
targetRecord->status);
}

// Restore most recently completed service
void restoreLastService() {

// Remove latest completed service from stack
Student *lastRecord =
pop(&recentServices);

if (lastRecord == NULL) {

printf("\nNo completed service available for undo.\n");

return;
}

// Remove same record from linked list
Student *deletedRecord =
deleteServiceRecord(&serviceHistory,
lastRecord->id);

if (deletedRecord == NULL) {

free(lastRecord);

printf("\nUndo failed.\n");

return;
}

// Return student back to waiting queue
enqueue(&serviceQueue,
lastRecord);

printf("\nUndo Successful!\n");

printf("Returned to queue:\n");

printf("%d %s %s\n",
lastRecord->id,
lastRecord->name,
lastRecord->serviceType);

free(lastRecord);
free(deletedRecord);
}

// Display current statistics of the system
void showSummary() {

printf("\n===== SYSTEM SUMMARY =====\n");

printf("Students Waiting : %d\n",
serviceQueue.count);

printf("Completed Records : %d\n",
serviceHistory.count);

printf("Undo Stack Size : %d\n",
recentServices.count);

Student *latest =
peek(&recentServices);

if (latest == NULL) {

printf("Latest Service : None\n");

return;
}

printf("Latest Service : %s (%d)\n",
latest->name,
latest->id);
}

/* ==========================================
MEMORY CLEANUP FUNCTIONS
Release dynamically allocated memory
========================================== */

// Free all nodes stored in queue
void clearQueue(Queue *queue) {

while (!isQueueEmpty(queue)) {

Student *record =
dequeue(queue);

free(record);
}
}

// Free all nodes stored in linked list
void clearHistory(LinkedList *list) {

Student *cursor = list->head;

while (cursor != NULL) {

Student *nextNode =
cursor->next;

free(cursor);

cursor = nextNode;
}
}

// Free all nodes stored in stack
void clearStack(Stack *stack) {

while (!isStackEmpty(stack)) {

Student *record =
pop(stack);

free(record);
}
}

/* ==========================================
MENU SECTION
========================================== */

// Display all available menu options
void displayMenu() {

printf("\n====================================\n");
printf(" SMART STUDENT SERVICE CENTRE SYSTEM\n");
printf("====================================\n");
printf("1. Add new student request\n");
printf("2. Serve next student\n");
printf("3. Display waiting queue\n");
printf("4. Display completed services\n");
printf("5. Search completed service\n");
printf("6. Undo last completed service\n");
printf("7. Display system summary\n");
printf("8. Exit\n");
printf("====================================\n");
printf("Choice: ");
}

/* ==========================================
MAIN PROGRAM
========================================== */

int main() {

// Initialize all data structures
initQueue(&serviceQueue);
initList(&serviceHistory);
initStack(&recentServices);

int choice;

// Process menu options until user exits
do {

displayMenu();

if (scanf("%d", &choice) != 1) {

flushInput();

printf("\nInvalid input.\n");

continue;
}

flushInput();

switch (choice) {

case 1:
registerRequest();
break;

case 2:
processNextStudent();
break;

case 3:
showWaitingQueue(&serviceQueue);
break;

case 4:
showCompletedHistory(&serviceHistory);
break;

case 5:
locateCompletedRecord();
break;

case 6:
restoreLastService();
break;

case 7:
showSummary();
break;

case 8:
printf("\nExiting system...\n");
break;

default:
printf("\nInvalid menu choice.\n");
}

} while (choice != 8);

// Release all dynamically allocated memory
clearQueue(&serviceQueue);
clearHistory(&serviceHistory);
clearStack(&recentServices);

return 0;
}
