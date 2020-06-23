#include "./btree.h"

/**
 * Read a new command (= line) from stdin and return it.
 */
char *getCommand() {
  size_t startLen = 100, len = startLen;

  char *line_curr = malloc(sizeof(char) * startLen);
  if (line_curr == NULL) {
    exit(1);
  }

  char *line_start = line_curr;
  int ch;

  while (1) {
    // Read character
    ch = fgetc(stdin);
    if (--len == 0) {
      // We have to realloc
      len = startLen;
      startLen *= 2; // Double the length every time
      char *new_line = realloc(line_start, startLen * sizeof(char));
      if (new_line == NULL) {
        exit(1);
      }

      line_curr = new_line + (line_curr - line_start);
      line_start = new_line;
    }

    // Save character on the pointer
    if ((*line_curr++ = ch) == '\n' || ch == EOF) {
      break;
    }
  }
  *line_curr = '\0';
  return line_start;
}

/**
 * The main function of the program, a loop that parses the commands given on stdin and performs the actions on the
 * BTree.
 * @param argc
 * @param argv
 * @return the exit code of the program
 */
int main(int argc, char *argv[]) {
  BTree *tree = createBTree();
  if (tree == NULL) {
    return 1;
  }

  char *line;

  char *date;
  char *description;
  char *sdate;

  while ((line = getCommand()) != NULL && strlen(line) > 0) {
    switch (*line) {
      case '+':
        if (strlen(line) < DATE + 1) {
          freeBTree(tree);
          free(line);
          printf("This is an invalid command. The program will now exit");
          return 1;
        }

        // We willen een date met description toevoegen
        date = malloc(sizeof(char) * DATE);
        if (date == NULL) {
          freeBTree(tree);
          free(line);
          return 1;
        }

        memcpy(date, &line[1], DATE - 1);
        date[DATE - 1] = '\0';

        description = malloc(sizeof(char) * (strlen(line) - DATE - 1));
        if (description == NULL) {
          freeBTree(tree);
          free(line);
          return 1;
        }
        memcpy(description, &line[1 + DATE], strlen(line) - DATE - 2);
        description[strlen(line) - DATE - 2] = '\0';

        insertEntry(tree, date, description);

        free(line);
        line = NULL;
        date = NULL;
        description = NULL;

        break;
      case '-':
        if (strlen(line) < DATE) {
          freeBTree(tree);
          free(line);
          printf("This is an invalid command. The program will now exit");
          return 1;
        }

        // We willen een date verwijderen
        date = malloc(sizeof(char) * DATE);
        if (date == NULL) {
          freeBTree(tree);
          free(line);
          return 1;
        }
        memcpy(date, &line[1], DATE - 1);
        date[DATE - 1] = '\0';

        int removeStatus = removeEntry(tree, date);

        if (removeStatus) {
          printf("-\n");
        } else {
          printf("?\n");
        }

        free(line);
        line = NULL;
        free(date);
        date = NULL;

        break;
      case '?':
        if (strlen(line) < DATE) {
          freeBTree(tree);
          free(line);
          printf("This is an invalid command. The program will now exit");
          return 1;
        }

        // We willen kijken of een bepaalde date in de boom zit
        date = malloc(sizeof(char) * DATE);
        if (date == NULL) {
          freeBTree(tree);
          free(line);
          return 1;
        }
        memcpy(date, &line[1], DATE - 1);
        date[DATE - 1] = '\0';

        char *result = getEntry(tree, date);

        if (result != NULL) {
          printf("!%s\n", result);
        } else {
          printf("?\n");
        }

        free(line);
        line = NULL;
        free(date);
        date = NULL;

        break;
      case '#':
        if (strlen(line) < DATE + DATE + 1) {
          freeBTree(tree);
          free(line);
          printf("This is an invalid command. The program will now exit");
          return 1;
        }
        date = malloc(sizeof(char) * DATE);
        if (date == NULL) {
          freeBTree(tree);
          free(line);
          return 1;
        }
        memcpy(date, &line[1], DATE - 1);
        date[DATE - 1] = '\0';

        sdate = malloc(sizeof(char) * DATE);
        if (sdate == NULL) {
          freeBTree(tree);
          free(line);
          return 1;
        }
        memcpy(sdate, &line[1 + DATE], DATE - 1);
        sdate[DATE - 1] = '\0';

        int amount;
        if (strcmp(date, sdate) < 0) {
          amount = countBetweenEntries(tree, date, sdate);
        } else {
          amount = countBetweenEntries(tree, sdate, date);
        }
        printf("%i\n", amount);

        free(line);
        line = NULL;
        free(date);
        date = NULL;
        free(sdate);
        sdate = NULL;

        break;
      case EOF:
      case '\0':
      case '\n':
        // Exit the program
        free(line);
        line = NULL;
        freeBTree(tree);

        return 0;
      default:
        // Invalid input command, exit program with exit code != 0
        printf("This is an invalid command. The program will now exit");

        free(line);
        line = NULL;
        freeBTree(tree);

        return 1;
    }
  }
}