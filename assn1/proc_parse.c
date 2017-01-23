/*
  Author: Alan Zheng
  Date: January 22, 2017
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define CPUINFO_FILE "/proc/cpuinfo"
#define VERSION_FILE "/proc/version"
#define MEMINFO_FILE "/proc/meminfo"
#define UPTIME_FILE  "/proc/uptime"
#define BUFFER_SIZE  1024

char* getProcessorType() {

}

int openFile(char* filename, FILE** file) {
  if ( (*file = fopen(filename, "r")) == NULL ) {
    printf("Could not open file: %s\n%s\n", 
            filename, strerror(errno));
    return -1;
  }
  printf("file successfully opened: %s\n", filename);
  return 0;
}

void parse(FILE* file, char* result, char* search) {
  char* buffer = (char *) malloc(BUFFER_SIZE);
  char* status = (char *) malloc(sizeof(NULL));
  char* currentToken = (char *) malloc(BUFFER_SIZE);
  
  do {
    status = fgets(buffer, BUFFER_SIZE, file); 
      
      // printf("DEBUG: %s\n", buffer);
    
    // !*search equivalent to (search[0] != '\0')
    if (search && !*search) {
      // search is empty
      continue;
    }

    if (strstr(buffer, search)) {
      currentToken = strtok(buffer, ":");
      while (currentToken != NULL) {
        
          // printf("DEBUG: FOUND - %s\n", currentToken);

        currentToken = strtok(NULL, ":");
        if (currentToken != NULL) {
          strcpy(result, currentToken);
        }
      }
      status = NULL; 
    }

  } while (status != NULL);
  
  free(buffer);
  free(status);
  free(currentToken);
}

int main() {

  FILE* file;
  int statusCode;

  statusCode = openFile(CPUINFO_FILE, &file);
  if (statusCode == -1) { return -1; }


  char* result = (char *) malloc(BUFFER_SIZE);
  parse(file, result, "model name");
  printf("result is: %s\n", result);
  free(result);

  return 0;
}
