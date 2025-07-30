/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_checker.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 17:26:46 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/30 17:33:23 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INPUT_CHECKER_H
# define INPUT_CHECKER_H

# include "types.h"
# include <stdbool.h>

void	handle_quote_char(char c,
			t_input_check *check, bool *in_squote, bool *in_dquote);
bool	handle_non_quote_syntax(char c,
			t_input_check *check, bool in_squote, bool in_dquote);
bool	process_line_for_syntax_errors(const char *line,
			t_input_check *check, bool *in_squote, bool *in_dquote);
int		check_for_input(char *line);

#endif
