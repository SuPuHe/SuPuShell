/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_word_1.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:15:20 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/18 18:01:01 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool is_token_quoted(t_token *token) {
	if (!token)
		return (false);
	return (token->type == TOKEN_SINGLE_QUOTE_WORD ||
			token->type == TOKEN_DOUBLE_QUOTE_WORD);
}

t_token_part *create_token_parts_list(t_list *tokens_start, t_list *tokens_end) {
	t_token_part	*head = NULL;
	t_token_part	*current_part = NULL;
	t_list			*token_node = tokens_start;
	int				pos = 0;

	while (token_node && token_node != tokens_end) {
		t_token *token = (t_token *)token_node->content;
		t_token_part *new_part = malloc(sizeof(t_token_part));

		if (!new_part) {
			free_token_parts(head);
			return NULL;
		}

		new_part->start_pos = pos;
		new_part->end_pos = pos + strlen(token->value) - 1;
		new_part->is_quoted = is_token_quoted(token);
		new_part->next = NULL;

		if (!head) {
			head = new_part;
			current_part = new_part;
		}
		else
		{
			current_part->next = new_part;
			current_part = new_part;
		}

		pos += strlen(token->value);
		token_node = token_node->next;
	}

	return (head);
}

void free_token_parts(t_token_part *parts) {
	while (parts) {
		t_token_part *next = parts->next;
		free(parts);
		parts = next;
	}
}

bool has_unquoted_wildcards(char *str, t_token_part *parts) {
	int len = strlen(str);

	for (int i = 0; i < len; i++) {
		if (str[i] == '*' || str[i] == '?') {
			// Проверяем, находится ли этот символ в закавыченной части
			t_token_part *current = parts;
			bool in_quoted_part = false;

			while (current) {
				if (i >= current->start_pos && i <= current->end_pos && current->is_quoted) {
					in_quoted_part = true;
					break ;
				}
				current = current->next;
			}

			if (!in_quoted_part) {
				return (true); // Нашли незакавыченный wildcard
			}
		}
	}

	return false;
}

char *create_bash_compatible_pattern(char *str, t_token_part *parts) {
	int		len = strlen(str);
	char	*pattern = malloc(len * 2 + 1); // Запас на экранирование
	if (!pattern)
		return NULL;

	int pattern_pos = 0;

	for (int i = 0; i < len; i++) {
		char c = str[i];

		// Проверяем, находится ли символ в закавыченной части
		t_token_part *current = parts;
		bool in_quoted_part = false;

		while (current)
		{
			if (i >= current->start_pos && i <= current->end_pos && current->is_quoted) {
				in_quoted_part = true;
				break ;
			}
			current = current->next;
		}

		// Если wildcard символ в закавыченной части - экранируем его
		if ((c == '*' || c == '?') && in_quoted_part)
		{
			pattern[pattern_pos++] = '\\';
		}

		pattern[pattern_pos++] = c;
	}

	pattern[pattern_pos] = '\0';
	return (pattern);
}

int matches_pattern_with_escape(const char *str, const char *pattern) {
	if (*pattern == '\0')
		return (*str == '\0');

	// Обработка экранированных символов
	if (*pattern == '\\' && *(pattern + 1) != '\0') {
		if (*str != *(pattern + 1))
			return (0);
		return (matches_pattern_with_escape(str + 1, pattern + 2));
	}

	if (*pattern == '*') {
		while (*pattern == '*')
			pattern++;
		if (*pattern == '\0')
			return (1);
		while (*str) {
			if (matches_pattern_with_escape(str, pattern))
				return (1);
			str++;
		}
		return (matches_pattern_with_escape(str, pattern));
	}

	if (*str == '\0')
		return 0;

	if (*pattern == *str || *pattern == '?')
		return matches_pattern_with_escape(str + 1, pattern + 1);

	return 0;
}

