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


int openFile(char* filename, FILE** file) {
  if ( (*file = fopen(filename, "r")) == NULL ) {
    printf("Could not open file: %s\n%s\n", 
            filename, strerror(errno));
    return -1;
  }
  return 0;
}

void parse(FILE* file, char** result, char* search, char* delimiter) {
  char* buffer = (char *) malloc(BUFFER_SIZE);
  char* status = (char *) malloc(sizeof(NULL));
  char* currentToken = (char *) malloc(BUFFER_SIZE);
 
  do {
    status = fgets(buffer, BUFFER_SIZE, file); 
      
      // printf("DEBUG: %s\n", buffer);
    
    // !*search equivalent to (search[0] != '\0')
    if ( !search || (search && !*search) ) {
      // search is empty
      strcpy(*result, buffer);
      continue;
    }

    if (strstr(buffer, search)) {
      currentToken = strtok(buffer, delimiter);
      while (currentToken != NULL) {
        
          // printf("DEBUG: FOUND - %s\n", currentToken);

        currentToken = strtok(NULL, ":");
        if (currentToken != NULL) {
          strcpy(*result, currentToken);
        }
      }
      status = NULL; 
    }

  } while (status != NULL);
  
  free(buffer);
  free(status);
  free(currentToken);
}


char* getSystemInfo(char* filename, char* filter, char* delimiter) {
  FILE* file;
  int statusCode = openFile(filename, &file);
  if (statusCode == -1) { return NULL; }
  char* result = (char *) malloc(BUFFER_SIZE);
  parse(file, &result, filter, delimiter);
  //printf("result is: %s\n", result);
  return result;
}

void printProcessorType() {
  char* cpuInfo = getSystemInfo(CPUINFO_FILE, "model name", ":");
  if (!cpuInfo) { printf("invalid cpu info file provided"); }
  printf("CPU Model Name: %s", cpuInfo);
  free(cpuInfo);
}

void printKernelVersion() {
  char* versionInfo = getSystemInfo(VERSION_FILE, NULL, NULL);
  if (!versionInfo) { printf("invalid version file provided"); }
  printf("Kernel Version: %s", versionInfo);
  free(versionInfo);
}

void printTotalMemoryAmount() {
  char* memoryInfo = getSystemInfo(MEMINFO_FILE, "MemTotal", ":");
  if (!memoryInfo) { printf("invalid meminfo file provided"); }
  printf("Main Memory Total: %s", memoryInfo);
  free(memoryInfo);
}

void printTimeSinceSystemLastBooted() {
  char* uptimeInfo = getSystemInfo(UPTIME_FILE, " ", " ");
  if (!uptimeInfo) { printf("invalid uptime file provided"); }
  printf("Uptime: %s", uptimeInfo);
  free(uptimeInfo);
}

int main() {
  printProcessorType();
  printKernelVersion();
  printTotalMemoryAmount();
  printTimeSinceSystemLastBooted();

  return 0;
}
