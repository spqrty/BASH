#include <getopt.h>
#include <regex.h>
#include <stdio.h>

#include "../common/flag.h"

#define BuffSize 10000
struct Data {
  char arguments[BuffSize];
  char outFlags[11];
  int filesCount;
  int eCount;
};

int inputFlags(int argc, char **argv, struct Data *data);
int initFile(char *fileName, char *argv[], struct Data *data);
void processEFlag(struct Data *data);
int processFFlag(struct Data *data);

void output(char *argv[], FILE *file, struct Data *data);

int main(int argc, char *argv[]) {
  int checkError = 1;
  struct Data data = {"\0", "\0", 0, 0};

  if (!inputFlags(argc, argv, &data)) checkError = 0;
  if (checkError) {
    data.filesCount = argc - optind;
    for (; optind < argc;) {
      if (!initFile(argv[optind], argv, &data)) {
        if (!isFlag('s', data.outFlags))
          printf("./s21_grep: %s: No such file or directory\n", argv[optind]);
      } else
        optind++;
    }
  }
  return 0;
}

int inputFlags(int argc, char **argv, struct Data *data) {
  const struct option longOptions[] = {
      {"e", no_argument, 0, 'e'}, {"i", no_argument, 0, 'i'},
      {"v", no_argument, 0, 'v'}, {"c", no_argument, 0, 'c'},
      {"l", no_argument, 0, 'l'}, {"n", no_argument, 0, 'n'},
      {"h", no_argument, 0, 'h'}, {"s", no_argument, 0, 's'},
      {"f", no_argument, 0, 'f'}, {"o", no_argument, 0, 'o'},
  };
  int result = 0, checkError = 1;
  while ((result = getopt_long(argc, argv, "e:ivclnhsf:o", longOptions, 0)) !=
             -1 &&
         checkError != 0) {
    if (result == '?' || !checkError) {
      checkError = 0;
    } else {
      append_flag(result, data->outFlags);
      if (isFlag('e', data->outFlags)) {
        data->eCount++;
        processEFlag(data);
      }
      if (isFlag('f', data->outFlags)) {
        if (!processFFlag(data)) {
          checkError = 0;
        }
      }
    }
  }
  if (!isFlag('e', data->outFlags) && !isFlag('f', data->outFlags))
    strcat(data->arguments, argv[optind++]);

  return checkError;
}

int initFile(char *fileName, char *argv[], struct Data *data) {
  int checkError = 1;

  FILE *file = fopen(fileName, "r");

  if (file != NULL) {
    output(argv, file, data);
    fclose(file);
  } else {
    checkError = 0;
  }

  return checkError;
}

void processEFlag(struct Data *data) {
  if (data->eCount > 1) strcat(data->arguments, "|");
  strcat(data->arguments, optarg);
}

int processFFlag(struct Data *data) {
  int isGood = 1;

  FILE *fFile = fopen(optarg, "r");
  char buffer[BuffSize] = "\0";

  if (fFile != NULL) {
    fseek(fFile, 0, SEEK_SET);
    while (fgets(buffer, BuffSize, fFile) != NULL) {
      if (buffer[strlen(buffer) - 1] == '\n') {
        buffer[strlen(buffer) - 1] = 0;
      }
      if (data->eCount > 0) {
        strcat(data->arguments, "|");
      }
      if (*buffer == '\0') {
        strcat(data->arguments, ".");
      } else {
        strcat(data->arguments, buffer);
      }
      data->eCount++;
    }
    fclose(fFile);
  } else {
    if (!isFlag('s', data->outFlags))
      printf("./s21_grep: %s: No such file or directory\n", optarg);
    isGood = 0;
  }

  return isGood;
}

void output(char *argv[], FILE *file, struct Data *data) {
  regex_t regex;
  int regFlags = REG_EXTENDED;

  char buffer[BuffSize];

  regmatch_t regMatch[1] = {0};
  int status;

  size_t fileNumLine = 1;
  int numLine = 0;

  if (isFlag('i', data->outFlags)) regFlags |= REG_ICASE;

  regcomp(&regex, data->arguments, regFlags);

  while (!feof(file)) {
    if (fgets(buffer, BuffSize, file)) {
      status = regexec(&regex, buffer, 1, regMatch, 0);
      if (isFlag('v', data->outFlags)) {
        status = status ? 0 : 1;
      }
      if (status != REG_NOMATCH) {
        if (!isFlag('c', data->outFlags) && !isFlag('l', data->outFlags)) {
          if (!isFlag('h', data->outFlags) && data->filesCount > 1)
            printf("%s:", argv[optind]);

          if (isFlag('n', data->outFlags)) printf("%lu:", fileNumLine);

          if (isFlag('o', data->outFlags)) {
            char *ptr = buffer;
            while (!status && regMatch[0].rm_eo != regMatch[0].rm_so) {
              printf("%.*s\n", (int)(regMatch[0].rm_eo - regMatch[0].rm_so),
                     ptr + regMatch[0].rm_so);
              ptr += regMatch[0].rm_eo;
              status = regexec(&regex, ptr, 1, regMatch, REG_NOTBOL);
            }
          } else {
            printf("%s", buffer);
          }
          if (buffer[strlen(buffer) - 1] != '\n') printf("\n");
        }
        numLine++;
      }
      fileNumLine++;
    }
  }

  if (isFlag('c', data->outFlags)) {
    if (!isFlag('h', data->outFlags) && data->filesCount > 1)
      printf("%s:", argv[optind]);
    if (isFlag('l', data->outFlags) && numLine)
      printf("1\n");
    else
      printf("%d\n", numLine);
  }

  if (isFlag('l', data->outFlags) && numLine) printf("%s\n", argv[optind]);

  regfree(&regex);
}
