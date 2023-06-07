#include "../../includes/minishell.h"

// Fonction pour exécuter une commande
void execute_command(char *cmd) {
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
    } else {
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

void    print_env(t_data *data)
{
    int     i;
    t_env   *env;
    
    env = data->env;
    i = 0;
    while(env)
    {
        // printf("env (%d) : \t[%s]\n", i, env->value);
        printf("env (%d) : \t[%s]\n", i, env->get_joined_env(env));
        env = env->next;
        i++;
    }
}

int main(int ac, char **av, char **env) {
    t_data data;
    // char cmd[MAX_CMD_LEN];

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
    print_env(&data);

    // signal(SIGINT, handle_signal);
    // while (1) {
    //     printf("minishell ▸ ");
    //     if (fgets(cmd, MAX_CMD_LEN, stdin) == NULL) {
    //         // L'utilisateur a tapé CTRL+D
    //         printf("\n");
    //         exit(0);
    //     }
    //     cmd[strcspn(cmd, "\n")] = 0;  // Enlever le retour à la ligne
    //     execute_command(cmd);
    // }

    // exit_shell(&data);
    return (0);
}
