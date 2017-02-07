/*
  Author: Alan Zheng
  Date: February 6, 2017
*/

/*
  TODO:
    ->
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#define TRUE 1
#define FALSE 0
#define STDIN_READ_COUNT 1024
#define PARSED_INPUTS_INITIAL_SIZE 4
#define PIPED_INPUTS_INITIAL_SIZE 4
#define HISTORY_INITIAL_SIZE 4
#define LAST_X_HISTORY 10
#define CWD_SIZE 1024
#define EXIT_COMMAND "exit"
#define CD_COMMAND "cd"
#define PWD_COMMAND "pwd"
#define HISTORY_COMMAND "history"

typedef struct {
  char** array;
  size_t used;
  size_t size;
} StringArray;

void sighandler(int signum);
void initStringArray(StringArray *a, size_t initialSize);
void insertStringArray(StringArray *a, char* stringToInsert);
void trimStringArray(StringArray *a);
void freeStringArray(StringArray *a);
void readInput(FILE* stream, char* buffer);
void saveHistory(StringArray* history, char* input);
void stripLinefeed(char* str);
char* trim(char* str);
void parseInput(char* input, StringArray* parsedInputs, char* delimiter);
void checkExitStatus(char* input);
void assignArgs(char*** argsPtr, StringArray* parsedInputs);
int changeDir(char* command, char** args);
int showHistory(char* command, char** args, StringArray* history);
int displayCurrentWorkingDir(char* command, char* cwd);
void invokeProgram(char** args);
void getPipedInputs(char* input, StringArray* pipedInputs);

// helpers for debugging
void debug(char** args);

int main() {
  signal(SIGINT, sighandler);

  pid_t pid;
  FILE* stream;
  char* input; 
  StringArray* parsedInputs;
  StringArray* pipedInputs;
  StringArray* history;
  char* command;
  char** args;
  char cwd[CWD_SIZE];

  input = malloc(STDIN_READ_COUNT);
  parsedInputs = malloc(sizeof(StringArray));
  pipedInputs = malloc(sizeof(StringArray));
  history = malloc(sizeof(StringArray));
  initStringArray(history, HISTORY_INITIAL_SIZE);

  while (TRUE) {
    initStringArray(parsedInputs, PARSED_INPUTS_INITIAL_SIZE);
    initStringArray(pipedInputs, PIPED_INPUTS_INITIAL_SIZE);
    getcwd(cwd, sizeof(cwd));
    printf("%s>> ", cwd);
    readInput(stdin, input);
    stripLinefeed(input);
    saveHistory(history, input);
    getPipedInputs(input, pipedInputs);
    parseInput(input, parsedInputs, " ");
    command = (parsedInputs->array)[0];
    checkExitStatus(command);
    assignArgs(&args, parsedInputs);
    if (changeDir(command, args) || 
          showHistory(command, args, history) ||
          displayCurrentWorkingDir(command, cwd)) {
      freeStringArray(parsedInputs);
      freeStringArray(pipedInputs);
      continue;
    }
    if (pipedInputs->used > 1) {
      // handle inputs with pipes

      freeStringArray(parsedInputs);
      freeStringArray(pipedInputs);
    } else {
      // handle inputs with no pipes
      pid = fork();
      if (pid == 0) {
        //printf("[child process] running command...\n"); 
        invokeProgram(parsedInputs->array);   
        _exit(EXIT_SUCCESS);
      } else {
        wait(NULL);
        freeStringArray(parsedInputs); 
        freeStringArray(pipedInputs);
        //printf("[parent process] child completed!\n");
      }
    }
  }

  freeStringArray(history);
  free(history);
  free(parsedInputs);
  free(pipedInputs);
  free(input);

  return 0;
}


void sighandler(int signum) {}

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
  a->array[a->used] = malloc((strlen(stringToInsert) + 1) * sizeof(char));
  strcpy(a->array[a->used++], stringToInsert);
  a->array[a->used] = NULL;
}

void trimStringArray(StringArray *a) {
  char** ptr = a->array;
  while (ptr && *ptr) {
    strcpy(*ptr, trim(*ptr));
    ptr++;
  }
}

void freeStringArray(StringArray *a) {
  if (!a) { return; }
  char** ptr = a->array;
  size_t count = 0;
  size_t len = a->used;
  while (count < len) {  
    free(*ptr);
    *ptr = NULL;
    ptr++;
    count++;
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
  buffer[strlen(buffer)] = '\0';
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

// see: http://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way
char* trim(char* str) {
  size_t len = 0;
  char *frontPtr = str;
  char *endPtr = NULL;

  if (str == NULL) { return NULL; }
  if (*str == '\0') { return str; }

  len = strlen(str);
  endPtr = str + len;

  while (isspace(*frontPtr)) { frontPtr++; }
  if (endPtr != frontPtr) {
    while ( (isspace(*endPtr)) && (endPtr != frontPtr) ) { endPtr--; }
    // endPtr points to last character - index: n-1
  }

  // there is whitespace at the end so need to change location of \0
  if (str + len - 1 != endPtr) {
    *(endPtr + 1) = '\0';
  }

  // reusing pointer to point at front of the string buffer
  endPtr = str;
  if (frontPtr != str) {
    while (*frontPtr) {
      *endPtr = *frontPtr;
      endPtr++;
      frontPtr++;
    }
    *endPtr = '\0';
  }
  return str;
}

void parseInput(char* input, StringArray* parsedInputs, char* delimiter) {
  char** ptr = parsedInputs->array;
  char* token;
  char* str = input;

  do {
    token = strtok_r(str, delimiter, &str);
    if (token == NULL) break; 
    //printf("token (%s) length: %ld\n", token, strlen(token));
    insertStringArray(parsedInputs, token);
    ptr++;  
  } while (token != NULL);

  trimStringArray(parsedInputs);
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
int showHistory(char* command, char** args, StringArray* history) {
  char **ptr;
  int index;
  int count = 0;
  if (command && (strcmp(command, HISTORY_COMMAND) == 0)) {
    ptr = history->array;
    if (history->used >= LAST_X_HISTORY) {
      index = history->used - LAST_X_HISTORY;
      ptr += index;
      count += index;
    }
    while (ptr && *ptr) {
      printf("  %d  %s\n", ++count, *(ptr++));  
    }
    return TRUE;
  }
  return FALSE;
}

// returns 1 if command is to show current working dir, 0 otherwise
int displayCurrentWorkingDir(char* command, char* cwd) {
  if (command && (strcmp(command, PWD_COMMAND) == 0)) {
    printf("%s\n", cwd);
    return TRUE;
  }
  return FALSE;
}

void invokeProgram(char** args) {
  if (args == NULL) return; 
  execvp(args[0], args);
  fprintf(stderr, "%s: command not found\n", args[0]);
}

void getPipedInputs(char* input, StringArray* pipedInputs) {
  parseInput(input, pipedInputs, "|");
  //debug(pipedInputs->array);
}

void debug(char** args) {
  char** tmp = args;
  printf("args: \n");
  if (tmp != NULL) {
    while (*tmp != NULL) {
      printf("   - %s\n", *tmp);
      tmp++;
    }
  }
}
