/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_checker.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 15:02:06 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/30 18:31:14 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Handles quote characters in input for syntax checking.
 *
 * Updates quote state and counters for single and double quotes.
 *
 * @param c Current character.
 * @param check Pointer to input check structure.
 * @param in_squote Pointer to single quote state.
 * @param in_dquote Pointer to double quote state.
 */
void	handle_quote_char(char c,
	t_input_check *check, bool *in_squote, bool *in_dquote)
{
	if (c == '"' && !(*in_squote))
	{
		*in_dquote = !(*in_dquote);
		check->d_quot++;
	}
	else if (c == '\'' && !(*in_dquote))
	{
		*in_squote = !(*in_squote);
		check->s_quot++;
	}
}

/**
 * @brief Handles non-quote syntax characters in input.
 *
 * Checks for unsupported characters and updates parentheses counters.
 *
 * @param c Current character.
 * @param check Pointer to input check structure.
 * @param in_squote Single quote state.
 * @param in_dquote Double quote state.
 * @return true if syntax is valid, false otherwise.
 */
bool	handle_non_quote_syntax(char c,
	t_input_check *check, bool in_squote, bool in_dquote)
{
	if (!in_squote && !in_dquote)
	{
		if (c == '\\' || c == ';')
			return (printf("Error input: Unsupported character '\\' or ';'\n"),
				false);
		else if (c == '(')
			check->l_par++;
		else if (c == ')')
			check->r_par++;
	}
	return (true);
}

/**
 * @brief Processes a line for syntax errors.
 *
 * Iterates through the line, updating quote and parentheses state,
 * and checks for syntax errors.
 *
 * @param line Input line to check.
 * @param check Pointer to input check structure.
 * @param in_squote Pointer to single quote state.
 * @param in_dquote Pointer to double quote state.
 * @return true if no errors, false otherwise.
 */
bool	process_line_for_syntax_errors(const char *line,
	t_input_check *check, bool *in_squote, bool *in_dquote)
{
	int	i;

	i = 0;
	while (line[i])
	{
		handle_quote_char(line[i], check, in_squote, in_dquote);
		if (!handle_non_quote_syntax(line[i], check, *in_squote, *in_dquote))
			return (false);
		i++;
	}
	return (true);
}

/**
 * @brief Main function to check for input errors.
 *
 * Checks for unsupported characters, unclosed quotes, and unbalanced
 * parentheses in the input line.
 *
 * @param line Input line to check.
 * @return 1 if input is valid, 0 if errors found.
 */
int	check_for_input(char *line)
{
	t_input_check	check;
	bool			in_squote;
	bool			in_dquote;

	check.d_quot = 0;
	check.s_quot = 0;
	check.l_par = 0;
	check.r_par = 0;
	in_squote = false;
	in_dquote = false;
	if (!process_line_for_syntax_errors(line, &check, &in_squote, &in_dquote))
		return (0);
	if ((check.d_quot % 2 != 0 || check.s_quot % 2 != 0))
		return (printf("Error input: Unclosed quotes\n"), 0);
	if (check.r_par != check.l_par)
		return (printf("Error input: Unbalanced parentheses\n"), 0);
	return (1);
}
