/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/31 13:33:20 by vagarcia          #+#    #+#             */
/*   Updated: 2025/03/26 11:31:49 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/planer.h"



/* Helper function (put in separate file) */
int	add_new_env_var(char *arg, char **env)
{
	int		count;
	char	**new_env;
	int		i;

	count = 0;
	if (!env)
		return (1);
	while (env[count])
		count++;
	new_env = malloc(sizeof(char *) * (count + 2));
	if (!new_env)
		return (1);
	i = 0;
	while (i < count)
	{
		new_env[i] = env[i];
		i++;
	}
	new_env[count] = ft_strdup(arg);
	new_env[count + 1] = 0;
	env = new_env;
	return (0);
}

void	ft_unset(t_cmd *cmd)
{
	char	**e;
	char	*arg;
	int		i;
	int		j;

	if (!cmd || !cmd->args)
		return;
	i = 1;
	cmd->exit_status = 0;
	while (cmd->args[i])
	{
		arg = cmd->args[i++];
		e = cmd->env;
		j = 0;
		while (e[j])
		{
			if (!ft_strncmp(e[j], arg, ft_strlen(arg))
				&& e[j][ft_strlen(arg)] == '=')
			{
				free(e[j]);
				while (e[j]) // Shift the remaining variables
				{
					e[j] = e[j + 1];
					j++;
				}
				break ;
			}
			j++;
		}
	}
}

int	ft_isnumber(char *str)
{
	int	i;

	i = 0;
	if (!str)
		return (0);
	if (str[i] == '-' || str[i] == '+')
		i++;
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

void	ft_exit(t_cmd *cmd)
{
	int	exit_code;

	exit_code = 0;
	if (!cmd->args[1])
		exit(exit_code);
	if (cmd->args[1] && cmd->args[2])
	{
		write(2, "exit: too many arguments\n", 26);
		exit_code = 1;
		return ;
	}
	if (cmd->args[1])
	{
		if ((!ft_isnumber(cmd->args[1]) && !cmd->args[2])
		|| (ft_strcmp(cmd->args[1], "9223372036854775808") == 0
		|| ft_strcmp(cmd->args[1], "-9223372036854775809") == 0))
		{
			write(2, "exit: ", 6);
			write(2, cmd->args[1], ft_strlen(cmd->args[1]));
			write(2, "numeric argument required\n", 27);
			cmd->exit_status = 2;
			exit(2);
		}
		exit_code = ft_atoi(cmd->args[1]);
	}
	exit(exit_code % 256);
}

int	is_builtin(char *cmd)
{
	if (!cmd)
		return (0);
	if (ft_strcmp(cmd, "echo") == 0)
		return (1);
	else if (ft_strcmp(cmd, "cd") == 0)
		return (1);
	else if (ft_strcmp(cmd, "pwd") == 0)
		return (1);
	else if (ft_strcmp(cmd, "export") == 0)
		return (1);
	else if (ft_strcmp(cmd, "unset") == 0)
		return (1);
	else if (ft_strcmp(cmd, "env") == 0)
		return (1);
	else if (ft_strcmp(cmd, "exit") == 0)
		return (1);
	return (0);
}

void	execute_builtin(t_cmd *cmd)
{
	if (ft_strcmp(cmd->args[0], "echo") == 0)
		ft_echo(cmd);
	else if (ft_strcmp(cmd->args[0], "cd") == 0)
		ft_cd(cmd);
	else if (ft_strcmp(cmd->args[0], "pwd") == 0)
		ft_pwd(cmd);
	else if (ft_strcmp(cmd->args[0], "export") == 0)
		ft_export(cmd);
	else if (ft_strcmp(cmd->args[0], "unset") == 0)
		ft_unset(cmd);
	else if (ft_strcmp(cmd->args[0], "env") == 0)
		ft_env(cmd);
	else if (ft_strcmp(cmd->args[0], "exit") == 0)
		ft_exit(cmd);
}


void	ft_echo(t_cmd *cmd)
{
    int		i;
    int		n_flag;

    i = 1;
    n_flag = 0;
    if (cmd->args[1] && ft_strcmp(cmd->args[1], "-n") == 0)
    {
        n_flag = 1;
        i++;
    }
    while (cmd->args[i])
    {
        printf("%s", cmd->args[i]);
        if (cmd->args[i + 1])
            printf(" ");
        i++;
    }
    if (!n_flag)
        printf("\n");
}

void	ft_cd(t_cmd *cmd)
{
    char	*path;

    if (!cmd->args[1])
        path = getenv("HOME");
    else if (cmd->args[2])
    {
        write(2, "cd: too many arguments\n", 24);
        cmd->exit_status = 1;
        return;
    }
    else
        path = cmd->args[1];
    if (chdir(path) != 0)
    {
        perror("cd");
        cmd->exit_status = 1;
    }
    else
        cmd->exit_status = 0;
}

void	ft_pwd(t_cmd *cmd)
{
    char	cwd[1024];

    if (cmd->args[1])
    {
        write(2, "pwd: too many arguments\n", 24);
        cmd->exit_status = 1;
        return;
    }
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        printf("%s\n", cwd);
    else
        perror("pwd");
    cmd->exit_status = 0;
}


void	ft_env(t_cmd *cmd)
{
    int		i;

    if (cmd->args[1])
    {
        write(2, "env: too many arguments\n", 25);
        cmd->exit_status = 1;
        return;
    }
    i = 0;
    while (cmd->env[i])
    {
        printf("%s\n", cmd->env[i]);
        i++;
    }
}

