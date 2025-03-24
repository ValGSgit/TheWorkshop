/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/31 13:34:42 by vagarcia          #+#    #+#             */
/*   Updated: 2025/03/24 16:47:19 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/planer.h"

/* Executes a single command (builtin or external) */
static void	execute_single_command(t_cmd *cmd, t_shell *shell)
{
    pid_t	pid;

    if (is_builtin(cmd->args[0]))
    {
        execute_builtin(cmd);
        return;
    }
    pid = fork();
    if (pid == 0)
    {
        handle_dup(cmd); // Set up redirections
        execve(cmd->args[0], cmd->args, shell->env);
        perror(cmd->args[0]); // Print error if execve fails
        exit(127); // Exit with 127 if command not found
    }
    else
    {
        waitpid(pid, &shell->exit_status, 0);
        if (WIFEXITED(shell->exit_status))
            shell->exit_status = WEXITSTATUS(shell->exit_status);
        else
            shell->exit_status = 1; // Handle signals
        restore_redirections(cmd); // Restore redirections
    }
}

/* Executes a pipeline of commands */
static void	execute_pipeline(t_cmd *cmd, t_shell *shell)
{
    int		pipe_fd[2];
    int		prev_pipe_in = 0;
    pid_t	pid;

    while (cmd)
    {
        if (cmd->next)
        {
            if (pipe(pipe_fd) == -1) // Create a pipe for the next command
            {
                perror("pipe");
                return;
            }
        }
        pid = fork();
        if (pid == -1)
        {
            perror("fork");
            return;
        }
        if (pid == 0) // Child process
        {
            if (prev_pipe_in != 0) // Redirect input from the previous pipe
            {
                dup2(prev_pipe_in, STDIN_FILENO);
                close(prev_pipe_in);
            }
            if (cmd->next) // Redirect output to the current pipe
            {
                dup2(pipe_fd[WRITE_END], STDOUT_FILENO);
                close(pipe_fd[WRITE_END]);
                close(pipe_fd[READ_END]);
            }
            handle_dup(cmd); // Set up redirections
            execve(cmd->args[0], cmd->args, shell->env);
            perror(cmd->args[0]); // Print error if execve fails
            exit(127);
        }
        else // Parent process
        {
            if (prev_pipe_in != 0) // Close the previous pipe's read end
                close(prev_pipe_in);
            if (cmd->next) // Close the current pipe's write end
                close(pipe_fd[WRITE_END]);
            prev_pipe_in = cmd->next ? pipe_fd[READ_END] : 0; // Update for the next iteration
            cmd = cmd->next;
        }
    }
    while (wait(NULL) > 0);
}

/* Handles redirections for a command */
void	handle_dup(t_cmd *cmd)
{
    if (cmd->in_fd != STDIN_FILENO)
    {
        dup2(cmd->in_fd, STDIN_FILENO);
        close(cmd->in_fd);
    }
    if (cmd->out_fd != STDOUT_FILENO)
    {
        dup2(cmd->out_fd, STDOUT_FILENO);
        close(cmd->out_fd);
    }
}

/* Restores redirections after command execution */
void	restore_redirections(t_cmd *cmd)
{
    if (cmd->in_fd != STDIN_FILENO)
        close(cmd->in_fd);
    if (cmd->out_fd != STDOUT_FILENO)
        close(cmd->out_fd);
}

/* Main executor function */
void	executor(t_cmd *cmd, t_shell *shell)
{
    if (!cmd)
        return;
    if (!cmd->next) // Single command
        execute_single_command(cmd, shell);
    else // Pipeline
        execute_pipeline(cmd, shell);
}