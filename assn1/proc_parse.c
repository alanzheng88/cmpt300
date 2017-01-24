/*
  Author: Alan Zheng
  Date: January 22, 2017
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>

#define CPUINFO_FILE         "/proc/cpuinfo"
#define VERSION_FILE         "/proc/version"
#define MEMINFO_FILE         "/proc/meminfo"
#define UPTIME_FILE          "/proc/uptime"
#define BUFFER_SIZE          1024
#define COLUMN_PRINT_PADDING -18


int openFile(char* filename, FILE** file) {
  if ( (*file = fopen(filename, "r")) == NULL ) {
    printf("\nCould not open file: %s\n%s\n", 
            filename, strerror(errno));
    return -1;
  }
  return 0;
}

void parse(FILE* file, char** result, char* search, char* delimiter) {
  char* buffer = (char *) malloc(BUFFER_SIZE);
  char* status = (char *) malloc(BUFFER_SIZE);
  char* currentToken = (char *) malloc(BUFFER_SIZE);
 
  do {
    strcpy(status, fgets(buffer, BUFFER_SIZE, file)); 
      
      // printf("DEBUG: %s\n", buffer);
    
    // copies over content for the first line of file
    if (!search) {
      strcpy(*result, buffer); 
      break;
    }

    // copies over characters in file until whitespace is encountered
    if (search && (*search == '\0')) {
      int index = 0;
      while (!isspace(buffer[index])) { index++; }
      buffer[index] = '\0';
      strcpy(*result, buffer);
      break;
    }

    if (strstr(buffer, search)) {
      currentToken = strtok(buffer, delimiter);
      while (currentToken != NULL) {
        
          // printf("DEBUG: FOUND - %s\n", currentToken);

        currentToken = strtok(NULL, delimiter);
        if (currentToken != NULL) {
          strcpy(*result, currentToken);
        }
      }
      break;
    }

  } while (status != NULL);
 
  free(buffer); 
  free(status);
  free(currentToken);
}

char* trim(char* str) {
  // source: http://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way

  size_t len = 0;
  char *frontp = str;
  char *endp = NULL;

  if (str == NULL) { return NULL; }
  if (*str == '\0') { return str; }

  len = strlen(str);
  endp = str + len;

  while (isspace(*frontp)) { frontp++; }
  if (endp != frontp) {
    while ( (isspace(*endp)) && (endp != frontp) ) { endp--; }
    // endp is not pointing to the last character
  }

  // there is whitespace at the end so need to change location of \0
  if (str + len - 1 != endp) {
    *(endp + 1) = '\0';
  }

  // reusing pointer to point at front of the string buffer
  endp = str;
  if (frontp != str) {
    while (*frontp) {
      *endp = *frontp;
      endp++;
      frontp++;
    }
    *endp = '\0';
  }
  return str;
}

char* getSystemInfo(char* filename, char* filter, char* delimiter) {
  FILE* file;
  int statusCode = openFile(filename, &file);
  if (statusCode == -1) { return NULL; }
  char* result = (char *) malloc(BUFFER_SIZE);
  parse(file, &result, filter, delimiter);
  return trim(result);
}

void printProcessorType() {
  char* cpuInfo = getSystemInfo(CPUINFO_FILE, "model name", ":");
  if (!cpuInfo) { printf("\ninvalid cpu info file provided"); return; }
  printf("%*s: %s", COLUMN_PRINT_PADDING, "CPU Model Name",  cpuInfo);
  free(cpuInfo);
}

void printKernelVersion() {
  char* versionInfo = getSystemInfo(VERSION_FILE, NULL, NULL);
  if (!versionInfo) { printf("\ninvalid version file provided"); return; }
  printf("%*s: %s", COLUMN_PRINT_PADDING, "Kernel Version", versionInfo);
  free(versionInfo);
}

void printTotalMemoryAmount() {
  char* memoryInfo = getSystemInfo(MEMINFO_FILE, "MemTotal", ":");
  if (!memoryInfo) { printf("\ninvalid meminfo file provided"); return; }
  printf("%*s: %s", COLUMN_PRINT_PADDING, "Main Memory Total", memoryInfo);
  free(memoryInfo);
}

void printTimeSinceSystemLastBooted() {
  char* uptimeInfo = getSystemInfo(UPTIME_FILE, "", NULL);
  if (!uptimeInfo) { printf("\ninvalid uptime file provided"); return; }
  printf("%*s: %s Seconds", COLUMN_PRINT_PADDING, "Uptime", uptimeInfo);
  free(uptimeInfo);
}

int main() {
  printf("\n");
  printProcessorType();
  printKernelVersion();
  printTotalMemoryAmount();
  printTimeSinceSystemLastBooted();
  printf("\n\n");

  return 0;
}
