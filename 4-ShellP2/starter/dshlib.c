#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"
#include "dragon.h"
#include <errno.h>

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
 static int last_exit_code = 0;

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
 
         // trim whitespace
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
 
         // handle quotes
         char *p = start;
         bool in_quotes = false;
         char *arg_start = NULL;
         
         while (*p)
         {
             if (*p == '"')
             {
                 in_quotes = !in_quotes;
                 if (in_quotes)
                 {
                     arg_start = p + 1; // start of quote
                 }
                 else
                 {
                     *p = '\0'; // end of quoted arg
                     cmd.argv[cmd.argc++] = arg_start;
                     arg_start = NULL;
                 }
             }
             else if (!in_quotes && isspace((unsigned char)*p))
             {
                 *p = '\0'; // null terminate arg
                 if (arg_start)
                 {
                     cmd.argv[cmd.argc++] = arg_start;
                     arg_start = NULL;
                 }
             }
             else if (!arg_start)
             {
                 arg_start = p; // start of new arg
             }
             p++;
         }
         
         if (arg_start)
         {
             cmd.argv[cmd.argc++] = arg_start;
         }
         cmd.argv[cmd.argc] = NULL;
 
         if (cmd.argc == 0)
         {
             printf(CMD_WARN_NO_CMD);
             continue;
         }
 
         // built in commands
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
                 // nothing
             }
             else if (cmd.argc > 1)
             {
                 if (chdir(cmd.argv[1]) != 0)
                 {
                     perror("cd failed");
                 }
             }
             continue;
         } else if (strcmp(cmd.argv[0], "rc") == 0)  // built-in "rc" command
         {
             printf("%d\n", last_exit_code);
             continue;
         }
 
         // external commands (fork and exec)
         pid_t pid = fork();
         if (pid == 0)
         {
             execvp(cmd.argv[0], cmd.argv);
             switch (errno)
            {
                case ENOENT:
                    fprintf(stderr, "Command not found in PATH\n");
                    exit(2);
                case EACCES:
                    fprintf(stderr, "Permission denied: %s\n", cmd.argv[0]);
                    exit(3);
                default:
                    perror(CMD_ERR_EXECUTE);
                    exit(errno);
            }
         }
         else if (pid > 0)
         {
             int status;
             waitpid(pid, &status, 0);
             if (WIFEXITED(status))
             {
                 last_exit_code = WEXITSTATUS(status);
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
