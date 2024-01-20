#include "flag.h"

int isFlag(char flag, char *outFlags) {
  return strchr(outFlags, flag) != NULL ? 1 : 0;
}

void append_flag(char flag, char *outFlags) {
  if (!isFlag(flag, outFlags)) {
    char temp[2] = "-";
    temp[0] = flag;
    strcat(outFlags, temp);
  }
}