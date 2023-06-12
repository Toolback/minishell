#include "minishell.h"

#define MAX_ARGS 50
void execute_command(t_data data)
{
    int fds[2];
    int in = 0;
    int out;
    t_token *t = data.token;
    pid_t pid;
    int child_processes = 0;

    while(t != NULL)
    {
        out = 1;  // Reset 'out' for each command

        if(t->type == cmd) 
        {
            char *args[MAX_ARGS] = {t->str};
            int i = 1;
            while(t->next && t->next->type != pipeline && t->next->type != simple_redir_right && 
                  t->next->type != simple_redir_left && t->next->type != double_redir_right && 
                  t->next->type != double_redir_left && i < MAX_ARGS - 1)
            {
                t = t->next;
                args[i++] = t->str;
            }
            args[i] = NULL;

            // Handle redirections after command args
            if(t->next && (t->next->type == simple_redir_right || t->next->type == double_redir_right ||
                           t->next->type == simple_redir_left))
            {
                t = t->next;
                if(t->type == simple_redir_right)
                {
                    t = t->next;
                    out = open(t->str, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
                }
                else if(t->type == double_redir_right)
                {
                    t = t->next;
                    out = open(t->str, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
                }
                else if(t->type == simple_redir_left)
                {
                    t = t->next;
                    in = open(t->str, O_RDONLY);
                }
            }

            // Setup pipeline
            pipe(fds);

            if((pid = fork()) == -1) 
            {
                perror("fork");
                exit(1);
            }

            if(pid == 0)
            {
                // Redirect stdin if necessary
                if(in != 0)
                {
                    dup2(in, 0);
                    close(in);
                }

                // Redirect stdout if necessary
                if(out != 1)
                {
                    dup2(out, 1);
                    close(out);
                }

                // Redirect stdout to pipe if it's not the last command
                if(t->next && t->next->type == pipeline)
                {
                    dup2(fds[1], 1);
                }
                close(fds[0]);
                close(fds[1]);

                char **envp = NULL;  // Prepare environment variables if required
                if(execve(args[0], args, envp) == -1)
                {
                    perror("execve");
                    printf("Command not found\n");
                    exit(EXIT_FAILURE);
                }
                exit(EXIT_SUCCESS);
            }
            else 
            {
                child_processes++; // Keep track of how many child processes we have
                close(fds[1]); // Close write end of the pipe immediately

                if(in != 0)
                {
                    close(in); // Close the input file descriptor from previous loop
                }

                in = fds[0]; // Keep track of the input for the next command
            }
        }
        t = t->next;
    }

    if(in != 0)
    {
        close(in);
    }

    // Wait for all child processes to finish
    for(int i = 0; i < child_processes; i++)
    {
        wait(NULL);
    }
}







char *getAbsoluteCommandPath(const char *command)
{
    // Replace these paths with the actual absolute paths for your system
    if (strcmp(command, "cat") == 0)
        return "/bin/cat";
    else if (strcmp(command, "grep") == 0)
        return "/usr/bin/grep";
    else if (strcmp(command, "yes") == 0)
        return "/usr/bin/yes";
    else if (strcmp(command, "ls") == 0)
        return "/bin/ls";
    else if (strcmp(command, "wc") == 0)
        return "/usr/bin/wc";
    else
        return NULL; // Command not found
}

void super_executer(t_data data)
{
// Manually create and link tokens
t_token *ls_cmd = malloc(sizeof(t_token));
ls_cmd->str = strdup("/bin/ls");   // Replace with actual path to ls
ls_cmd->type = cmd;
ls_cmd->prev = NULL;

t_token *ls_arg = malloc(sizeof(t_token));
ls_arg->str = strdup("-l");
ls_arg->type = arg;
ls_arg->prev = ls_cmd;

ls_cmd->next = ls_arg;

t_token *pipe1 = malloc(sizeof(t_token));
pipe1->str = strdup("|");
pipe1->type = pipeline;
pipe1->prev = ls_arg;

ls_arg->next = pipe1;

t_token *grep_cmd = malloc(sizeof(t_token));
grep_cmd->str = strdup("/usr/bin/grep");   // Replace with actual path to grep
grep_cmd->type = cmd;
grep_cmd->prev = pipe1;

pipe1->next = grep_cmd;

t_token *grep_arg = malloc(sizeof(t_token));
grep_arg->str = strdup(".txt");
grep_arg->type = arg;
grep_arg->prev = grep_cmd;

grep_cmd->next = grep_arg;

t_token *pipe2 = malloc(sizeof(t_token));
pipe2->str = strdup("|");
pipe2->type = pipeline;
pipe2->prev = grep_arg;

grep_arg->next = pipe2;

t_token *wc_cmd = malloc(sizeof(t_token));
wc_cmd->str = strdup("/usr/bin/wc");   // Replace with actual path to wc
wc_cmd->type = cmd;
wc_cmd->prev = pipe2;

pipe2->next = wc_cmd;

t_token *wc_arg = malloc(sizeof(t_token));
wc_arg->str = strdup("-l");
wc_arg->type = arg;
wc_arg->prev = wc_cmd;

wc_cmd->next = wc_arg;

t_token *redir_right = malloc(sizeof(t_token));
redir_right->str = strdup("<");
redir_right->type = simple_redir_left;
redir_right->prev = wc_arg;

wc_arg->next = redir_right;

t_token *file_name = malloc(sizeof(t_token));
file_name->str = strdup("file_count.txt");
file_name->type = arg;
file_name->prev = redir_right;

redir_right->next = file_name;
file_name->next = NULL;   // This is the end of the linked list

data.token = ls_cmd;   // Starting point of the linked list



    t_token *curr = data.token;
    int i = 0;
    char sign[MAX_CMD_LEN];

    if (fgets(sign, MAX_CMD_LEN, stdin) == NULL)
    {
        // L'utilisateur a tapé CTRL+D
        printf("Exit <3\n");
        exit(0);
    }
    sign[strcspn(sign, "\n")] = 0; // Enlever le retour à la ligne

    while (curr)
    {
        printf("cmd id -> [%d] | value -> [%s] | type -> [%d]\n", i, curr->str, curr->type);
        i++;
        curr = curr->next;
    }
    execute_command(data);
}