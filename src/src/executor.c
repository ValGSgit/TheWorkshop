/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/31 13:34:42 by vagarcia          #+#    #+#             */
/*   Updated: 2025/03/25 14:05:28 by vagarcia         ###   ########.fr       */
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

    if (is_builtin(cmd->args[0]))
    {
        execute_builtin(cmd);
        exit(shell->exit_status);
    }
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

// /* Main executor function */
void	executor(t_cmd *cmd, t_shell *shell)
{
    if (!cmd)
        return;
    if (cmd->in_fd == -1 || cmd->out_fd == -1)
    {
        perror("Error opening file");
        shell->exit_status = 1;
        return;
    }
    if (!cmd->next) // Single command
        execute_single_command(cmd, shell);
    else // Pipeline
        execute_pipeline(cmd, shell);
}

/* Executes a command with redirections and pipes */
// static void execute_command_with_redirections(t_cmd *cmd, int prev_pipe_in, int *pipe_fd, t_shell *shell)
// {
//     pid_t pid;

//     pid = fork();
//     if (pid == -1)
//     {
//         perror("fork");
//         return;
//     }
//     if (pid == 0) // Child process
//     {
//         // Handle input redirection or pipe input
//         if (cmd->in_fd != STDIN_FILENO)
//             dup2(cmd->in_fd, STDIN_FILENO);
//         else if (prev_pipe_in != 0)
//             dup2(prev_pipe_in, STDIN_FILENO);

//         // Handle output redirection or pipe output
//         if (cmd->out_fd != STDOUT_FILENO)
//             dup2(cmd->out_fd, STDOUT_FILENO);
//         else if (pipe_fd)
//             dup2(pipe_fd[WRITE_END], STDOUT_FILENO);

//         // Close unused pipe ends
//         if (prev_pipe_in != 0)
//             close(prev_pipe_in);
//         if (pipe_fd)
//         {
//             close(pipe_fd[WRITE_END]);
//             close(pipe_fd[READ_END]);
//         }

//         // Apply additional redirections
//         handle_dup(cmd);

//         // Execute the command
//         if (is_builtin(cmd->args[0]))
//         {
//             execute_builtin(cmd);
//             exit(shell->exit_status);
//         }
//         else
//         {
//             execve(cmd->args[0], cmd->args, shell->env);
//             perror(cmd->args[0]);
//             exit(127);
//         }
//     }
//     else // Parent process
//     {
//         // Close unused pipe ends in the parent
//         if (prev_pipe_in != 0)
//             close(prev_pipe_in);
//         if (pipe_fd)
//             close(pipe_fd[WRITE_END]);

//         // Wait for the child process
//         waitpid(pid, &shell->exit_status, 0);
//         if (WIFEXITED(shell->exit_status))
//             shell->exit_status = WEXITSTATUS(shell->exit_status);
//         else
//             shell->exit_status = 1;
//     }
// }