char **expand_wildcards_with_escape(const char *pattern) {
	DIR				*dir;
	struct dirent	*entry;
	char			**result;
	int				i = 0;
	int				count;

	if (!ft_strchr(pattern, '*') && !ft_strchr(pattern, '?')) {
		result = malloc(sizeof(char*) * 2);
		if (!result) return NULL;
		result[0] = strdup(pattern);
		result[1] = NULL;
		return result;
	}

	// Считаем совпадения
	count = 0;
	dir = opendir(".");
	if (!dir) return NULL;

	while ((entry = readdir(dir)) != NULL) {
		if (entry->d_name[0] != '.' && matches_pattern_with_escape(entry->d_name, pattern)) {
			count++;
		}
	}
	closedir(dir);

	if (count == 0) {
		// Нет совпадений - возвращаем оригинальный паттерн
		result = malloc(sizeof(char*) * 2);
		if (!result) return NULL;
		result[0] = strdup(pattern);
		result[1] = NULL;
		return result;
	}

	result = malloc(sizeof(char*) * (count + 1));
	if (!result) return NULL;

	dir = opendir(".");
	if (!dir) {
		free(result);
		return NULL;
	}

	while ((entry = readdir(dir)) != NULL && i < count) {
		if (entry->d_name[0] != '.' && matches_pattern_with_escape(entry->d_name, pattern)) {
			result[i] = strdup(entry->d_name);
			if (!result[i]) {
				// Cleanup on error
				while (--i >= 0) free(result[i]);
				free(result);
				closedir(dir);
				return NULL;
			}
			i++;
		}
	}

	result[i] = NULL;
	closedir(dir);
	return result;
}

void handle_bash_compatible_wildcard_expansion(t_input *input, char *expanded_value,
											  t_list *tokens_start, t_list *tokens_end) {
	char **wildcards;
	char *bash_pattern;
	t_token_part *parts;
	int i;

	// Создаем список частей с информацией о кавычках
	parts = create_token_parts_list(tokens_start, tokens_end);
	if (!parts) {
		input->args = append_arg(input->args, expanded_value);
		cf_free_one(expanded_value);
		return;
	}

	// Проверяем, есть ли незакавыченные wildcard символы
	if (!has_unquoted_wildcards(expanded_value, parts)) {
		// Нет незакавыченных wildcard - обрабатываем как обычное слово
		input->args = append_arg(input->args, expanded_value);
		cf_free_one(expanded_value);
		free_token_parts(parts);
		return;
	}

	// Создаем bash-совместимый паттерн (экранируем wildcard в кавычках)
	bash_pattern = create_bash_compatible_pattern(expanded_value, parts);
	if (!bash_pattern) {
		input->args = append_arg(input->args, expanded_value);
		cf_free_one(expanded_value);
		free_token_parts(parts);
		return;
	}

	// Применяем wildcard расширение с экранированным паттерном
	wildcards = expand_wildcards_with_escape(bash_pattern);
	if (wildcards && wildcards[0]) {
		i = 0;
		while (wildcards[i]) {
			input->args = append_arg(input->args, wildcards[i]);
			i++;
		}
		free_expanded_wildcards(wildcards);
	} else {
		input->args = append_arg(input->args, expanded_value);
	}

	cf_free_one(expanded_value);
	cf_free_one(bash_pattern);
	free_token_parts(parts);
}
/**
 * @brief Handles fallback for heredoc if no arguments are present.
 *
 * Sets a default argument ':' if heredoc is used without any command
 * arguments, ensuring valid command execution.
 *
 * @param input Pointer to the input structure.
 */
void	handle_heredoc_fallback(t_input *input)
{
	if ((!input->args || !input->args[0]) && input->heredoc_count > 0)
	{
		input->args = cf_malloc(sizeof(char *) * 2);
		if (!input->args)
		{
			input->syntax_ok = false;
			return ;
		}
		input->args[0] = cf_strdup(":");
		input->args[1] = NULL;
	}
}

/**
 * @brief Handles a regular word token, adding it to command arguments.
 *
 * Adds the expanded word to the input arguments if not empty.
 * Frees the expanded value after use.
 *
 * @param input Pointer to the input structure.
 * @param expanded_value Expanded word string.
 */
