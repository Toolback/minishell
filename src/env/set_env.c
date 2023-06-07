#include "../../includes/minishell.h"


void increment_shell_level(t_env *env_list)
{
    t_env *shlvl;
    int lvl;

    shlvl = get_env_with_key("SHLVL", env_list);
    lvl = ft_atoi(shlvl->value) + 1;
    free(shlvl->value);
    shlvl->value = ft_strdup(ft_itoa(lvl));
}

int add_new_env(t_env *env, char *key, char *value)
{
    t_env *new;
    t_env *list;

    list = get_last_env(env);
    if (!(new = malloc(sizeof(t_env))))
		return (1);
    new->key = key;
    new->value = value;
    new->get_joined_env = &get_joined_env;
    new->next = NULL;
    list->next = new;
    return (0);
}