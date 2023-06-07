

#include "minishell.h"

int		ft_env(t_env *env)
{
	while (env && env->next != NULL)
	{
		ft_putendl(env->get_joined_env(env));
		env = env->next;
	}
	if (env)
		ft_putendl(env->get_joined_env(env));
	return (0);
}
