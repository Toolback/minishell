
#ifndef MINISHELL_H
# define MINISHELL_H

# include "../libft/includes/libft.h"
# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>
# include <string.h>
# include <fcntl.h>
# include <dirent.h>
# include <sys/wait.h>
# include <limits.h>
# include <errno.h>
# include <signal.h>

#define MAX_CMD_LEN 256
#define MAX_NUM_ARGS 10
# define STDIN 0
# define STDOUT 1
# define STDERR 2
# define MAX_INT 2147483647

typedef enum		e_token_type
{
	and,
	or,
	semicolon,
	simple_redir_left,
	simple_redir_right,
	double_redir_left,
	double_redir_right,
	pipeline,
	option_command,
	single_quote,
	double_quote,
	backslash,
	literal,
	variable,
	space,
}					t_token_type;

typedef struct	s_token
{
	char			*str;
	t_token_type	type;
	struct s_token	*prev;
	struct s_token	*next;
}				t_token;

typedef struct	s_env
{
	char			*key;
	char			*value;
	char				*(*get_joined_env)(struct s_env*);
	struct s_env	*next;
}				t_env;

typedef struct s_data
{
    t_token     *token;
    t_env       *env;
    t_env       *secret_env;
	int			in;
	int			out;
	int			exit;
}               t_data;

//	env/
//  init_env.c
//

int		init_env(t_data *data, char **local_env, int flag);
char	*get_joined_env(t_env *curr_env);

//	env/
//  get_env.c
//
t_env   *get_env_with_key(char *key, t_env *env_list);
t_env   *get_last_env(t_env *env_list);
//	env/
//  set_env.c
//
void increment_shell_level(t_env *env_list);
int add_new_env(t_env *env, char *key, char *value);
//	shell_data/
//  init_data.c
//
int 	init_data(t_data *data);

// parsing /
// token.c
t_token *add_token(char *line, int *i);
int tokenize(t_data *data, char *line);

//	utils/
//  free.c
//
void	free_env(t_env *env);

// builtins /
// env.c/
//
int		ft_env(t_env *env);


#endif