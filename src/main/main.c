#include "../../includes/minishell.h"

// Fonction pour exécuter une commande
void execute_command(char *cmd, t_data *data) {
    char *args[MAX_NUM_ARGS];
    int i = 0;

    // Diviser la commande en arguments
    args[i] = strtok(cmd, " ");
    while (args[i] != NULL) {
        i++;
        args[i] = strtok(NULL, " ");
    }

    // Vérifier la commande
    if (strcmp(args[0], "exit") == 0) {
        exit(0);
    }
    if (strcmp(args[0], "env") == 0) {
        ft_env(data->env);
    }
    else {
        pid_t pid = fork();
        if (pid == 0) {
            // Nous sommes dans le processus fils
            execvp(args[0], args);
            // Si execvp retourne, c'est qu'une erreur s'est produite
            printf("minishell: %s: command not found\n", args[0]);
            exit(1);
        } else {
            // Nous sommes dans le processus parent
            wait(NULL);
        }
    }
}

// Gestionnaire de signaux
void handle_signal(int sig) {
    if (sig == SIGINT) {
        printf("\nminishell2 ▸ ");
        fflush(stdout);
    }
}
// flag 0 = quote close 
// flag 1 = double quote open
// flag 2 = single quote open
int is_quote_open(char *line, int max)
{
    int i;
    int flag;

    i = 0;
    flag = 0;
    while(line[i] && i < max)
	{
		if (i > 0 && line[i - 1] == '\\')
			;
		else if (flag == 0 && line[i] == '\"')
			flag = 1;
		else if (flag == 0 && line[i] == '\'')
			flag = 2;
		else if (flag == 1 && line[i] == '\"')
			flag = 0;
		else if (flag == 2 && line[i] == '\'')
			flag = 0;
		i++;
	}
    return (flag);
}

int sanitise_args(char *line)
{
    if (is_quote_open(line, MAX_INT) != 0)
        return (1);
    // space_line(line);
    return (0);
}

int main(int ac, char **av, char **env) {
    t_data data;
    // char cmd[MAX_CMD_LEN];
    char *line;

    (void)ac;
	(void)av;
    init_data(&data);
    if (init_env(&data, env, 1) != 0 || init_env(&data, env, 2) != 0)
    {
        // exit_shell(&data);
        return (1);
    }
	increment_shell_level(data.env);

    // testing purpose
    add_new_env(data.env, "test", "SHLVLMEEEE");
    //

    signal(SIGINT, handle_signal);
    while (data.exit == 0) {
        // printf("minishell ▸ ");
    	ft_putstr_fd("\033[0;36m\033[1mminishell ▸ \033[0m", STDERR);
        if (get_next_line(0, &line) == -2 && (data.exit = 1))
		    ft_putendl_fd("exit by GNL", STDERR);
        if (sanitise_args(line) != 0)
        {
            free(line);
            ft_putendl_fd("Error : Sanitise Args, wrong args ? ", STDERR);
        }
        if (tokenize(&data, line) != 0)
            ft_putendl_fd("Error : Tokenize ", STDERR);

        t_token *curr = data.token;
        int i = 0;
        while(curr)
        {
            printf("cmd id -> [%d] | value -> [%s] | type -> [%d]\n", i, curr->str, curr->type);
            i++;
            curr = curr->next;
        }

        // if (fgets(cmd, MAX_CMD_LEN, stdin) == NULL) {
        //     // L'utilisateur a tapé CTRL+D
        //     printf("Exit <3\n");
        //     exit(0);
        // }
        // cmd[strcspn(cmd, "\n")] = 0;  // Enlever le retour à la ligne
        // printf("fgets => (%s)\n", cmd);
        // execute_command(cmd, &data);
    }

    // exit_shell(&data);
    return (0);
}
