/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   small_helpers.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 12:44:02 by omizin            #+#    #+#             */
/*   Updated: 2025/07/31 10:29:52 by omizin           ###   ########.fr       */
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

int	last_status_assign(t_input *command, t_shell *shell)
{
	int	last_status;

	last_status = 0;
	if (command->args[1])
	{
		last_status = ft_atoi(command->args[1]);
		if (ft_isalpha(command->args[1][0]))
		{
			printf("exit: %s: numeric argument required\n", command->args[1]);
			last_status = 2;
		}
		else
			printf("exit\n");
	}
	else
		last_status = shell->last_exit_status;
	return (last_status);
}
