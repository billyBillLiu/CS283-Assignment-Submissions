#include <stdio.h>

// EXTRA CREDIT - print the drexel dragon from the readme.md
extern void print_dragon(){
  // TODO implement 
  FILE *file = fopen("dragon.txt", "r");
            char line[256];
            while (fgets(line, sizeof(line), file))
            {
                printf("%s", line);
            }
            printf("\n");
            fclose(file);
}
