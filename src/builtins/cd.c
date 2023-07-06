

#include "minishell.h"

char		*get_env_name(char *dest, const char *src)
{
	int		i;

	i = 0;
	while (src[i] && src[i] != '=' && ft_strlen(src) < 4096)
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return (dest);
}

int add_new_enve(t_env *env, char *key, char *value)
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


// IS IN ENV

int is_in_env(t_env *env, char *key)
{
	while(env && ft_strcmp(key, env->key) != 0)
		env = env->next;
	if (env)
		return (1);
	return (0);
}

static void		print_error(char **args)
{
	ft_putstr_fd("cd: ", 2);
	if (args[2])
		ft_putstr_fd("string not in pwd: ", 2);
	else
	{
		ft_putstr_fd(strerror(errno), 2);
		ft_putstr_fd(": ", 2);
	}
	ft_putendl_fd(args[1], 2);
}

static char		*get_env_path(t_env *env, const char *var, size_t len)
{
	char	*oldpwd;
	char *tmp;

	tmp = env->get_joined_env(env);

	while (env && env->next != NULL)
	{
		if (ft_strncmp(tmp, var, len) == 0)
		{
			oldpwd = env->value;
			return (oldpwd);
		}
		env = env->next;
	}
	return (NULL);
}

static int		update_oldpwd(t_env *env)
{
	char	cwd[PATH_MAX];
	char	*oldpwd;

	if (getcwd(cwd, PATH_MAX) == NULL)
		return (1);
	if (!(oldpwd = ft_strjoin("OLDPWD=", cwd)))
		return (1);
	if (is_in_env(env, oldpwd) == 0)
		add_new_enve(env, "OLDPWD", cwd);
	ft_memdel(oldpwd);
	return (0);
}

static int		go_to_path(int option, t_env *env)
{
	int		ret;
	char	*env_path;

	env_path = NULL;
	if (option == 0)
	{
		update_oldpwd(env);
		env_path = get_env_path(env, "HOME", 4);
		if (!env_path)
			ft_putendl_fd("minishell : cd: HOME not set", STDERR);
		if (!env_path)
			return (1);
	}
	else if (option == 1)
	{
		env_path = get_env_path(env, "OLDPWD", 6);
		if (!env_path)
			ft_putendl_fd("minishell : cd: OLDPWD not set", STDERR);
		if (!env_path)
			return (1);
		update_oldpwd(env);
	}
	ret = chdir(env_path);
	ft_memdel(env_path);
	return (ret);
}

int				ft_cd(char **args, t_env *env)
{
	int		cd_ret;

	if (!args[1])
		return (go_to_path(0, env));
	if (ft_strcmp(args[1], "-") == 0)
		cd_ret = go_to_path(1, env);
	else
	{
		update_oldpwd(env);
		cd_ret = chdir(args[1]);
		if (cd_ret < 0)
			cd_ret *= -1;
		if (cd_ret != 0)
			print_error(args);
	}
	return (cd_ret);
}
