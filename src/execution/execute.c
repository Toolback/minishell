#include "minishell.h"

// Définit les limites pour le nombre d'arguments d'une commande et la taille d'une ligne
#define MAX_ARGS 50
#define MAX_LINE 1024

// Fonction pour exécuter une commande
void execute_command(t_data data)
{
    // Déclare des descripteurs de fichier et des variables pour les entrées et sorties
    int fds[2];
    int in = 0;
    int out;
    t_token *t = data.token;
    pid_t pid;
    // Compteur pour les processus enfants
    int child_processes = 0;

    // Boucle tant que le token n'est pas NULL
    while (t != NULL)
    {
        out = 1; // Réinitialise 'out' pour chaque commande

        // Si le type de token est une commande
        if (t->type == cmd)
        {
            // Crée un tableau pour les arguments de la commande
            char *args[MAX_ARGS] = {t->str};
            int i = 1;
            // Remplit le tableau avec les arguments de la commande jusqu'à un certain type de token ou une certaine limite
            while (t->next && t->next->type != pipeline && t->next->type != simple_redir_right &&
                   t->next->type != simple_redir_left && t->next->type != double_redir_right &&
                   t->next->type != double_redir_left && i < MAX_ARGS - 1)
            {
                t = t->next;
                args[i++] = t->str;
            }
            args[i] = NULL;

            // Gère les redirections après les arguments de la commande
            if (t->next && (t->next->type == simple_redir_right || t->next->type == double_redir_right ||
                            t->next->type == simple_redir_left || t->next->type == double_redir_left))
            {
                t = t->next;
                // Pour les différents types de redirections, ouvre les fichiers appropriés
                if (t->type == simple_redir_right)
                {
                    t = t->next;
                    out = open(t->str, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
                }
                else if (t->type == double_redir_right)
                {
                    t = t->next;
                    out = open(t->str, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
                }
                else if (t->type == simple_redir_left)
                {
                    t = t->next;
                    in = open(t->str, O_RDONLY);
                }
                else if (t->type == double_redir_left)
                {
                    // Pour la redirection double vers la gauche, crée un fichier temporaire et y écrit les lignes d'entrée
                    t = t->next;
                    char *input_lines = t->str;
                    char *end_word = strtok(input_lines, "\n");
                    char tmp_filename[] = "/tmp/double_redir_left_XXXXXX";
                    in = mkstemp(tmp_filename);
                    char *line = strtok(NULL, "\n");
                    while (line && strcmp(line, end_word) != 0)
                    {
                        write(in, line, strlen(line));
                        write(in, "\n", 1);
                        line = strtok(NULL, "\n");
                    }
                    lseek(in, 0, SEEK_SET); // Retourne au début du fichier temporaire
                }
            }

            // Configure le pipeline
            pipe(fds);

            // Crée un nouveau processus
            if ((pid = fork()) == -1)
            {
                perror("fork");
                exit(1);
            }

            // Si le processus est un processus enfant
            if (pid == 0)
            {
                // Redirige stdin si nécessaire
                if (in != 0)
                {
                    dup2(in, 0);
                    close(in);
                }

                // Redirige stdout si nécessaire
                if (out != 1)
                {
                    dup2(out, 1);
                    close(out);
                }

                // Redirige stdout vers le pipeline si ce n'est pas la dernière commande
                if (t->next && t->next->type == pipeline)
                {
                    dup2(fds[1], 1);
                }
                close(fds[0]);
                close(fds[1]);

                // Prépare les variables d'environnement si nécessaire
                char **envp = NULL;
                // Exécute la commande
                if (strcmp(args[0], "echo") == 0)
                {
                    // Appelle ft_echo avec les arguments appropriés.
                    // printf("tezt");
                    ft_echo(args);
                }
                else if (strcmp(args[0], "cd") == 0)
                {
                    // Appelle ft_echo avec les arguments appropriés.
                    // printf("tezt");
                    //ft_cd(args, data.env);
                }
                else if (strcmp(args[0], "pwd") == 0)
                {
                    // Appelle ft_echo avec les arguments appropriés.
                    // printf("tezt");
                    ft_pwd();
                }
                else if (strcmp(args[0], "env") == 0)
                {
                    // Appelle ft_echo avec les arguments appropriés.
                    // printf("tezt");
                    ft_env(data.env);
                }

                else if (execve(args[0], args, envp) == -1)
                {
                    perror("execve");
                    printf("Commande non trouvée [%s]\n", args[0]);
                    exit(EXIT_FAILURE);
                }
                exit(EXIT_SUCCESS);
            }
            // Si le processus est le processus parent
            else
            {
                child_processes++; // Incrémente le compteur de processus enfants
                close(fds[1]);     // Ferme l'extrémité d'écriture du pipeline immédiatement
                if (in != 0)
                {
                    close(in); // Ferme le descripteur de fichier d'entrée de la boucle précédente
                }
                in = fds[0]; // Garde la trace de l'entrée pour la prochaine commande
            }
        }
        t = t->next;
    }

    if (in != 0)
    {
        close(in);
    }

    // Attend que tous les processus enfants se terminent
    for (int i = 0; i < child_processes; i++)
    {
        wait(NULL);
    }
}

void super_executer(t_data data)
{
    execute_command(data);
}
