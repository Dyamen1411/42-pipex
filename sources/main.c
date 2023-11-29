/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amassias <amassias@student.42lehavre.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/25 19:41:10 by amassias          #+#    #+#             */
/*   Updated: 2023/11/29 13:44:48 by amassias         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file main.c
 * @author Antoine Massias (amassias@student.42lehavre.fr)
 * @date 2023-11-29
 * @copyright Copyright (c) 2023
 */

/* ************************************************************************** */
/*                                                                            */
/* Includes                                                                   */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

#include "utils.h"

#include <fcntl.h>
#include <stdio.h>

/* ************************************************************************** */
/*                                                                            */
/* Global variables                                                           */
/*                                                                            */
/* ************************************************************************** */

char	*g_pname = NULL;

/* ************************************************************************** */
/*                                                                            */
/* Helper prototypes                                                          */
/*                                                                            */
/* ************************************************************************** */

/**
 * @brief 
 * @param in 
 * @param out 
 * @return int 
 * @author Antoine Massias (amassias@student.42lehavre.fr)
 * @date 2023-11-29
 * @todo Make documentation.
 */
static int	_cat(
				int in,
				int out);

/* ************************************************************************** */
/*                                                                            */
/* Main                                                                       */
/*                                                                            */
/* ************************************************************************** */

int	main(
		int argc,
		char *argv[],
		char *envp[])
{
	t_args	args;
	int		ret;

	g_pname = argv[0];
	args.envp = envp;
	args.null_fd = open("/dev/null", O_RDWR, 0666);
	if (args.null_fd < 0)
		return (EXIT_FAILURE);
	if (parse_input(argc, argv, &args) != RET_OK)
		return (EXIT_FAILURE);
	args.paths = get_paths(envp);
	if (args.start == args.end)
	{
		if (_cat(args.input_fd, args.out_fd) != RET_OK)
			return (EXIT_FAILURE);
		return (EXIT_SUCCESS);
	}
	ret = execute_sub_process_list(&args, args.out_fd);
	close(args.null_fd);
	free_string_list(&args.paths);
	return (ret);
}

/* ************************************************************************** */
/*                                                                            */
/* Header implementations                                                     */
/*                                                                            */
/* ************************************************************************** */

int	read_here_doc(
		const char *end,
		t_args *args)
{
	char	*line;
	size_t	l;

	args->input_fd = open(TMP_FILE_PATH, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (args->input_fd < 0)
		return (error_message(g_pname, ERROR_HEREDOC_INT), RET_ERR);
	while (1)
	{
		ft_printf("heredoc> ");
		line = get_next_line(STDIN_FILENO);
		if (line == NULL)
			return (error_message(g_pname, ERROR_HEREDOC_INT),
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

char	*get_absolute_path(
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

/* ************************************************************************** */
/*                                                                            */
/* Helper definitions                                                         */
/*                                                                            */
/* ************************************************************************** */

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
