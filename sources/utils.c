/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amassias <amassias@student.42lehavre.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/29 10:26:04 by amassias          #+#    #+#             */
/*   Updated: 2023/11/29 13:45:49 by amassias         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file utils.c
 * @author Antoine Massias (amassias@student.42lehavre.fr)
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

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* ************************************************************************** */
/*                                                                            */
/* Header implementations                                                     */
/*                                                                            */
/* ************************************************************************** */

void	free_string_list(
			char ***argv_ptr)
{
	char	**argv;

	argv = *argv_ptr;
	if (*argv == NULL)
		return ;
	while (*argv)
		free(*argv++);
	free(*argv_ptr);
	*argv_ptr = NULL;
}

char	**get_paths(
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

void	error_message(
			const char *pname,
			const char *message)
{
	ft_fprintf(STDERR_FILENO, "%s: %s.\n", pname, message);
}

int	parse_input(
		int argc,
		char *argv[],
		t_args *args)
{
	args->input_flags = O_WRONLY | O_CREAT;
	if (argc < 3)
		return (error_message(g_pname, ERROR_MISSING_ARG), RET_ERR);
	args->start = &argv[2];
	args->end = &argv[argc - 1];
	if (ft_strcmp(argv[1], HEREDOC) == 0)
	{
		if (argc < 4)
			return (error_message(g_pname, ERROR_MISSING_ARG), RET_ERR);
		if (read_here_doc(argv[2], args))
			return (RET_ERR);
		args->input_flags |= O_APPEND;
		args->start = &argv[3];
	}
	else
	{
		args->input_fd = open(argv[1], O_RDONLY);
		args->input_flags |= O_TRUNC;
	}
	if (args->input_fd < 0)
		return (perror(g_pname), RET_ERR);
	args->out_fd = open(argv[argc - 1], args->input_flags, 0666);
	if (args->out_fd < 0)
		return (perror(g_pname), close(args->input_fd), RET_ERR);
	return (RET_OK);
}
