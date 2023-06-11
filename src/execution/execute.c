# include "minishell.h"

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

void super_executer(t_data data)
{
    char cmd[MAX_CMD_LEN];
    if (fgets(cmd, MAX_CMD_LEN, stdin) == NULL) {
        // L'utilisateur a tapé CTRL+D
        printf("Exit <3\n");
        exit(0);
    }
    cmd[strcspn(cmd, "\n")] = 0;  // Enlever le retour à la ligne
    printf("fgets => (%s)\n", cmd);
    execute_command(cmd, &data);    
}