/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/31 13:57:26 by vagarcia          #+#    #+#             */
/*   Updated: 2025/04/04 18:07:58 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char	*append_str(char *dest, char *src)
{
	char	*new_str;

	if (!dest)
	{
		if (src)
			return (ft_strdup(src));
		else
			return (ft_strdup(""));
	}
	if (src && *src)
		new_str = ft_strjoin(dest, src);
	else
		new_str = ft_strjoin(dest, "");
	free(dest);
	return (new_str);
}

char	*process_argument(char *arg, t_shell *shell)
{
	t_expander_state	state;
	int					i;
	char				*value;

	ft_memset(&state, 0, sizeof(t_expander_state));
	state.result = ft_strdup("");
	 if (!arg)
	 	return (state.result);
	i = -1;
	while (arg[++i])
	{
		if (arg[i] == '\'' && !state.in_dquote)
			state.in_quote = !state.in_quote;
		else if (arg[i] == '"' && !state.in_quote)
			state.in_dquote = !state.in_dquote;
		else if (arg[i] == '$' && !state.in_quote)
		{
			value = expand_variable(arg, &i, shell, state.in_dquote);
			state.result = append_str(state.result, value);
			free(value);
		}
		else
			state.result = append_str(state.result, (char[]){arg[i], '\0'});
	}
	return (state.result);
}

void	expander(t_cmd *cmd, t_shell *shell)
{
	int		i;
	char	*expanded;

	if (!cmd || !cmd->args)
		return ;
	if ((ft_strncmp(cmd->args[0], "./", 2) != 0 || ft_strncmp(cmd->args[0],
				"../", 3)) && !is_builtin(cmd->args[0]))
	{
		expanded = resolve_path(cmd->args[0], shell->env);
		free(cmd->args[0]);
		cmd->args[0] = expanded;
	}
	i = 0;
	while (cmd->args && cmd->args[i])
	{
		expanded = process_argument(cmd->args[i], shell);
		if (expanded)
		{
			free(cmd->args[i]);
			cmd->args[i] = expanded;
		}
		i++;
	}
}

void	expand_nodes(t_cmd *cmd, t_shell *shell)
{
	t_cmd	*node;

	node = cmd;
	shell->cmd = node;
	while (node)
	{
		expander(node, shell);
		if (node->args)
			node->args = ft_clean_args(node->args);
		node->shell = shell;
		node = node->next;
	}
}
