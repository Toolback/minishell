#include "../../includes/minishell.h"


t_env   *get_env(char *key, t_env *env_list)
{
    while(env_list && ft_strcmp(key, env_list->key) != 0)
        env_list = env_list->next;
    return (env_list);
}