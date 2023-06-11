# include "minishell.h"

char **construct_envp(t_env *env)
{
    int count = 0;
    t_env *temp = env;
    while (temp) // Count the number of elements in the linked list
    {
        count++;
        temp = temp->next;
    }

    char **envp = malloc(sizeof(char*) * (count + 1));
    if (!envp) // Check if malloc failed
    {
        perror("malloc");
        exit(1);
    }

    temp = env;
    for (int i = 0; i < count; i++)
    {
        envp[i] = temp->get_joined_env(temp);
        temp = temp->next;
    }
    envp[count] = NULL; // The envp array must be null-terminated

    return envp;
}

// Function to handle commands and arguments
char **construct_cmd_args(t_token *token)
{
    int arg_count = 0;
    t_token *tmp = token->next;  // start from the next token, as current is a cmd

    while (tmp && tmp->type == arg)  // Only count tokens of type arg
    {
        arg_count++;
        tmp = tmp->next;
    }

    char **args = malloc(sizeof(char*) * (arg_count + 2));  // +2 for cmd itself and NULL at the end
    args[0] = token->str;  // The first argument is always the command itself
    for (int i = 0; i < arg_count; i++)  // Assign arguments
    {
        token = token->next;
        args[i+1] = token->str;
    }
    args[arg_count+1] = NULL;

    return args;
}




// Function to execute a single command with execve
void exec_single_command(t_token *token, t_env *env)
{
    char *cmd = token->str;
    token = token->next;
    char **args = construct_cmd_args(token);

    char **envp = construct_envp(env); // Assume you have a function to construct envp from env linked list

    execve(cmd, args, envp);

    perror("Command not found"); // If we reached here, execve failed
    exit(1);
}

// Function to create a pipe and fork a child process to execute a command
void fork_and_exec(t_token *token, t_env *env, int input_fd, int output_fd)
{
    int pid = fork();

    if (pid == 0)
    {
        if (input_fd != STDIN_FILENO) // If we have input redirection
        {
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
        }
        if (output_fd != STDOUT_FILENO) // If we have output redirection
        {
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
        }

        exec_single_command(token, env);
    }
}

void execute_command(t_data data)
{
    int input_fd = STDIN_FILENO;
    int output_fd = STDOUT_FILENO;
    int fd[2]; // For pipes

    t_token *token = data.token;
    while (token)
    {
        switch (token->type)
        {
            case simple_redir_left:
                input_fd = open(token->next->str, O_RDONLY);
                token = token->next->next;
                break;
            case simple_redir_right:
                output_fd = open(token->next->str, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                token = token->next->next;
                break;
            case double_redir_left:
                // Need additional implementation
                break;
            case double_redir_right:
                output_fd = open(token->next->str, O_WRONLY | O_CREAT | O_APPEND, 0644);
                token = token->next->next;
                break;
            case pipeline:
                pipe(fd);
                fork_and_exec(token, data.env, input_fd, fd[1]);
                if (input_fd != STDIN_FILENO) {
                    close(input_fd);
                }
                close(fd[1]); // We don't need the write end of the pipe
                input_fd = fd[0]; // The next command will read from here
                token = token->next;
                break;
            case cmd:
                // If the next token is a pipeline, we are not in the last command yet
                if (token->next && token->next->type == pipeline)
                {
                    fork_and_exec(token, data.env, input_fd, output_fd);
                }
                else
                {
                    // This is the last command, it should write to the original output
                    fork_and_exec(token, data.env, input_fd, STDOUT_FILENO);
                }
                token = NULL; // Finish execution
                break;
            default:
                token = token->next;
                break;
        }
    }

    // Close any remaining pipes
    if (input_fd != STDIN_FILENO)
    {
        close(input_fd);
    }
    if (output_fd != STDOUT_FILENO)
    {
        close(output_fd);
    }

    // Wait for all children to finish
    while (wait(NULL) > 0);
}



char *getAbsoluteCommandPath(const char *command) {
    // Replace these paths with the actual absolute paths for your system
    if (strcmp(command, "cat") == 0)
        return "/bin/cat";
    else if (strcmp(command, "grep") == 0)
        return "/usr/bin/grep";
    else if (strcmp(command, "ls") == 0)
        return "/bin/ls";
    else if (strcmp(command, "wc") == 0)
        return "/usr/bin/wc";
    else
        return NULL; // Command not found
}

void super_executer(t_data data)
{
    // Create the s_token for the command "ls -a"
    t_token *token1 = (t_token *)malloc(sizeof(t_token));
    token1->str = getAbsoluteCommandPath("cat");
    token1->type = cmd;
    token1->prev = NULL;
    token1->next = NULL;

    t_token *token2 = (t_token *)malloc(sizeof(t_token));
    token2->str = "<";
    token2->type = simple_redir_left;
    token2->prev = token1;
    token2->next = NULL;
    token1->next = token2;

    t_token *token3 = (t_token *)malloc(sizeof(t_token));
    token3->str = "Makefile";
    token3->type = arg;
    token3->prev = token2;
    token3->next = NULL;
    token2->next = token3;


    data.token = token1;

    char sign[MAX_CMD_LEN];
    if (fgets(sign, MAX_CMD_LEN, stdin) == NULL) {
        // L'utilisateur a tapé CTRL+D
        printf("Exit <3\n");
        exit(0);
    }
    sign[strcspn(sign, "\n")] = 0;  // Enlever le retour à la ligne

            t_token *curr = data.token;
        int i = 0;

        while(curr)
        {
            printf("cmd id -> [%d] | value -> [%s] | type -> [%d]\n", i, curr->str, curr->type);
            i++;
            curr = curr->next;
        }    
    execute_command(data);
}