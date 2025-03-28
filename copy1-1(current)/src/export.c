/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 10:35:57 by vagarcia          #+#    #+#             */
/*   Updated: 2025/03/25 16:10:24 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/planer.h"


static void	print_sorted_env(char **env)
{
    int		i;

    if (!env)
       return ;
    i = 0;
    while (env[i] != NULL)
        printf("declare -x %s\n", env[i++]);
}

void	update_or_add_env(char *arg, char **env)
{
    char	*key;
    char	*value;
    char	*eq;
    int		i;

    eq = ft_strchr(arg, '=');
	i = 0;
    if (eq)
    {
        key = ft_substr(arg, 0, eq - arg);
        value = ft_strdup(eq + 1);
        while (env[i++])
        {
            if (ft_strncmp(env[i], key, ft_strlen(key) == 0) && env[i][ft_strlen(key)] == '=')
            {
                free(env[i]);
                env[i] = ft_strjoin(key, "=");
                env[i] = ft_strjoin(env[i], value);
                free(key);
                free(value);
                return;
            }
        }
        add_new_env_var(arg, env); // Add new variable if not found
        free(key);
        free(value);
    }
    else if (!add_new_env_var(arg, env)) // Add variable without value
        return;
}


static int	is_valid_identifier(char *arg)
{
    int i;

    i = 1;
    if (!ft_isalpha(arg[0]) && arg[0] != '_')
        return (0);
    while (arg[i] && arg[i] != '=')
    {
        if (!ft_isalnum(arg[i]) && arg[i] != '_')
            return (0);
        i++;
    }
    return (1);
}

void	ft_export(t_cmd *cmd)
{
    int		i;

    if (!cmd->args[1])
    {
        print_sorted_env(cmd->env);
        return;
    }
    i = 1;
    while (cmd->args[i])
    {
        if (!is_valid_identifier(cmd->args[i]))
        {
            write(2, "export: not a valid identifier\n", 31);
            cmd->exit_status = 1;
        }
        else
        {
            update_or_add_env(cmd->args[i], cmd->env);
            cmd->exit_status = 0;
        }
        i++;
    }
}