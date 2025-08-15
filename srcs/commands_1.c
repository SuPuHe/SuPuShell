/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands_1.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 11:39:33 by omizin            #+#    #+#             */
/*   Updated: 2025/08/15 16:55:39 by vpushkar         ###   ########.fr       */
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

void	cd_logic(t_shell *shell, char **commands, t_env **env,
	const char *home)
{
	if (!commands[1] || ft_strncmp(commands[1], "~", 2) == 0)
		chdir(home);
	else if (ft_strncmp(commands[1], "-", 2) == 0)
		goto_prev_dir(env);
	else
	{
		if (chdir(commands[1]) == -1)
		{
			perror(BOLDRED"cd"RESET);
			shell->last_exit_status = 1;
		}
	}
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
void	do_cd(t_shell *shell, char **commands, t_env **env)
{
	const char	*home;
	char		oldpwd[512];
	char		newpwd[512];

	if (commands[2])
	{
		ft_putstr_fd("Billyshell: cd: too many arguments\n", 2);
		shell->last_exit_status = 1;
		return ;
	}
	if (!getcwd(oldpwd, sizeof(oldpwd)))
		return ;
	home = getenv("HOME");
	cd_logic(shell, commands, env, home);
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
void	print_all_env_vars(t_env *env)
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

bool	handle_parsed_export_arg(char *key, char *val, t_env **env)
{
	if (is_valid_var_name(key))
	{
		update_or_add_env_var(env, key, val);
		return (true);
	}
	else
	{
		ft_putstr_fd("Billyshell: export: not a valid identifier\n", 2);
		return (false);
	}
}
