#include "../../includes/minishell.h"


void increment_shell_level(t_env *env_list)
{
    t_env *shlvl;
    char  *temp;
    int lvl;

    lvl = 0;
    shlvl = get_env_with_key("SHLVL", env_list);
    lvl = ft_atoi(shlvl->value);
    lvl++;
    free(shlvl->value);
    temp = ft_itoa(lvl);
    shlvl->value = ft_strdup(temp);
    free(temp);
}

int add_new_env(t_env *env, char *key, char *value)
{
    t_env *new;
    t_env *list;
    // ft_printf("KEEEEEY [%s]", key);
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