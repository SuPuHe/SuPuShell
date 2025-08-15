/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands_2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 12:30:27 by omizin            #+#    #+#             */
/*   Updated: 2025/08/15 16:58:56 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Prints an error message for a command to stderr in red color.
 *
 * Writes the command name in red to stderr using write().
 *
 * @param argv Command name to print.
 */
void	print_error(char *argv)
{
	int	i;

	i = 0;
	write(2, BOLDRED, 9);
	while (argv[i] != '\0')
	{
		write(2, &argv[i], 1);
		i++;
	}
}

int	return_exit_code(char *path)
{
	if (!path)
	{
		write(2, ": command not found\n"RESET, 25);
		return (127);
	}
	else if (access(path, F_OK) == 0 && access(path, X_OK) != 0)
	{
		write(2, ": permission denied\n"RESET, 25);
		return (126);
	}
	else
	{
		write(2, ": command not found\n"RESET, 25);
		return (127);
	}
}

/**
 * @brief Executes an external command using execve.
 *
 * Searches for the command in PATH, builds environment, and calls execve.
 * Handles errors and exits with appropriate code if execution fails.
 *
 * @param argv Array of command arguments.
 * @param env Pointer to environment variable list.
 */
void	run_external_command(char **argv, t_env *env)
{
	char	*path;
	char	**envp;
	char	*cur_dir;

	envp = build_envp(env);
	cur_dir = get_env_value(env, "PWD");
	if (ft_strcmp(cur_dir, "/usr/bin") == 0)
		path = argv[0];
	else if (ft_strncmp(argv[0], "./", 2) == 0
		|| ft_strncmp(argv[0], "/", 1) == 0)
		path = argv[0];
	else
		path = search_path(argv[0], env);
	if (!path || access(path, X_OK) != 0)
	{
		print_error(argv[0]);
		rl_clear_history();
		cf_free_all();
		exit(return_exit_code(path));
	}
	execve(path, argv, envp);
	perror("execve");
	rl_clear_history();
	cf_free_all();
	exit(127);
}

/**
 * @brief Implements the echo command with -n option support.
 *
 * Prints arguments separated by spaces. Handles multiple -n options
 * to suppress the trailing newline.
 *
 * @param argv Array of arguments to echo.
 */
void	do_echo(char **argv)
{
	int	i;
	int	j;
	int	new_line_flag;

	i = 1;
	new_line_flag = 1;
	while (argv[i] && argv[i][0] == '-' && argv[i][1] == 'n')
	{
		j = 2;
		while (argv[i][j] == 'n')
			j++;
		if (argv[i][j] != '\0')
			break ;
		new_line_flag = 0;
		i++;
	}
	while (argv[i])
	{
		printf("%s", argv[i]);
		if (argv[i + 1])
			printf(" ");
		i++;
	}
	if (new_line_flag)
		printf("\n");
}

/**
 * @brief Prints all environment variables in KEY=VALUE format.
 *
 * Iterates through the environment list and prints each variable
 * with its value. If arguments are provided, returns error.
 *
 * @param env Pointer to environment variable list.
 * @param argv Array of command arguments.
 */
void	do_env(t_env *env, char **argv)
{
	if (argv[1])
	{
		print_error(argv[1]);
		write(2, ": No such file or directory\n"RESET, 30);
		return ;
	}
	while (env)
	{
		if (env->value)
			printf("%s=%s\n", env->key, env->value);
		env = env->next;
	}
}
