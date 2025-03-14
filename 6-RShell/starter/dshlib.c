#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"

#include <errno.h>

/**** 
 **** FOR REMOTE SHELL USE YOUR SOLUTION FROM SHELL PART 3 HERE
 **** THE MAIN FUNCTION CALLS THIS ONE AS ITS ENTRY POINT TO
 **** EXECUTE THE SHELL LOCALLY
 ****
 */

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

 int exec_local_cmd_loop() {
     char *cmd_buff = malloc(SH_CMD_MAX);
     int rc = 0;
     command_list_t cmd_list;
 
 
     while (1) {
         // Display shell prompt
         printf("%s", SH_PROMPT);
 
         // Read user input
         if (fgets(cmd_buff, SH_CMD_MAX, stdin) == NULL) {
             printf("\n");
             break;
         }
 
         // Remove trailing newline
         cmd_buff[strcspn(cmd_buff, "\n")] = '\0';
 
         // Check if input is empty
         if (strlen(cmd_buff) == 0) {
             printf(CMD_WARN_NO_CMD);
             continue;
         }
 
         // Exit condition
         if (strcmp(cmd_buff, EXIT_CMD) == 0) {
             break;
         }
 
         // Parse input into command_list_t
         rc = build_cmd_list(cmd_buff, &cmd_list);
         if (rc == WARN_NO_CMDS) {
             printf(CMD_WARN_NO_CMD);
             continue;
         } else if (rc == ERR_TOO_MANY_COMMANDS) {
             printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
             continue;
         }
 
         // Check if it's a built-in command (only works if one command was entered)
         if (cmd_list.num == 1) {
             Built_In_Cmds cmd_type = exec_built_in_cmd(&cmd_list.commands[0]);
             if (cmd_type == BI_EXECUTED) {
                 free_cmd_list(&cmd_list);
                 continue;
             }
         }
 
         // Execute pipeline if multiple commands are piped together
         int exec_status;
         if (cmd_list.num > 1) {
             exec_status = execute_pipeline(&cmd_list);
         } else {
             exec_status = exec_cmd(&cmd_list.commands[0]);
         }
 
         // Handle execution errors
         if (exec_status == ERR_EXEC_CMD) {
             printf(CMD_ERR_EXECUTE);
         }
 
         // Free memory
         free_cmd_list(&cmd_list);
     }
 
     return OK;
 }
 
 
 int build_cmd_list(char *cmd_line, command_list_t *clist) {
     memset(clist, 0, sizeof(command_list_t));
 
     char *save_ptr1;
     char *cmd_ptr = strtok_r(cmd_line, "|", &save_ptr1);
     int cmd_count = 0;
 
     while (cmd_ptr != NULL) {
         if (cmd_count >= CMD_MAX) {
             return ERR_TOO_MANY_COMMANDS;
         }
 
         cmd_buff_t *cmd = &clist->commands[cmd_count];
 
         // Trim leading spaces
         while (*cmd_ptr == ' ') cmd_ptr++;
 
         // Allocate buffer for command storage
         cmd->_cmd_buffer = strdup(cmd_ptr);
         if (!cmd->_cmd_buffer) {
             return ERR_MEMORY;
         }
 
         // Manually tokenize arguments, preserving spaces inside quotes but removing quotes
         cmd->argc = 0;
         char *arg_ptr = cmd->_cmd_buffer;
         int in_quotes = 0;
         char quote_char = '\0';
         char *start = NULL;
         char clean_buffer[strlen(cmd->_cmd_buffer) + 1];  // Buffer to store cleaned argument
         char *write_ptr = clean_buffer; // Pointer to store valid argument characters
 
         while (*arg_ptr) {
             if ((*arg_ptr == '"' || *arg_ptr == '\'') && (!in_quotes || quote_char == *arg_ptr)) {
                 if (in_quotes && quote_char == *arg_ptr) {
                     in_quotes = 0;  // Closing quote
                 } else if (!in_quotes) {
                     in_quotes = 1;
                     quote_char = *arg_ptr;  // Store which quote was opened
                 }
             } else if (!in_quotes && isspace((unsigned char)*arg_ptr)) {
                 *arg_ptr = '\0';  // End of argument
                 if (start) {
                     *write_ptr = '\0';  // Ensure proper termination in the clean buffer
                     cmd->argv[cmd->argc++] = strdup(start);  // Store cleaned argument
                     start = NULL;
                     write_ptr = clean_buffer;  // Reset write pointer for next argument
                 }
             } else {
                 if (!start) start = write_ptr;  // Mark the beginning of an argument
                 *write_ptr++ = *arg_ptr;  // Copy character into clean buffer
             }
             arg_ptr++;
         }
 
         if (start) { // Add the last argument
             *write_ptr = '\0';  // Ensure null termination
             cmd->argv[cmd->argc++] = strdup(start);  // Store cleaned argument
         }
 
         cmd->argv[cmd->argc] = NULL; // NULL termination for execvp()
         
         cmd_count++;
         cmd_ptr = strtok_r(NULL, "|", &save_ptr1);
     }
 
     clist->num = cmd_count;
     return OK;
 }
 
 
 
 
 
 Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
     if (cmd->argc == 0 || cmd->argv[0] == NULL) {
         return BI_NOT_BI;
     }
 
     // Handle "exit" command
     if (strcmp(cmd->argv[0], "exit") == 0) {
         exit(0);
     }
 
     // Handle "cd" command
     if (strcmp(cmd->argv[0], "cd") == 0) {
         if (cmd->argc < 2 || cmd->argv[1] == NULL) {
             fprintf(stderr, "cd: missing argument\n");
             return BI_EXECUTED;
         }
 
         if (chdir(cmd->argv[1]) != 0) {
             perror("cd");
         }
 
         printf("Attempting to cd into: %s\n", cmd->argv[1]);
         return BI_EXECUTED;
     }
 
     return BI_NOT_BI;
 }
 
 
 int free_cmd_list(command_list_t *clist) {
     for (int i = 0; i < clist->num; i++) {
         free_cmd_buff(&clist->commands[i]); // Ensure each command is freed properly
     }
     clist->num = 0;
     return OK;
 }
 
 int free_cmd_buff(cmd_buff_t *cmd_buff) {
     if (cmd_buff->_cmd_buffer) {
         free(cmd_buff->_cmd_buffer); // Only free if allocated
         cmd_buff->_cmd_buffer = NULL;
     }
     cmd_buff->argc = 0;
     for (int i = 0; i < CMD_ARGV_MAX; i++) {
         cmd_buff->argv[i] = NULL;
     }
     return OK;
 }
 
 int exec_cmd(cmd_buff_t *cmd) {
     if (cmd->argc == 0 || cmd->argv[0] == NULL) {
         fprintf(stderr, "error: invalid command\n");
         return ERR_EXEC_CMD;
     }
 
     pid_t pid = fork();
     
     // debug
     if (pid == 0) {
 
     /*  //debug
         printf("Executing command: %s\n", cmd->argv[0]);
         for (int i = 0; cmd->argv[i] != NULL; i++) {
             printf(" Arg[%d]: %s\n", i, cmd->argv[i]);
         }
     */
 
         execvp(cmd->argv[0], cmd->argv);
         perror("execvp failed");
         exit(errno);
     } else if (pid > 0) {
         int status;
         waitpid(pid, &status, 0);
         if (WIFEXITED(status)) {
 
             return WEXITSTATUS(status);
         }
     } else {
         perror("fork failed");
         return ERR_EXEC_CMD;
     }
 
     return ERR_EXEC_CMD;
 }
 
 
 int execute_pipeline(command_list_t *clist) {
     int num_cmds = clist->num;
     int pipe_fds[CMD_MAX - 1][2];
     pid_t pids[CMD_MAX];
     int last_status = OK;
     int error_occurred = 0;  // Track if any command fails
 
     for (int i = 0; i < num_cmds; i++) {
         if (i < num_cmds - 1) {
             if (pipe(pipe_fds[i]) == -1) {
                 perror("pipe");
                 return ERR_EXEC_CMD;
             }
         }
 
         pids[i] = fork();
         if (pids[i] == -1) {
             perror("fork");
             return ERR_EXEC_CMD;
         }
 
         if (pids[i] == 0) {  // Child process
             if (i > 0) { dup2(pipe_fds[i - 1][0], STDIN_FILENO); }
             if (i < num_cmds - 1) { dup2(pipe_fds[i][1], STDOUT_FILENO); }
 
             for (int j = 0; j < num_cmds - 1; j++) {
                 close(pipe_fds[j][0]);
                 close(pipe_fds[j][1]);
             }
 
             execvp(clist->commands[i].argv[0], clist->commands[i].argv);
             perror("execvp");
             exit(ERR_EXEC_CMD);  // Exit with error code if execvp fails
         }
     }
 
     // Parent process: Close pipes
     for (int i = 0; i < num_cmds - 1; i++) {
         close(pipe_fds[i][0]);
         close(pipe_fds[i][1]);
     }
 
     // Wait for all child processes and check their exit statuses
     for (int i = 0; i < num_cmds; i++) {
         int status;
         waitpid(pids[i], &status, 0);
         if (WIFEXITED(status)) {
             int exit_status = WEXITSTATUS(status);
             if (exit_status == ERR_EXEC_CMD) {
                 error_occurred = 1;  // Mark that a command failed
             }
         }
     }
 
     return error_occurred ? ERR_EXEC_CMD : OK;
 }
 