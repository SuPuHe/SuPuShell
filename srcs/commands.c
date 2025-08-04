/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 11:39:33 by omizin            #+#    #+#             */
/*   Updated: 2025/08/04 13:08:49 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Prints the current working directory.
 *
 * Uses getcwd to get the current directory and prints it in green color.
 */
void	do_pwd(void)
{
	char	buf[512];

	getcwd(buf, sizeof(buf));
	printf("%s\n", buf);
}

/**
 * @brief Changes the current directory and updates environment variables.
 *
 * Handles cd with no arguments, '~', '-', and a specific path. Updates
 * OLDPWD and PWD environment variables after changing directory.
 *
 * @param commands Array of command arguments.
 * @param env Pointer to environment variable list.
 */
void	do_cd(char **commands, t_env **env)
{
	const char	*home = getenv("HOME");
	char		newpwd[512];
	char		oldpwd[512];

	if (!getcwd(oldpwd, sizeof(oldpwd)))
		return ;
	if (!commands[1])
		chdir(home);
	else if (ft_strncmp(commands[1], "~", 2) == 0)
		chdir(home);
	else if (ft_strncmp(commands[1], "-", 2) == 0)
		goto_prev_dir(env);
	else
	{
		if (chdir(commands[1]) == -1)
			perror(BOLDRED"cd"RESET);
	}
	if (getcwd(newpwd, sizeof(newpwd)))
	{
		update_or_add_env_var(env, "OLDPWD", oldpwd);
		update_or_add_env_var(env, "PWD", newpwd);
	}
}

/**
 * @brief Prints all environment variables in export format.
 *
 * Iterates through the environment list and prints each variable in
 * 'declare -x KEY="VALUE"' format.
 *
 * @param env Pointer to environment variable list.
 */
static void	print_all_env_vars(t_env *env)
{
	t_env	**env_array;
	int		count;
	int		i;

	env_array = create_sorted_env_array(env, &count);
	if (!env_array)
		return ;
	i = 0;
	while (i < count)
	{
		printf("declare -x %s", env_array[i]->key);
		if (env_array[i]->value && env_array[i]->value[0])
			printf("=\"%s\"", env_array[i]->value);
		printf("\n");
		i++;
	}
	cf_free_one(env_array);
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
	char	*key;
	char	*val;
	char	*tmp;
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
		tmp = cf_strdup(argv[i]);
		if (!tmp)
		{
			perror("minishell: export: memory allocation error");
			shell->last_exit_status = 1;
			return ;
		}
		if (parse_export_argument(tmp, &key, &val))
		{
			if (is_valid_var_name(key))
				update_or_add_env_var(env, key, val);
			else
			{
				ft_putstr_fd("Billyshell: export: not a valid identifier\n", 2);
				any_invalid_arg = true;
			}
		}
		else
		{
			if (is_valid_var_name(argv[i]))
			{
				update_or_add_env_var(env, argv[i], "");
			}
			else
			{
				ft_putstr_fd("Billyshell: export: not a valid identifier\n", 2);
				any_invalid_arg = true;
			}
		}
		cf_free_one(tmp);
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
