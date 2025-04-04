/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 12:02:58 by vagarcia          #+#    #+#             */
/*   Updated: 2025/04/04 18:04:12 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/* Remove null or empty strings from args array */
char	**ft_clean_args(char **args)
{
	int		i;
	int		j;
	char	**cleaned;

	i = 0;
	j = 0;
	if (!args)
		return (NULL);
	while (args[i])
		i++;
	cleaned = malloc(sizeof(char *) * (i + 1));
	i = 0;
	if (!cleaned)
		return (NULL);
	while (args[i])
	{
		if (args[i] && *args[i])
			cleaned[j++] = args[i];
		else
			free(args[i]);
		i++;
	}
	cleaned[j] = NULL;
	free(args);
	return (cleaned);
}

static char	*handle_empty_var_name(char *var_name)
{
	free(var_name);
	return (ft_strdup("$"));
}

static char	*handle_miss(int *i, char *str, int strt_len[2], bool in_dquote)
{
	char	*value;

	if (in_dquote)
		value = ft_strdup("");
	else
	{
		value = ft_strdup("");
		*i += strt_len[1];
		value = append_str(value, str + strt_len[0] + strt_len[1]);
	}
	*i = ft_strlen(str) - 1;
	return (value);
}

char	*expand_variable(char *str, int *i, t_shell *shell, bool in_dquote)
{
	char	*var_name;
	char	*value;
	int		strt_len[2];

	strt_len[0] = *i + 1;
	strt_len[1] = 0;
	if (!str[strt_len[0]])
		return (ft_strdup("$"));
	if (str[strt_len[0]] == '?')
	{
		*i += 1;
		return (ft_itoa(shell->exit_status));
	}
	while (str[strt_len[0] + strt_len[1]] && ft_isalnum(str[strt_len[0]
			+ strt_len[1]]))
		strt_len[1]++;
	var_name = ft_substr(str, strt_len[0], strt_len[1]);
	if (!var_name || !var_name[0])
		return (handle_empty_var_name(var_name));
	value = get_env_value(var_name, shell->env);
	*i += strt_len[1];
	if (!value)
		return (free(var_name), handle_miss(i, str, strt_len, in_dquote));
	return (free(var_name), ft_strdup(value));
}

// char	*expand_variable(char *str, int *i, t_shell *shell, bool in_dquote)
// {
// 	char *var_name;
// 	char *value;
// 	int strt_len[2];

// 	strt_len[0] = *i + 1;
// 	strt_len[1] = 0;

// 	// Handle case where `$"` is treated as a literal
// 	if (str[strt_len[0]] == '"')
// 	{
// 		(*i)++; // Skip the `$` and treat the rest as a literal
// 		//return (ft_strdup("$"));
// 	}
// 	// Handle case where `$` is at the end of the string
// 	if (!str[strt_len[0]])
// 		return (ft_strdup("$"));
// 	// Handle `$?` for exit status
// 	if (str[strt_len[0]] == '?')
// 	{
// 		*i += 2; // Skip `$?`
// 		return (ft_itoa(shell->exit_status));
// 	}
// 	// Calculate the length of the variable name
// 	while (str[strt_len[0] + strt_len[1]] && ft_isalnum(str[strt_len[0]
// 			+ strt_len[1]]))
// 		strt_len[1]++;
// 	var_name = ft_substr(str, strt_len[0], strt_len[1]);
// 		// Extract variable name
// 	if (!var_name || !var_name[0])                      
// 		// Handle empty variable name
// 		return (handle_empty_var_name(var_name));
// 	value = get_env_value(var_name, shell->env); // Get the variable's value
// 	free(var_name);
// 	if (!value) // Handle missing variable
// 		return (handle_miss(i, str, strt_len, in_dquote));
// 	*i += strt_len[1];        
// 	return (ft_strdup(value)); // Return a copy of the value
// }