

#include "minishell.h"

// static int	print_error(int error, const char *arg)
// {
// 	int		i;

// 	if (error == -1)
// 		ft_putstr_fd("export: not valid in this context: ", STDERR);
// 	else if (error == 0 || error == -3)
// 		ft_putstr_fd("export: not a valid identifier: ", STDERR);
// 	i = 0;
// 	while (arg[i] && (arg[i] != '=' || error == -3))
// 	{
// 		write(STDERR, &arg[i], 1);
// 		i++;
// 	}
// 	write(STDERR, "\n", 1);
// 	return (1);
// }

int get_total_env(t_env *list)
{
	int	i;
	t_env	*temp;

	i = 0;
	temp = list;
	while (temp->next)
	{
		i++;
		temp = temp->next;
	}
	return (i);
}

char	**env_to_arr(t_env *list, int env_count)
{
	int		i;
	char	**res;
	t_env	*temp;

	i = 0;
	temp = list;
	if (!(res = (char **)malloc(sizeof(char *) * env_count)))
		return NULL;
	while (i <= env_count)
	{
		// res[i] = (char *)malloc(sizeof(char) * (ft_strlen(temp->key) + ft_strlen(temp->value) + 1));
		res[i] = temp->get_joined_env(temp);
		temp = temp->next;
		i++;
	}
	res[i] = NULL;
	return (res);
}

// void		sort_env(char **tab, int env_len)
// {
// 	int		ordered;
// 	int		i;
// 	char	*tmp;

// 	ordered = 0;
// 	while (tab && ordered == 0)
// 	{
// 		ordered = 1;
// 		i = 0;
// 		while (i < env_len - 1)
// 		{
// 			if (ft_strcmp(tab[i], tab[i + 1]) > 0)
// 			{
// 				tmp = tab[i];
// 				tab[i] = tab[i + 1];
// 				tab[i + 1] = tmp;
// 				ordered = 0;
// 			}
// 			i++;
// 		}
// 		env_len--;
// 	}
// }

int			ft_export(t_env *env)
{
	int		i;
	int		env_count;
	char	**tab;

	env_count = get_total_env(env);
	tab = env_to_arr(env, env_count);
	sort_env(tab, env_count);
	i = 0;
	while (tab[i] != NULL)
	{
		ft_putstr("declare -x ");
		ft_putendl(tab[i]);
		i++;
	}
	free_arr(tab);
	return 0;
}