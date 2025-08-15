/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands_3.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 16:52:22 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/15 17:52:41 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Processes a single argument for the export built-in.
 *
 * Duplicates the argument string, parses it into key and value if it
 * contains '=', then validates the key. Updates or adds the variable
 * to the environment if valid, otherwise prints an error.
 *
 * @param arg The argument string to process.
 * @param env Pointer to the environment linked list.
 *
 * @return true if the variable was valid and added/updated, false
 * if invalid or on memory allocation failure.
 */
static bool	process_export_arg(char *arg, t_env **env)
{
	char	*key;
	char	*val;
	char	*tmp;
	bool	valid;

	valid = true;
	tmp = cf_strdup(arg);
	if (!tmp)
		return (perror("Billyshell: export: memory allocation error"), false);
	if (parse_export_argument(tmp, &key, &val))
		valid = handle_parsed_export_arg(key, val, env);
	else
	{
		if (is_valid_var_name(arg))
			update_or_add_env_var(env, arg, "");
		else
		{
			ft_putstr_fd("Billyshell: export: not a valid identifier\n", 2);
			valid = false;
		}
	}
	cf_free_one(tmp);
	return (valid);
}

/**
 * @brief Handles the 'export' command in the minishell.
 *
 * This function provides behavior consistent with a standard shell's 'export'
 * command, handling argument parsing, variable validation, updating the
 * environment, and setting the correct exit status. It also ensures proper
 * memory management.
 *
 * @param shell Pointer to the shell structure (for last_exit_status).
 * @param argv Array of command-line arguments for 'export'.
 * @param env Pointer to the head of the environment variables linked list.
 */
void	do_export(t_shell *shell, char **argv, t_env **env)
{
	int		i;
	bool	any_invalid_arg;

	any_invalid_arg = false;
	if (!argv[1])
	{
		print_all_env_vars(*env);
		shell->last_exit_status = 0;
		return ;
	}
	i = 1;
	while (argv[i])
	{
		if (!process_export_arg(argv[i], env))
			any_invalid_arg = true;
		i++;
	}
	if (any_invalid_arg)
		shell->last_exit_status = 1;
	else
		shell->last_exit_status = 0;
}

/**
 * @brief Handles the unset command for environment variables.
 *
 * Removes each specified variable from the environment list.
 *
 * @param argv Array of variable names to unset.
 * @param env Pointer to environment variable list.
 */
void	do_unset(char **argv, t_env **env)
{
	int	i;

	i = 1;
	while (argv[i])
	{
		remove_env_var(env, argv[i]);
		i++;
	}
}
