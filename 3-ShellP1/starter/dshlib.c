#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
 int build_cmd_list(char *cmd_line, command_list_t *clist) {
    memset(clist, 0, sizeof(command_list_t)); 

    // printf("BUILD COMMAND LIST: RECEIVED: \"%s\"\n", cmd_line);

    char *save_ptr1, *save_ptr2;
    char *cmd_ptr = strtok_r(cmd_line, "|", &save_ptr1); // split by "|"
    int cmd_count = 0;

    while (cmd_ptr != NULL) {
        // trim
        while (isspace((unsigned char)*cmd_ptr)) cmd_ptr++;
        char *end = cmd_ptr + strlen(cmd_ptr) - 1;
        while (end > cmd_ptr && isspace((unsigned char)*end)) {
            *end = '\0';
            end--;
        }

        // printf("BUILD COMMAND LIST PARSED: %d: \"%s\"\n", cmd_count + 1, cmd_ptr);

        if (cmd_count >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }

        command_t *cmd = &clist->commands[cmd_count];

        // extract command name
        char *arg_ptr = strtok_r(cmd_ptr, " ", &save_ptr2);
        if (arg_ptr == NULL || strlen(arg_ptr) >= EXE_MAX) {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
        strncpy(cmd->exe, arg_ptr, EXE_MAX);
        cmd->exe[EXE_MAX - 1] = '\0';

        // extract arguments
        cmd->args[0] = '\0';
        int args_len = 0;

        while ((arg_ptr = strtok_r(NULL, " ", &save_ptr2)) != NULL) {
            if (args_len + strlen(arg_ptr) + 1 >= ARG_MAX) {
                return ERR_CMD_OR_ARGS_TOO_BIG;
            }
            if (args_len > 0) {
                strcat(cmd->args, " ");
                args_len++;
            }
            strcat(cmd->args, arg_ptr);
            args_len += strlen(arg_ptr);
        }

        cmd_count++;
        cmd_ptr = strtok_r(NULL, "|", &save_ptr1); // go to next command
    }

    clist->num = cmd_count;
    return OK;
}



