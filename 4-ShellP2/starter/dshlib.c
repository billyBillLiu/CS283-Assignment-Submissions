#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */
 int exec_local_cmd_loop()
 {
     char *cmd_buff = malloc(SH_CMD_MAX);
     int rc = 0;
     cmd_buff_t cmd = {0};
 
     while (1)
     {
         printf("%s", SH_PROMPT);
         if (fgets(cmd_buff, SH_CMD_MAX, stdin) == NULL)
         {
             printf("\n");
             break;
         }
 
         // Trim leading and trailing spaces
         char *start = cmd_buff;
         while (isspace((unsigned char)*start))
             start++;
 
         char *end = start + strlen(start) - 1;
         while (end > start && isspace((unsigned char)*end))
             *end-- = '\0';
 
         if (*start == '\0')
         {
             printf(CMD_WARN_NO_CMD);
             continue;
         }
 
         // Handle quoted arguments and space elimination
         cmd.argc = 0;
         cmd._cmd_buffer = start;
         bool in_quotes = false;
         char *token = strtok(start, " ");
         
         while (token && cmd.argc < CMD_ARGV_MAX - 1)
         {
             if (token[0] == '"')
             {
                 in_quotes = true;
                 cmd.argv[cmd.argc++] = token + 1;
             }
             else if (in_quotes)
             {
                 strcat(cmd.argv[cmd.argc - 1], " ");
                 strcat(cmd.argv[cmd.argc - 1], token);
                 if (token[strlen(token) - 1] == '"')
                 {
                     in_quotes = false;
                     token[strlen(token) - 1] = '\0';
                 }
             }
             else
             {
                 cmd.argv[cmd.argc++] = token;
             }
             token = strtok(NULL, " ");
         }
         cmd.argv[cmd.argc] = NULL;
 
         if (cmd.argc == 0)
         {
             printf(CMD_WARN_NO_CMD);
             continue;
         }
 
         // Built-in command handling
         if (strcmp(cmd.argv[0], EXIT_CMD) == 0)
         {
             printf("Exiting shell\n");
             break;
         }
         else if (strcmp(cmd.argv[0], "dragon") == 0)
         {
             print_dragon();
             continue;
         }
         else if (strcmp(cmd.argv[0], "cd") == 0)
         {
             if (cmd.argc == 1)
             {
                 // Do nothing when no argument is provided
             }
             else if (cmd.argc > 1)
             {
                 if (chdir(cmd.argv[1]) != 0)
                 {
                     perror("cd failed");
                 }
             }
             continue;
         }
 
         // Execute external command
         pid_t pid = fork();
         if (pid == 0)
         {
             execvp(cmd.argv[0], cmd.argv);
             perror(CMD_ERR_EXECUTE);
             exit(ERR_EXEC_CMD);
         }
         else if (pid > 0)
         {
             int status;
             waitpid(pid, &status, 0);
             if (WIFEXITED(status))
             {
                 int exit_code = WEXITSTATUS(status);
                 // printf("Process exited with status %d\n", exit_code);
             }
         }
         else
         {
             perror("fork failed");
         }
     }
 
     free(cmd_buff);
     return OK;
 }
