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

int main() {
  printf("string: %s\n"  "length of string: %zd\n",
          CPUINFO_FILE,   strlen(CPUINFO_FILE));

  FILE* file;
  int statusCode;

  statusCode = openFile(CPUINFO_FILE, &file);
  if (statusCode == -1) { return -1; }



  char* buffer;
  char* success;
  char* pch;
  char* result = (char *) malloc(BUFFER_SIZE);

  do {
    success = fgets(buffer, BUFFER_SIZE, file);
    printf("%s\n", buffer);
    if (strstr(buffer, "model name")) {
      pch = strtok(buffer, ":");
      while (pch != NULL) {
        printf("FOUND: %s\n", pch);
        pch = strtok(NULL, ":");
        if (pch != NULL) {
          strcpy(result, pch);
        }
      }
      return 0;
    }
  } while (success != NULL);
 
  free(result);

  return 0;
}
