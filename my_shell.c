#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define STDIN_READ_COUNT 1024

void readInput(FILE* stream, char* buffer) {

  if (stream == stdin) {
    fgets(buffer, STDIN_READ_COUNT, stream);
  }
}

void parseInput(char* input, char** parsedInputs) {

/*
  char saveptr[STDIN_READ_COUNT];
  char* delimiter = " ";

  strtok_r(input, delimiter, &saveptr);
  */
}

int main() {

  pid_t pid;
  FILE* stream;
  char* input;
  char** parsedInputs;

  input = malloc(STDIN_READ_COUNT);

  printf(">> ");
  readInput(stdin, input);
  printf("input is: %s\n", input);

  free(input);

  return 0;
}
