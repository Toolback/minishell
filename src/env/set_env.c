#include "../../includes/minishell.h"


void increment_shell_level(t_env *env_list)
{
    t_env *shlvl;
    int lvl;

    shlvl = get_env("SHLVL", env_list);
    lvl = ft_atoi(shlvl->value) + 1;
    free(shlvl->value);
    shlvl->value = ft_strdup(ft_itoa(lvl));
}