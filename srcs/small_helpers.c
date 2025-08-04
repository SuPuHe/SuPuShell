/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   small_helpers.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 12:44:02 by omizin            #+#    #+#             */
/*   Updated: 2025/08/04 11:26:30 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Checks if a character is a whitespace character.
 *
 * Returns true if the character is a space, tab, newline, vertical tab,
 * form feed, or carriage return.
 *
 * @param c Character to check.
 * @return Nonzero if whitespace, zero otherwise.
 */
int	ft_isspace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n'
		|| c == '\v' || c == '\f' || c == '\r');
}

/**
 * @brief Returns the length of a string array.
 *
 * Counts the number of non-NULL strings in the array.
 *
 * @param arr Array of strings.
 * @return Number of strings in the array.
 */
static int	ft_arrlen(char **arr)
{
	int	i;

	i = 0;
	if (!arr)
		return (0);
	while (arr[i])
		i++;
	return (i);
}

/**
 * @brief Appends a new argument to a string array.
 *
 * Allocates a new array, copies existing arguments, adds the new argument,
 * and returns the new array.
 *
 * @param args Array of arguments.
 * @param new_arg Argument to append.
 * @return Pointer to new array, or NULL on error.
 */
char	**append_arg(char **args, char *new_arg)
{
	int		len;
	char	**new_args ;
	int		i;

	len = ft_arrlen(args);
	new_args = cf_malloc(sizeof(char *) * (len + 2));
	i = 0;
	if (!new_args)
		return (NULL);
	while (i < len)
	{
		new_args[i] = args[i];
		i++;
	}
	new_args[len] = cf_strdup(new_arg);
	new_args[len + 1] = NULL;
	return (new_args);
}

/**
 * @brief Determines the exit status for the shell's exit command.
 *
 * Parses the argument for exit, checks if it is numeric, prints error
 * if not, and returns the appropriate exit status. If no argument is
 * provided, returns the last exit status from the shell.
 *
 * @param command Pointer to input command structure.
 * @param shell Pointer to shell structure.
 * @return Exit status value.
 */
int	ft_isnumeric(const char *str)
{
	int	i;

	i = 0;
	if (!str || !str[0])
		return (0);
	if (str[0] == '-' || str[0] == '+')
		i++;
	if (!str[i])
		return (0);
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

int	last_status_assign(t_input *command, t_shell *shell)
{
	int	last_status;

	// ft_putstr_fd("exit\n", 2);
	if (command->args[1])
	{
		if (!ft_isnumeric(command->args[1]))
		{
			ft_putstr_fd("Billyshell: exit: numeric argument required\n", 2);
			return (2);
		}
		last_status = ft_atoi(command->args[1]);
		if (command->args[2])
		{
			ft_putstr_fd("Billyshell: exit: too many arguments\n", 2);
			return (1);
		}
		return ((unsigned char)last_status);
	}
	return (shell->last_exit_status);
}
