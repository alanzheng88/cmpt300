/*
  Author: Alan Zheng
  Date: February 1, 2017
*/

/*
  [fix] enter internal command, enter alot of spaces -> program crashes
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define TRUE 1
#define FALSE 0
#define STDIN_READ_COUNT 1024
#define PARSED_INPUT_INITIAL_SIZE 4
#define HISTORY_INITIAL_SIZE 4
#define CWD_SIZE 1024
#define EXIT_COMMAND "exit"
#define CD_COMMAND "cd"
#define HISTORY_COMMAND "history"

typedef struct {
  char** array;
  size_t used;
  size_t size;
} StringArray;

void initStringArray(StringArray *a, size_t initialSize) {
  a->array = malloc(initialSize * sizeof(char*));
  if (a->array == NULL) {
    printf("Failed to allocate memory in initStringArray\n");
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

void readInput(FILE* stream, char* buffer) {
  if (stream == stdin) {
    fgets(buffer, STDIN_READ_COUNT, stream);
  }
}

void saveHistory(StringArray* history, char* input) {
  insertStringArray(history, input);
}

// see: http://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input
void stripLinefeed(char* str) {
  if (str) {
    str[strcspn(str, "\r\n")] = '\0';
  }
}

void parseInput(char* input, StringArray* parsedInputs) {
  char** ptr = parsedInputs->array;
  char* token;
  char* str = input;
  char* delimiter = " "; 

  do {
    token = strtok_r(str, delimiter, &str);
    if (token == NULL) break;
    //printf("token (%s) length: %ld\n", token, strlen(token));
    insertStringArray(parsedInputs, token);
    ptr++;  
  } while (token != NULL);
}

void checkExitStatus(char* input) {
  if (input && (strcmp(input, EXIT_COMMAND) == 0)) {
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

// returns 1 if command is to change directory, 0 otherwise
int changeDir(char* command, char** args) {
  int status;
  if (command && (strcmp(command, CD_COMMAND) == 0)) {
    if (!args) {
      return TRUE;
    }
    if (args && args[0]) {
      status = chdir(args[0]);
      if (status == 0) {
        return TRUE;
      } else {
        fprintf(stderr, "cd: %s: %s\n", args[0], strerror(errno));
      }
    } 
  }
  return FALSE;
}

// returns 1 if command is to show history, 0 otherwise
int showHistory(char* command, char**args, StringArray* history) {
  if (command && (strcmp(command, HISTORY_COMMAND) == 0)) {
    char** ptr = history->array;
    int count = 1;
    while (ptr && *ptr) {
      printf("  %d  %s\n", count++, *(ptr++));  
    }
    return 1;
  }
  return 0;
}

void invokeProgram(char** args) {
  if (args == NULL) return;
  /*
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
  */
  execvp(args[0], args);
  fprintf(stderr, "%s: command not found\n", args[0]);
}

int main() {
  pid_t pid;
  FILE* stream;
  char* input; 
  StringArray* parsedInputs;
  char* command;
  char** args;
  char cwd[CWD_SIZE];
  StringArray* history;

  parsedInputs = malloc(sizeof(StringArray));
  history = malloc(sizeof(StringArray));
  initStringArray(history, HISTORY_INITIAL_SIZE);

  while (TRUE) {
    initStringArray(parsedInputs, PARSED_INPUT_INITIAL_SIZE);
    input = malloc(STDIN_READ_COUNT);
    getcwd(cwd, sizeof(cwd));
    printf("%s>> ", cwd);
    readInput(stdin, input);
    stripLinefeed(input);
    saveHistory(history, input);
    parseInput(input, parsedInputs);
    command = (parsedInputs->array)[0];
    checkExitStatus(command);
    assignArgs(&args, parsedInputs); 
    if (changeDir(command, args) || 
        showHistory(command, args, history)) {
      freeStringArray(parsedInputs);
      continue; 
    }
    pid = fork();
    if (pid == 0) {
      //printf("[child process] running command...\n"); 
      invokeProgram(parsedInputs->array);   
      _exit(EXIT_SUCCESS);
    } else {
      wait(NULL);
      freeStringArray(parsedInputs); 
      //printf("[parent process] child completed!\n");
    }
  }

  freeStringArray(history);
  free(history);
  free(parsedInputs);
  free(input);

  return 0;
}
