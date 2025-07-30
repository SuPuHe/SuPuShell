/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 11:39:33 by omizin            #+#    #+#             */
/*   Updated: 2025/07/30 18:22:08 by vpushkar         ###   ########.fr       */
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
	printf(BOLDGREEN"%s\n"RESET, buf);
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
	t_env	*cur;

	cur = env;
	while (cur)
	{
		printf("declare -x %s", cur->key);
		if (cur->value)
			printf("=\"%s\"", cur->value);
		printf("\n");
		cur = cur->next;
	}
}

/**
 * @brief Handles the export command for environment variables.
 *
 * Prints all variables if no arguments. Otherwise, parses and updates
 * each argument as an environment variable.
 *
 * @param argv Array of export arguments.
 * @param env Pointer to environment variable list.
 */
void	do_export(char **argv, t_env **env)
{
	int		i;
	char	*key;
	char	*val;
	char	*tmp;

	if (!argv[1])
	{
		print_all_env_vars(*env);
		return ;
	}
	i = 1;
	while (argv[i])
	{
		tmp = cf_strdup(argv[i]);
		if (!tmp)
			return ;
		if (!parse_export_argument(tmp, &key, &val))
		{
			printf(BOLDRED"export: invalid format: %s\n"RESET, argv[i]);
			i++;
			continue ;
		}
		update_or_add_env_var(env, key, val);
		i++;
	}
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