void	handle_regular_word(t_input *input, char *expanded_value)
{
	if (ft_strlen(expanded_value) > 0)
	{
		input->args = append_arg(input->args, expanded_value);
		cf_free_one(expanded_value);
	}
	else if (expanded_value)
	{
		input->args = append_arg(input->args, cf_strdup(""));
		cf_free_one(expanded_value);
	}
	else
		cf_free_one(expanded_value);
}

/**
 * @brief Handles wildcard expansion for word tokens.
 *
 * Expands wildcards in the word and adds all matches to command arguments.
 * If no matches, adds the original word. Frees memory after use.
 *
 * @param input Pointer to the input structure.
 * @param current_tok Pointer to the current token.
 * @param expanded_value Expanded word string.
 */
void handle_wildcard_expansion(t_input *input, t_token *current_tok, char *expanded_value) {
	char	**wildcards;
	int		i;

	if (current_tok->type == TOKEN_WORD &&
		(ft_strchr(expanded_value, '*') || ft_strchr(expanded_value, '?'))) {

		wildcards = expand_wildcards(expanded_value);
		if (wildcards && wildcards[0]) {
			i = 0;
			while (wildcards[i]) {
				input->args = append_arg(input->args, wildcards[i]);
				i++;
			}
			free_expanded_wildcards(wildcards);
		}
		else
		{
			input->args = append_arg(input->args, expanded_value);
		}
		cf_free_one(expanded_value);
	}
	else
	{
		handle_regular_word(input, expanded_value);
	}
}

/**
 * @brief Concatenates adjacent word tokens into a single string.
 *
 * Combines adjacent tokens without spaces, expanding each and joining
 * them into one argument string for the command.
 *
 * @param current_tokens Pointer to the token list pointer.
 * @param expanded_value Initial expanded value string.
 * @param env Pointer to the environment structure.
 * @param shell Pointer to the shell structure.
 * @return Combined expanded string (must be freed).
 */
char *concatenate_adjacent_tokens(t_list **current_tokens, char *expanded_value,
				 t_env *env, t_shell *shell) {
	t_list	*next_token;
	char	*next_expanded;
	char	*combined;

	next_token = (*current_tokens)->next;
	while (next_token && !((t_token *)next_token->content)->has_space &&
			is_adjacent_word_token((t_token *)next_token->content)) {

		next_expanded = expand_token_value((t_token *)next_token->content, env, shell);

		if (next_expanded) {
			combined = ft_strjoin_free(expanded_value, next_expanded);
			cf_free_one(expanded_value);
			cf_free_one(next_expanded);
			expanded_value = combined;

			*current_tokens = next_token;
			next_token = next_token->next;
		}
		else
			break ;
	}

	return (expanded_value);
}

/**
 * @brief Handles a word token, expanding and adding to command arguments.
 *
 * Expands the token value, concatenates adjacent tokens, handles wildcards,
 * and advances the token list pointer.
 *
 * @param input Pointer to the input structure.
 * @param current_tokens Pointer to the token list pointer.
 * @param env Pointer to the environment structure.
 * @param shell Pointer to the shell structure.
 * @return true if successful, false otherwise.
 */
bool handle_word_token(t_input *input, t_list **current_tokens, t_env *env, t_shell *shell) {
	t_token	*current_tok;
	char	*expanded_value;
	t_list	*tokens_start;
	t_list	*tokens_end_next;

	current_tok = (t_token *)(*current_tokens)->content;
	tokens_start = *current_tokens;

	expanded_value = expand_token_value(current_tok, env, shell);
	if (!expanded_value)
		return (input->syntax_ok = false, false);

	expanded_value = concatenate_adjacent_tokens(current_tokens, expanded_value, env, shell);
	tokens_end_next = (*current_tokens)->next;

	// Используем bash-совместимую обработку wildcard
	handle_bash_compatible_wildcard_expansion(input, expanded_value, tokens_start, tokens_end_next);

	*current_tokens = (*current_tokens)->next;
	return (true);
}
