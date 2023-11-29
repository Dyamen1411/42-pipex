/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amassias <amassias@student.42lehavre.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/29 10:26:58 by amassias          #+#    #+#             */
/*   Updated: 2023/11/29 13:42:10 by amassias         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file utils.h
 * @author Antoine Massias (amassias@student.42lehavre.fr)
 * @date 2023-11-29
 * @copyright Copyright (c) 2023
 */

#ifndef UTILS_H
# define UTILS_H

/* ************************************************************************** */
/*                                                                            */
/* Defines                                                                    */
/*                                                                            */
/* ************************************************************************** */

/**
 * @brief The path to the temporary file that holds the `here_doc` input.
 */
# define TMP_FILE_PATH "/tmp/pipex-heredoc-tmp-file"

/**
 * @brief The token that indicates to `pipex` that it should use here_doc.
 */
# define HEREDOC "here_doc"

/**
 * @brief Error string for when an operand is missing.
 */
# define ERROR_MISSING_ARG "Missing operand"

/**
 * @brief 
 */
# define ERROR_INT "Internal error"
# define ERROR_HEREDOC_INT "Internal error while reading here_doc"
# define ERROR_COMMAND_NOT_FOUND "%s: command not found: %s\n"

# define BUFFER_SIZE 4096

# define RET_OK 0
# define RET_ERR 1

# define PIPE__READ 0
# define PIPE__WRITE 1

/* ************************************************************************** */
/*                                                                            */
/* Structures                                                                 */
/*                                                                            */
/* ************************************************************************** */

struct s_args {
	int		input_fd;
	int		input_flags;
	int		out_fd;
	int		null_fd;
	char	**start;
	char	**end;
	char	**paths;
	char	**envp;
};

/* ************************************************************************** */
/*                                                                            */
/* Types                                                                      */
/*                                                                            */
/* ************************************************************************** */

typedef struct s_args	t_args;

/* ************************************************************************** */
/*                                                                            */
/* Global variables                                                           */
/*                                                                            */
/* ************************************************************************** */

extern char				*g_pname;

/* ************************************************************************** */
/*                                                                            */
/* Utilitaries                                                                */
/*                                                                            */
/* ************************************************************************** */

/**
 * @brief 
 * @param string_list_ptr 
 * @author Antoine Massias (amassias@student.42lehavre.fr)
 * @date 2023-11-29
 * @todo Make documentation.
 */
void	free_string_list(
			char ***string_list_ptr);

/**
 * @brief 
 * @param envp 
 * @author Antoine Massias (amassias@student.42lehavre.fr)
 * @date 2023-11-29
 * @todo Make documentation.
 */
char	**get_paths(
			char *envp[]);

/**
 * @brief 
 * @param pname 
 * @param message 
 * @author Antoine Massias (amassias@student.42lehavre.fr)
 * @date 2023-11-29
 * @todo Make documentation.
 */
void	error_message(
			const char *pname,
			const char *message);

/**
 * @brief 
 * @param argc 
 * @param argv 
 * @param args 
 * @author Antoine Massias (amassias@student.42lehavre.fr)
 * @date 2023-11-29
 * @todo Make documentation.
 */
int		parse_input(
			int argc,
			char *argv[],
			t_args *args);

/**
 * @brief 
 * @param args 
 * @param out 
 * @return int 
 * @author Antoine Massias (amassias@student.42lehavre.fr)
 * @date 2023-11-29
 * @todo Make documentation.
 */
int		execute_sub_process_list(
			t_args *args,
			int out);

/**
 * @brief 
 * @param end 
 * @param args 
 * @return int 
 * @author Antoine Massias (amassias@student.42lehavre.fr)
 * @date 2023-11-29
 * @todo Make documentation.
 */
int		read_here_doc(
			const char *end,
			t_args *args);

/**
 * @brief 
 * @param command 
 * @param paths 
 * @return char* 
 * @author Antoine Massias (amassias@student.42lehavre.fr)
 * @date 2023-11-29
 * @todo Make documentation.
 */
char	*get_absolute_path(
			char *command,
			char **paths);

#endif