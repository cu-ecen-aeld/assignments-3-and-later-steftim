#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

int main(int argc, char** argv) {

  openlog("writer", LOG_PERROR | LOG_CONS, LOG_USER);
  if (argc != 3) {
    syslog(LOG_ERR, "arguments no specified");
    syslog(LOG_USER, "%s usage: %s /path file_name", argv[0], argv[0]);
    exit(1);
  }

  char dirpath[strlen(argv[1])];
  strcpy(dirpath, argv[1]);
  char* ptr = strrchr(dirpath, '/');
  (ptr == NULL) ?: (*ptr = '\0');
  DIR* dir = opendir(dirpath);
  if (dir) {
    closedir(dir);
  } else {
    syslog(LOG_ERR, "directory not exists");
    exit(1);
  }

  FILE* file = fopen(argv[1], "w");
  if (file == NULL) {
    syslog(LOG_ERR, "Error when opening file");
    exit(1);
  }

  syslog(LOG_DEBUG, "Writing %s to %s", argv[2], argv[1]);

  int buf_size = strlen(argv[2]) + 1;
  int len = fprintf(file, "%s\n", argv[2]);
  if (len <= 0 || len > buf_size) {
    syslog(LOG_ERR, "Error when writing to file");
  }

  fclose(file);
  closelog();

  return 0;
}