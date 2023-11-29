/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amassias <amassias@student.42lehavre.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/29 13:02:57 by amassias          #+#    #+#             */
/*   Updated: 2023/11/29 13:45:58 by amassias         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file exec.c
 * @author Antoine Massias (amassias@student.42lehavre.com)
 * @date 2023-11-29
 * @copyright Copyright (c) 2023
 */

/* ************************************************************************** */
/*                                                                            */
/* Includes                                                                   */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"

#include "libft.h"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* ************************************************************************** */
/*                                                                            */
/* Helper prototypes                                                          */
/*                                                                            */
/* ************************************************************************** */

/**
 * @brief 
 * @param out 
 * @param args 
 * @param argv 
 * @return int 
 * @author Antoine Massias (amassias@student.42lehavre.fr)
 * @date 2023-11-29
 * @todo Make documentation.
 */
static int	_continue_on_error(
				int out,
				t_args *args,
				char **argv);

/**
 * @brief 
 * @param command_pid 
 * @param args 
 * @param out 
 * @param pipe_fd 
 * @return int 
 * @author Antoine Massias (amassias@student.42lehavre.fr)
 * @date 2023-11-29
 * @todo Make documentation.
 */
static int	_continue_exec(
				pid_t command_pid,
				t_args *args,
				int out,
				int pipe_fd[2]);

/**
 * @brief 
 * @param args 
 * @param fds 
 * @param cmd 
 * @param argv 
 * @author Antoine Massias (amassias@student.42lehavre.fr)
 * @date 2023-11-29
 * @todo Make documentation.
 */
static void	_child_process(
				t_args *args,
				int fds[3],
				char *cmd,
				char **argv);

/* ************************************************************************** */
/*                                                                            */
/* Header implementations                                                     */
/*                                                                            */
/* ************************************************************************** */

int	execute_sub_process_list(
		t_args *args,
		int out)
{
	pid_t	command_pid;
	char	*cmd;
	char	**argv;
	int		pipe_fd[3];

	--args->end;
	argv = ft_split(*args->end, ' ');
	if (argv == NULL)
		return (_continue_on_error(out, args, argv));
	cmd = get_absolute_path(argv[0], args->paths);
	if (cmd == NULL)
		return (_continue_on_error(out, args, argv));
	pipe_fd[PIPE__WRITE] = args->null_fd;
	if (args->start == args->end)
		pipe_fd[PIPE__READ] = args->input_fd;
	else if (pipe(pipe_fd))
		pipe_fd[PIPE__READ] = args->null_fd;
	command_pid = fork();
	pipe_fd[2] = out;
	if (command_pid == 0)
		_child_process(args, pipe_fd, cmd, argv);
	(free(cmd), free_string_list(&argv));
	if (command_pid == 0)
		exit(EXIT_FAILURE);
	return (_continue_exec(command_pid, args, out, pipe_fd));
}

/* ************************************************************************** */
/*                                                                            */
/* Helper implementations                                                     */
/*                                                                            */
/* ************************************************************************** */

static int	_continue_on_error(
				int out,
				t_args *args,
				char **argv)
{
	if (argv != NULL)
		ft_fprintf(STDERR_FILENO, ERROR_COMMAND_NOT_FOUND, g_pname, argv[0]);
	else
		error_message(g_pname, ERROR_INT);
	free_string_list(&argv);
	close(out);
	if (args->start != args->end)
		execute_sub_process_list(args, args->null_fd);
	return (127);
}

static int	_continue_exec(
				pid_t command_pid,
				t_args *args,
				int out,
				int pipe_fd[2])
{
	int	ret;

	if (command_pid < 0)
		return (error_message(g_pname, ERROR_INT), RET_ERR);
	if (out != args->null_fd)
		close(out);
	if (pipe_fd[PIPE__READ] != args->null_fd)
		close(pipe_fd[PIPE__READ]);
	if (args->start != args->end)
		execute_sub_process_list(args, pipe_fd[PIPE__WRITE]);
	waitpid(command_pid, &ret, 0);
	return (ret);
}

static void	_child_process(
				t_args *args,
				int fds[3],
				char *cmd,
				char **argv)
{
	close(fds[PIPE__WRITE]);
	if (dup2(fds[PIPE__READ], STDIN_FILENO) < 0
		|| dup2(fds[2], STDOUT_FILENO) < 0)
		exit(EXIT_FAILURE);
	close(fds[PIPE__READ]);
	close(fds[2]);
	execve(cmd, argv, args->envp);
}
