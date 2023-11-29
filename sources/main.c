/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amassias <amassias@student.42lehavre.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/25 19:41:10 by amassias          #+#    #+#             */
/*   Updated: 2023/11/29 08:31:01 by amassias         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

#include <stdbool.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#define TMP_FILE_PATH "/tmp/pipex-heredoc-tmp-file"

#define HEREDOC "here_doc"

#define ERROR_MISSING_ARG "Missing operand"
#define ERROR_INT "Internal error"
#define ERROR_HEREDOC_INT "Internal error while reading here_doc"
#define ERROR_COMMAND_NOT_FOUND "%s: command not found: %s\n"

#define BUFFER_SIZE 4096

#define RET_OK 0
#define RET_ERR 1

#define PIPE__READ 0
#define PIPE__WRITE 1

typedef struct s_args {
	int		input_fd;
	int		out_fd;
	int		null_fd;
	char	**start;
	char	**end;
	char	**paths;
	char	**envp;
}			t_args;

static char	*g_pname = NULL;

static void	_error_message(
				const char *pname,
				const char *message)
{
	ft_fprintf(STDERR_FILENO, "%s: %s.\n", pname, message);
}

static int	_cat(
				int in,
				int out)
{
	static unsigned char	buffer[BUFFER_SIZE] = {0};
	ssize_t					n;

	while (1)
	{
		n = read(in, buffer, BUFFER_SIZE);
		if (n <= 0)
			break ;
		n = write(out, buffer, (size_t) n);
		if (n < 0)
			break ;
	}
	if (n < 0)
		return (perror(g_pname), RET_ERR);
	return (RET_OK);
}

static int	_read_here_doc(
				const char *end,
				t_args *args)
{
	char	*line;
	size_t	l;

	args->input_fd = open(TMP_FILE_PATH, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (args->input_fd < 0)
		return (_error_message(g_pname, ERROR_HEREDOC_INT), RET_ERR);
	while (1)
	{
		ft_printf("heredoc> ");
		line = get_next_line(STDIN_FILENO);
		if (line == NULL)
			return (_error_message(g_pname, ERROR_HEREDOC_INT),
				close(args->input_fd), RET_ERR);
		l = ft_strlen(line);
		line[l - 1] = '\0';
		if (ft_strcmp(end, line) == 0)
			break ;
		line[l - 1] = '\n';
		(ft_putstr_fd(line, args->input_fd), free(line));
	}
	(free(line), close(args->input_fd));
	args->input_fd = open(TMP_FILE_PATH, O_RDONLY, 0666);
	if (args->input_fd < 0)
		return (RET_ERR);
	return (RET_OK);
}

static char	**_get_paths(
				char *envp[])
{
	char	*path;

	while (*envp)
	{
		if (ft_strncmp(*envp, "PATH=", 5) != 0)
		{
			++envp;
			continue ;
		}
		path = (*envp) + 5;
		return (ft_split(path, ':'));
	}
	return (NULL);
}

static char	*_get_absolute_path(
				char *command,
				char **paths)
{
	char	*res;
	size_t	command_size;
	size_t	path_size;

	command_size = ft_strlen(command);
	while (*paths)
	{
		path_size = ft_strlen(*paths);
		res = (char *) malloc(path_size + 1 + command_size + 1);
		if (res == NULL)
			return (NULL);
		ft_memcpy(res, *paths, path_size);
		ft_memcpy(res + path_size + 1, command, command_size);
		res[path_size] = '/';
		res[path_size + command_size + 1] = '\0';
		if (access(res, F_OK) == 0)
			return (res);
		free(res);
		++paths;
	}
	return (NULL);
}

static int	_parse_input(
				int argc,
				char *argv[],
				t_args *args)
{
	int	flags;

	flags = O_WRONLY | O_CREAT;
	if (argc < 3)
		return (_error_message(g_pname, ERROR_MISSING_ARG), RET_ERR);
	args->start = &argv[2];
	args->end = &argv[argc - 1];
	if (ft_strcmp(argv[1], HEREDOC) == 0)
	{
		if (argc < 4)
			return (_error_message(g_pname, ERROR_MISSING_ARG), RET_ERR);
		if (_read_here_doc(argv[2], args))
			return (RET_ERR);
		flags |= O_APPEND;
		args->start = &argv[3];
	}
	else
	{
		args->input_fd = open(argv[1], O_RDONLY);
		flags |= O_TRUNC;
	}
	if (args->input_fd < 0)
		return (perror(g_pname), RET_ERR);
	args->out_fd = open(argv[argc - 1], flags, 0666);
	if (args->out_fd < 0)
		return (perror(g_pname), close(args->input_fd), RET_ERR);
	return (RET_OK);
}

static void	_free_argv(
				char ***argv_ptr)
{
	char	**argv;

	argv = *argv_ptr;
	while (*argv)
		free(*argv++);
	free(*argv_ptr);
	*argv_ptr = NULL;
}

static int	_execute_sub_process_list(
				int in,
				t_args *args,
				int out)
{
	pid_t	command_pid;
	int		ret;
	char	*cmd;
	char	**argv;
	int		pipe_fd[2];

	--args->end;
	argv = ft_split(*args->end, ' ');
	cmd = _get_absolute_path(argv[0], args->paths);
	if (cmd == NULL)
	{
		ft_fprintf(STDERR_FILENO, ERROR_COMMAND_NOT_FOUND, g_pname, argv[0]);
		_free_argv(&argv);
		close(out);
		if (args->start != args->end)
			_execute_sub_process_list(in, args, args->null_fd);
		return (127);
	}
	pipe_fd[PIPE__WRITE] = args->null_fd;
	if (args->start == args->end)
		pipe_fd[PIPE__READ] = in;
	else if (pipe(pipe_fd))
		pipe_fd[PIPE__READ] = args->null_fd;
	command_pid = fork();
	if (command_pid == 0)
	{
		close(pipe_fd[PIPE__WRITE]);
		if (dup2(pipe_fd[PIPE__READ], STDIN_FILENO) < 0
			|| dup2(out, STDOUT_FILENO) < 0)
			exit(EXIT_FAILURE);
		close(pipe_fd[PIPE__READ]);
		close(out);
		execve(cmd, argv, args->envp);
		free(cmd);
		_free_argv(&argv);
		exit(EXIT_FAILURE);
	}
	free(cmd);
	_free_argv(&argv);
	if (command_pid < 0)
		return (_error_message(g_pname, ERROR_INT), RET_ERR);
	if (out != args->null_fd)
		close(out);
	if (pipe_fd[PIPE__READ] != args->null_fd)
		close(pipe_fd[PIPE__READ]);
	if (args->start != args->end)
		_execute_sub_process_list(in, args, pipe_fd[PIPE__WRITE]);
	waitpid(command_pid, &ret, 0);
	return (ret);
}

int	main(
		int argc,
		char *argv[],
		char *envp[])
{
	t_args	args;
	int		ret;

	g_pname = argv[0];
	if (_parse_input(argc, argv, &args) != RET_OK)
		return (EXIT_FAILURE);
	args.null_fd = open("/dev/null", O_RDWR, 0666);
	if (args.null_fd < 0)
		return (EXIT_FAILURE);
	args.envp = envp;
	args.paths = _get_paths(envp);
	if (args.start == args.end)
	{
		if (_cat(args.input_fd, args.out_fd) != RET_OK)
			return (EXIT_FAILURE);
		return (EXIT_SUCCESS);
	}
	ret = _execute_sub_process_list(args.input_fd, &args, args.out_fd);
	close(args.null_fd);
	_free_argv(&args.paths);
	return (ret);
}
