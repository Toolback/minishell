#include "minishell.h"

// Gestionnaire de signaux
void handle_signal(int sig) {
    if (sig == SIGINT) {
        printf("\nminishell2 ▸ ");
        fflush(stdout);
    }

    printf("SIG RETRIEVED : [%d]", sig);
}

int main(int ac, char **av, char **env) {
    t_data data;
    (void)ac;
	(void)av;

    init_data(&data);
    if (init_env(&data, env, 1) != 0 || init_env(&data, env, 2) != 0)
    {
        //exit_shell(&data);
        return (1);
    }
	increment_shell_level(data.env);
    signal(SIGINT, handle_signal);
    while (data.exit == 0) {
        printf("minishell ▸ ");
        // super_parser(data);
        // super_executer(data);
    }
    // rl_clear_history();
    // exit_shell(&data);
    return (0);
}
