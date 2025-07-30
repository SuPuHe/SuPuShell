/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_main_second_part.c                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:48:36 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/30 13:33:03 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Helper function to add end token
void	add_end_token(t_list **tokens)
{
	t_token	*end_token;
	t_list	*end_node;

	end_token = create_token(TOKEN_END, NULL);
	end_token->has_space = false;
	end_node = cf_lstnew(end_token);
	ft_lstadd_back(tokens, end_node);
}
