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
  a->array[a->used] = malloc(strlen(stringToInsert) * sizeof(char*));
  strcpy(a->array[a->used++], stringToInsert);
}

void freeStringArray(StringArray *a) {
  char** head = a->array;
  while ((head != NULL) && (*head != NULL)) { 
    free(*head);
    *head = NULL;
    head++;
  }
  free(a->array);
  a->array = NULL;
  a->used = 0;
  a->size = 0;
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

void parseInput(char* input, StringArray* parsedInputs) {
  char** ptr = parsedInputs->array;
  char* token;
  char* str = input;
  char* delimiter = " "; 

  stripLinefeed(input);
  do {
    token = strtok_r(str, delimiter, &str);
    if (token == NULL) break;
    printf("token (%s) length: %ld\n", token, strlen(token));
    insertStringArray(parsedInputs, token);
    ptr++;  
  } while (token != NULL);
 
}

int main() {
  pid_t pid;
  FILE* stream;
  char* input; 
  StringArray* parsedInputs = malloc(sizeof(StringArray));

  input = malloc(STDIN_READ_COUNT);
  initStringArray(parsedInputs, PARSED_INPUT_INITIAL_SIZE);
  printf(">> ");
  readInput(stdin, input);
  printf("input is: %s\n", input);
  parseInput(input, parsedInputs);

  freeStringArray(parsedInputs);
  free(parsedInputs);
  free(input);

  return 0;
}
