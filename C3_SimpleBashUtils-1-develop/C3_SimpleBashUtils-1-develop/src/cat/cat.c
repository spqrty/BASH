#include <getopt.h>
#include <stdio.h>

#include "../common/flag.h"

int input_flags(int argc, char **argv, char *Flags);
int input_files(char *FileName, char *Flags);
void output(int Symbol, char *Flags);

int main(int argc, char *argv[]) {
  char Flags[9] = "\0";
  int Error = 1;
  if (!input_flags(argc, argv, Flags)) {
    Error = 0;
  }

  if (Error) {
    for (int i = optind; i < argc; i++) {
      if (!input_files(argv[i], Flags)) {
        printf("./s21_cat: %s: No such file or directory\n", argv[i]);
        break;
      }
    }
  }
  return 0;
}

int input_flags(int argc, char **argv, char *Flags) {
  const char *short_options = "+benstvET";
  const struct option long_options[] = {
      {"b", 0, 0, 'b'}, {"number-nonblank", 0, 0, 'b'},
      {"e", 0, 0, 'e'}, {"E", 0, 0, 'E'},
      {"n", 0, 0, 'n'}, {"number", 0, 0, 'n'},
      {"s", 0, 0, 's'}, {"squeeze-blank", 0, 0, 's'},
      {"t", 0, 0, 't'}, {"T", 0, 0, 'T'},
      {"v", 0, 0, 'v'}};
  int rez, OptionIndex;
  int check = 1;
  while ((rez = getopt_long(argc, argv, short_options, long_options,
                            &OptionIndex)) != -1) {
    if (rez == '?') {
      check = 0;
      break;
    } else {
      append_flag(rez, Flags);
    }
  }
  return check;
}

int input_files(char *FileName, char *Flags) {
  int check = 1;
  FILE *File;
  if (!(File = fopen(FileName, "r"))) check = 0;
  if (check) {
    int Symbol = fgetc(File);
    while (Symbol != EOF) {
      output(Symbol, Flags);
      Symbol = fgetc(File);
    }
    fclose(File);
  }
  return check;
}

void output(int Symbol, char *Flags) {
  static int LineNumber = 0, IsCheckOut = 0, PostSymbol = '\n';
  if (!(isFlag('s', Flags) && PostSymbol == '\n' && Symbol == '\n' &&
        IsCheckOut)) {
    IsCheckOut = PostSymbol == '\n' && Symbol == '\n' ? 1 : 0;

    if (((isFlag('n', Flags) && !isFlag('b', Flags)) ||
         (isFlag('b', Flags) && Symbol != '\n')) &&
        PostSymbol == '\n') {
      LineNumber += 1;
      printf("%6d\t", LineNumber);
    }

    if ((isFlag('e', Flags) || isFlag('E', Flags)) && Symbol == '\n') {
      printf("$");
    }

    if ((isFlag('t', Flags) || isFlag('T', Flags)) && Symbol == '\t') {
      printf("^");
      Symbol = '\t' + 64;
    }
    if ((isFlag('v', Flags) || isFlag('t', Flags) || isFlag('e', Flags))) {
      if (Symbol < 32 && Symbol != 9 && Symbol != 10) {
        Symbol += 64;
        printf("^");
      }

      if (Symbol == 127) {
        Symbol = '?';
        printf("^");
      }
    }
    fputc(Symbol, stdout);
  }
  PostSymbol = Symbol;
}