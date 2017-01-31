#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define STDIN_READ_COUNT 1024
#define PARSED_INPUT_INITIAL_SIZE 4

typedef struct {
  char** array;
  size_t used;
  size_t size;
} StringArray;

void initStringArray(StringArray *a, size_t initialSize) {
  a->array = malloc(initialSize * sizeof(char*));
  if (a->array == NULL) {
    printf("Failed to allocate memory in initStringArray");
    exit(EXIT_FAILURE);
  }
  a->used = 0;
  a->size = initialSize;
}

void insertStringArray(StringArray *a, char* stringToInsert) {
  if (a->used == a->size) {
    a->size *= 2;
    a->array = realloc(a->array, a->size * sizeof(char*));
  }
  a->array[a->used++] = stringToInsert;
}

void freeStringArray(StringArray *a) {
  char** head = a->array;
  while (head != NULL) {
    free(*head);
    *head = NULL;
    head++;
  }
  free(a->array);
  a->array = NULL;
  a->used = 0;
  a->size = 0;
}

// dynamically allocate memory
void allocateParsedInputs(char*** parsedInputs, int size) {
  *parsedInputs = malloc(size * sizeof(char*));
  if (parsedInputs == NULL) {
    printf("Failed to allocate memory in allocateParseInputs()\n");
    exit(EXIT_FAILURE);
  }
}

// reallocating memory
void reallocateParsedInputs(char*** parsedInputs) {
  
}

// clean up pointers
void freeParsedInputs(char*** parsedInputs) {
  char** ptr = *parsedInputs;
  char* current = *ptr;
  while (current != NULL) {
    free(current);
    current = NULL;
  }
  free(ptr);
  ptr = NULL;
}

// helper functions
void readInput(FILE* stream, char* buffer) {

  if (stream == stdin) {
    fgets(buffer, STDIN_READ_COUNT, stream);
  }
}

// see: http://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input
void stripLinefeed(char* str) {
  str[strcspn(str, "\r\n")] = '\0';
}

void parseInput(char* input, char** parsedInputs) {

  char* token;
  char* str = input;
  char* delimiter = " "; 
  int counter = 0;

  stripLinefeed(input);
  do {
    token = strtok_r(str, delimiter, &str);
    if (token == NULL) break;
    printf("token (%s) length: %ld\n", token, strlen(token));
    *parsedInputs = malloc(strlen(token) + 1);
    strcpy(*parsedInputs, token);
    printf("after strcpy length: %ld\n", strlen(*parsedInputs));
    parsedInputs++;
    counter++;
  } while ((token != NULL) && (counter < PARSED_INPUT_INITIAL_SIZE));

}

int main() {

  pid_t pid;
  FILE* stream;
  char* input;
  char** parsedInputs;

  input = malloc(STDIN_READ_COUNT);
  allocateParsedInputs(&parsedInputs, PARSED_INPUT_INITIAL_SIZE);

  printf(">> ");
  readInput(stdin, input);
  printf("input is: %s\n", input);
  parseInput(input, parsedInputs);

  free(input);
  freeParsedInputs(&parsedInputs);

  return 0;
}
