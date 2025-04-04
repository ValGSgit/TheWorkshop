/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/31 12:01:26 by vagarcia          #+#    #+#             */
/*   Updated: 2025/04/04 18:01:55 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "../ShellLibft/libft.h"
# include "heredocs.h"
# include <dirent.h>
# include <errno.h>
# include <fcntl.h>
# include <limits.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

/* Defines */
# define LMAX "9223372036854775808"
# define LMIN "-9223372036854775809"
# define SUCCESS 0
# define FAILURE 1
# define PROMPT "minishell-> "
# define READ_END 0
# define WRITE_END 1
# define MAX_TOKENS 1024
# define HERE_TEMP "/tmp/heredoc_XXXXXX"
# define RESET_COLOR "\033[0m"
# define PROMPT_COLOR "\033[1;32m"
# define ERROR_COLOR "\033[1;31m"

/* Enums */
typedef enum e_redir_type
{
	REDIR_NONE,
	REDIR_IN,     // <
	REDIR_OUT,    // >
	REDIR_APPEND, // >>
	REDIR_HEREDOC // <<
}					t_redir_type;

/* Structs */
typedef struct s_lexer
{
	char			*start;
	bool			in_quote;
	char			quote_char;
	int				tokcount;
}					t_lexer;

typedef struct s_redir
{
	t_redir_type	type;
	char			*file;
	struct s_redir	*next;
}					t_redir;

typedef struct s_expander_state
{
	char			*result;
	char			*dst;
	char			quote_char;
	bool			in_quote;
	bool			in_dquote;
}					t_expander_state;

typedef struct s_cmd
{
	int				in_fd;
	char			*in_file;
	int				out_fd;
	char			*out_file;
	char			**args;
	t_redir			*redirs;
	int				exit_status;
	bool			syntax_error;
	char			*err_msg;
	struct s_cmd	*next;
	struct s_shell	*shell;
}					t_cmd;

typedef struct s_shell
{
	char			**env;
	int				exit_status;
	bool			is_interactive;
	int				signal_status;
	t_cmd			*cmd;
}					t_shell;

/* Function Prototypes */

/* Debug */
void				debug_shell_state(char **tokens, t_cmd *cmd,
						const char *stage);

/* Main Loop */
void				minishell_loop(t_shell *shell);

/* Lexer */
char				**lexer(char *input);
int					handle_quotes(char **input, bool *in_quote,
						char *quote_char);

/* Parser */
t_cmd				*parser(char **tokens, t_shell *shell);

/* Expander */
char				*process_argument(char *arg, t_shell *shell);
char				*expand_variable(char *arg, int *i, t_shell *shell,
						bool in_dquote);
void				expand_nodes(t_cmd *cmd, t_shell *shell);
void				expander(t_cmd *cmd, t_shell *shell);

/* Executor */
void				executor(t_cmd *cmd, t_shell *shell);

/* Builtins */
void				execute_builtin(t_cmd *cmd);
int					is_builtin(char *cmd);
void				ft_echo(t_cmd *cmd);
void				ft_cd(t_cmd *cmd);
void				ft_pwd(t_cmd *cmd);
void				ft_export(t_cmd *cmd);
void				ft_unset(t_cmd *cmd);
void				ft_env(t_cmd *cmd);
void				ft_exit(t_cmd *cmd);

/* Redirections */
void				handle_heredoc(t_cmd *cmd, char *eof);
t_cmd				*create_cmd_node(void);
void				create_redir_node(t_cmd *cmd, int type, char *file);
void				restore_redirections(t_cmd *cmd);
void				apply_redirection(t_cmd *cmd);

/* Pipes */
void				execute_command(t_cmd *cmd, t_shell *shell);
void				handle_pipes(t_cmd *cmd);

/* Signals */
void				setup_signals(void);
void				reset_signals(void);
void				ignore_signals(void);
void				handle_sigint(int sig);
void				handle_sigquit(int sig);

/* Environment */
char				*resolve_path(char *cmd, char **env);
void				update_or_add_env(char *arg, char ***env);
char				**copy_env(char **env);
char				*get_env_value(char *name, char **env);
void				set_env_value(char *key, char *value, t_shell *shell);
void				ft_sort_strings(char **arr, int count);
int					add_new_env_var(char *arg, char **env);

/* Utils */
char				**ft_clean_args(char **args);
void				cleanup_heredocs(t_shell *shell);
bool				is_metacharacter(char *token);
void				add_argument(t_cmd *node, char *arg);
char				*update_prompt(void);
bool				is_special_char(char c);
int					ft_isspace(char c);
int					is_redir_without_space(char **input);
void				ft_free(char **arr);
void				free_cmd(t_cmd *cmd);
void				free_tokens(char **tokens);
void				free_env(char **env);
void				print_error(char *msg, char *arg);
bool				is_quoted(char *token);
void				free_shell(t_shell *shell);
char				*append_str(char *dest, char *src);
void	re_process_node_args(t_cmd *cmd, t_shell *shell);
#endif