#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TRUE 1
#define STDIN_READ_COUNT 1024
#define PARSED_INPUT_INITIAL_SIZE 4
#define CWD_SIZE 1024
#define EXIT_COMMAND "exit"

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
    //printf("token (%s) length: %ld\n", token, strlen(token));
    insertStringArray(parsedInputs, token);
    ptr++;  
  } while (token != NULL);
}

void checkExitStatus(char* input) {
  if (strcmp(input, EXIT_COMMAND) == 0) {
    exit(0);
  }
}

void assignArgs(char*** argsPtr, StringArray* parsedInputs) {
  if (parsedInputs->used > 1) {
    *argsPtr = (parsedInputs->array) + 1;
  } else {
    *argsPtr = NULL;
  } 
}

void invokeProgram(char** args) {
  if (args == NULL) return;
  char* command = args[0];
  printf("command is: %s\n", command);
  char** tmp = args;
  printf("args: \n");
  if (tmp != NULL) {
    while (*tmp != NULL) {
      printf("   - %s\n", *tmp);
      tmp++;
    }
  }
  execvp(args[0], args);
}

int main() {
  pid_t pid;
  FILE* stream;
  char* input; 
  StringArray* parsedInputs = malloc(sizeof(StringArray));
  char* command;
  char** args;
  char cwd[CWD_SIZE];

  while (TRUE) {
    input = malloc(STDIN_READ_COUNT);
    initStringArray(parsedInputs, PARSED_INPUT_INITIAL_SIZE);
    getcwd(cwd, sizeof(cwd));
    printf("%s>> ", cwd);
    readInput(stdin, input); 
    parseInput(input, parsedInputs);
    command = (parsedInputs->array)[0];
    checkExitStatus(command);
    assignArgs(&args, parsedInputs); 
    pid = fork();
    if (pid == 0) {
      //printf("[child process] running command...\n"); 
      invokeProgram(parsedInputs->array);   
      _exit(EXIT_SUCCESS);
    } else {
      wait(NULL);
      //printf("[parent process] child completed!\n");
    }
  }

  freeStringArray(parsedInputs);
  free(parsedInputs);
  free(input);

  return 0;
}
