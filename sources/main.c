/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amassias <amassias@student.42lehavre.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/25 19:41:10 by amassias          #+#    #+#             */
/*   Updated: 2023/11/26 08:58:45 by amassias         ###   ########.fr       */
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

#define HEREDOC "here_doc"

#define ERROR_HEREDOC_INT "Internal error while reading here_doc"

#define PIPE__READ 0
#define PIPE__WRITE 1

static pid_t	g_here_doc_pid = -1;
static char		*g_pname = NULL;

static void	error_message(
				const char *pname,
				const char *message)
{
	ft_fprintf(STDERR_FILENO, "%s: %s.\n", pname, message);
}

static int	_get_input(
				int *argc,
				char ***argv)
{
	int		fd[2];
	char	*line;
	size_t	here_doc_len;
	t_list	*lines;
	t_list	*node;

	if (ft_strncmp((*argv)[1], HEREDOC, sizeof(HEREDOC)) != 0)
	{
		fd[0] = open((*argv)[1], O_RDONLY);
		if (fd[0] >= 0)
			return (fd[0]);
		perror(g_pname);
		exit(EXIT_FAILURE);
	}
	(*argc)--;
	(*argv)++;
	if (*argc < 3)
	{
		error_message(g_pname, "Missing arguments");
		// Usage ?
		exit(EXIT_FAILURE);
	}
	here_doc_len = ft_strlen((*argv)[1]);
	(*argv)[1][here_doc_len] = '\n';
	lines = NULL;
	while (1)
	{
		ft_printf("heredoc> ");
		line = get_next_line(STDIN_FILENO);
		if (line == NULL)
		{
			ft_lstclear(&lines, free);
			error_message(g_pname, ERROR_HEREDOC_INT);
			exit(EXIT_FAILURE);
		}
		if (strncmp(line, (*argv)[1], here_doc_len + 1) == 0)
		{
			free(line);
			break ;
		}
		node = ft_lstnew(line);
		if (node == NULL)
		{
			ft_lstclear(&lines, free);
			error_message(g_pname, ERROR_HEREDOC_INT);
			return (1);
		}
		ft_lstadd_back(&lines, node);
	}
	if (pipe(fd))
	{
		perror(g_pname);
		exit(EXIT_FAILURE);
	}
	g_here_doc_pid = fork();
	if (g_here_doc_pid < 0)
	{
		perror(g_pname);
		exit(EXIT_FAILURE);
	}
	if (g_here_doc_pid != 0)
	{
		close(fd[PIPE__WRITE]);
		return (fd[PIPE__READ]);
	}
	close(fd[PIPE__READ]);
	if (dup2(fd[PIPE__WRITE], STDOUT_FILENO) == -1)
	{
		close(fd[PIPE__WRITE]);
		perror(g_pname);
		exit(EXIT_FAILURE);
	}
	ft_lstiter(lines, (void (*)(void *))ft_putstr);
	close(fd[PIPE__WRITE]);
	ft_lstclear(&lines, free);
	exit(EXIT_SUCCESS);
}

int	main(
		int argc,
		char *argv[])
{
	int	input_fd;
	int	output_fd;

	g_pname = argv[0];
	if (argc < 3)
	{
		error_message(argv[0], "Missing arguments");
		// Usage ?
		return (EXIT_FAILURE);
	}

	input_fd = _get_input(&argc, &argv);
	output_fd = open(argv[argc - 1], O_WRONLY | O_CREAT);
	if (output_fd < 0)
	{
		close(input_fd);
	}

	// if (does_use_heredoc && argc < 4)
	// {
	// 	ft_fprintf(STDERR_FILENO, "%s: Missing arguments.\n", argv[0]);
	// 	// Usage ?
	// 	return (EXIT_FAILURE);
	// }
	// out_flags = O_RDWR | O_CREAT | O_SYNC;
	// if (does_use_heredoc)
	// 	out_flags |= O_APPEND;
	// out_fd = open(argv[argc - 1], out_flags, 0666);
	// if (out_fd < 0)
	// 	return (perror(argv[0]), EXIT_FAILURE);
	
	// if (pipe(pipe_fd))
	// 	return (close(out_fd), perror(argv[0]), EXIT_FAILURE);
	// close(out_fd);
	// close(pipe_fd[PIPE__READ]);
	// close(pipe_fd[PIPE__WRITE]);
	return (0);
}
