/*
 * blogc: A blog compiler.
 * Copyright (C) 2014-2020 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the BSD License.
 * See the file LICENSE.
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "content-parser.h"
#include "toctree.h"
#include "../common/utils.h"

// this is a half ass implementation of a markdown-like syntax. bugs are
// expected. feel free to improve the parser and add new features.


char*
blogc_slugify(const char *str)
{
    if (str == NULL)
        return NULL;
    char *new_str = bc_strdup(str);
    int diff = 'a' - 'A';  // just to avoid magic numbers
    for (size_t i = 0; new_str[i] != '\0'; i++) {
        if (new_str[i] >= 'a' && new_str[i] <= 'z')
            continue;
        if (new_str[i] >= '0' && new_str[i] <= '9')
            continue;
        if (new_str[i] >= 'A' && new_str[i] <= 'Z')
            new_str[i] += diff;
        else
            new_str[i] = '-';
    }
    return new_str;
}


static const char*
htmlentities(char c)
{
    switch (c) {
        case '&':
            return "&amp;";
        case '<':
            return "&lt;";
        case '>':
            return "&gt;";
        case '"':
            return "&quot;";
        case '\'':
            return "&#x27;";
        case '/':
            return "&#x2F;";
    }
    return NULL;
}


static void
htmlentities_append(bc_string_t *str, char c)
{
    const char *e = htmlentities(c);
    if (e == NULL)
        bc_string_append_c(str, c);
    else
        bc_string_append(str, e);
}


char*
blogc_htmlentities(const char *str)
{
    if (str == NULL)
        return NULL;
    bc_string_t *rv = bc_string_new();
    for (size_t i = 0; str[i] != '\0'; i++)
        htmlentities_append(rv, str[i]);
    return bc_string_free(rv, false);
}


char*
blogc_fix_description(const char *paragraph)
{
    if (paragraph == NULL)
        return NULL;
    bc_string_t *rv = bc_string_new();
    bool last = false;
    bool newline = false;
    char *tmp = NULL;
    size_t start = 0;
    size_t current = 0;
    while (true) {
        switch (paragraph[current]) {
            case '\0':
                last = true;
            case '\r':
            case '\n':
                if (newline)
                    break;
                tmp = bc_strndup(paragraph + start, current - start);
                bc_string_append(rv, bc_str_strip(tmp));
                free(tmp);
                tmp = NULL;
                if (!last)
                    bc_string_append_c(rv, ' ');
                start = current + 1;
                newline = true;
                break;
            default:
                newline = false;
        }
        if (last)
            break;
        current++;
    }
    tmp = blogc_htmlentities(bc_str_strip(rv->str));
    bc_string_free(rv, true);
    return tmp;
}


typedef enum {
    CONTENT_START_LINE = 1,
    CONTENT_EXCERPT,
    CONTENT_EXCERPT_END,
    CONTENT_HEADER,
    CONTENT_HEADER_TITLE_START,
    CONTENT_HEADER_TITLE,
    CONTENT_HTML,
    CONTENT_HTML_END,
    CONTENT_BLOCKQUOTE,
    CONTENT_BLOCKQUOTE_START,
    CONTENT_BLOCKQUOTE_END,
    CONTENT_CODE,
    CONTENT_CODE_START,
    CONTENT_CODE_END,
    CONTENT_UNORDERED_LIST_OR_HORIZONTAL_RULE_OR_EMPHASIS,
    CONTENT_HORIZONTAL_RULE_OR_EMPHASIS,
    CONTENT_HORIZONTAL_RULE,
    CONTENT_UNORDERED_LIST_START,
    CONTENT_UNORDERED_LIST_END,
    CONTENT_ORDERED_LIST,
    CONTENT_ORDERED_LIST_SPACE,
    CONTENT_ORDERED_LIST_START,
    CONTENT_ORDERED_LIST_END,
    CONTENT_PARAGRAPH,
    CONTENT_PARAGRAPH_END,
} blogc_content_parser_state_t;


typedef enum {
    CONTENT_INLINE_START = 1,
    CONTENT_INLINE_ASTERISK,
    CONTENT_INLINE_ASTERISK_DOUBLE,
    CONTENT_INLINE_UNDERSCORE,
    CONTENT_INLINE_UNDERSCORE_DOUBLE,
    CONTENT_INLINE_BACKTICKS,
    CONTENT_INLINE_BACKTICKS_DOUBLE,
    CONTENT_INLINE_LINK_START,
    CONTENT_INLINE_LINK_AUTO,
    CONTENT_INLINE_LINK_CONTENT,
    CONTENT_INLINE_LINK_URL_START,
    CONTENT_INLINE_LINK_URL,
    CONTENT_INLINE_IMAGE_START,
    CONTENT_INLINE_IMAGE_ALT,
    CONTENT_INLINE_IMAGE_URL_START,
    CONTENT_INLINE_IMAGE_URL,
    CONTENT_INLINE_ENDASH,
    CONTENT_INLINE_EMDASH,
    CONTENT_INLINE_LINE_BREAK_START,
    CONTENT_INLINE_LINE_BREAK,
} blogc_content_parser_inline_state_t;


static char*
blogc_content_parse_inline_internal(const char *src, size_t src_len)
{
    size_t current = 0;
    size_t start = 0;
    size_t count = 0;

    const char *tmp = NULL;
    char *tmp2 = NULL;
    char *tmp3 = NULL;

    size_t start_link = 0;
    char *link1 = NULL;

    bc_string_t *rv = bc_string_new();

    blogc_content_parser_inline_state_t state = CONTENT_INLINE_START;

    while (current < src_len) {
        char c = src[current];
        bool is_last = current == src_len - 1;

        switch (state) {
            case CONTENT_INLINE_START:
                if (is_last) {
                    htmlentities_append(rv, c);
                    break;
                }
                if (c == '\\') {
                    htmlentities_append(rv, src[++current]);
                    break;
                }
                if (c == '*') {
                    state = CONTENT_INLINE_ASTERISK;
                    break;
                }
                if (c == '_') {
                    state = CONTENT_INLINE_UNDERSCORE;
                    break;
                }
                if (c == '`') {
                    state = CONTENT_INLINE_BACKTICKS;
                    break;
                }
                if (c == '[') {
                    state = CONTENT_INLINE_LINK_START;
                    break;
                }
                if (c == '!') {
                    state = CONTENT_INLINE_IMAGE_START;
                    break;
                }
                if (c == '-') {
                    state = CONTENT_INLINE_ENDASH;
                    break;
                }
                if (c == ' ') {
                    state = CONTENT_INLINE_LINE_BREAK_START;
                    break;
                }
                htmlentities_append(rv, c);
                break;

            case CONTENT_INLINE_ASTERISK:
                if (c == '*') {
                    state = CONTENT_INLINE_ASTERISK_DOUBLE;
                    break;
                }
                tmp = bc_str_find(src + current, '*');
                if (tmp == NULL || ((tmp - src) >= src_len)) {
                    bc_string_append_c(rv, '*');
                    state = CONTENT_INLINE_START;
                    continue;
                }
                tmp2 = blogc_content_parse_inline_internal(
                    src + current, (tmp - src) - current);
                bc_string_append_printf(rv, "<em>%s</em>", tmp2);
                current = tmp - src;
                tmp = NULL;
                free(tmp2);
                tmp2 = NULL;
                state = CONTENT_INLINE_START;
                break;

            case CONTENT_INLINE_ASTERISK_DOUBLE:
                tmp = src + current;
                do {
                    tmp = bc_str_find(tmp, '*');
                    if (((tmp - src) < src_len) && *(tmp + 1) == '*') {
                        break;
                    }
                    tmp++;
                } while (tmp != NULL && (tmp - src) < src_len);
                if (tmp == NULL || ((tmp - src) >= src_len)) {
                    bc_string_append_c(rv, '*');
                    bc_string_append_c(rv, '*');
                    state = CONTENT_INLINE_START;
                    continue;
                }
                tmp2 = blogc_content_parse_inline_internal(
                    src + current, (tmp - src) - current);
                bc_string_append_printf(rv, "<strong>%s</strong>", tmp2);
                current = tmp - src + 1;
                tmp = NULL;
                free(tmp2);
                tmp2 = NULL;
                state = CONTENT_INLINE_START;
                break;

            case CONTENT_INLINE_UNDERSCORE:
                if (c == '_') {
                    state = CONTENT_INLINE_UNDERSCORE_DOUBLE;
                    break;
                }
                tmp = bc_str_find(src + current, '_');
                if (tmp == NULL || ((tmp - src) >= src_len)) {
                    bc_string_append_c(rv, '_');
                    state = CONTENT_INLINE_START;
                    continue;
                }
                tmp2 = blogc_content_parse_inline_internal(
                    src + current, (tmp - src) - current);
                bc_string_append_printf(rv, "<em>%s</em>", tmp2);
                current = tmp - src;
                tmp = NULL;
                free(tmp2);
                tmp2 = NULL;
                state = CONTENT_INLINE_START;
                break;

            case CONTENT_INLINE_UNDERSCORE_DOUBLE:
                tmp = src + current;
                do {
                    tmp = bc_str_find(tmp, '_');
                    if (((tmp - src) < src_len) && *(tmp + 1) == '_') {
                        break;
                    }
                    tmp++;
                } while (tmp != NULL && (tmp - src) < src_len);
                if (tmp == NULL || ((tmp - src) >= src_len)) {
                    bc_string_append_c(rv, '_');
                    bc_string_append_c(rv, '_');
                    state = CONTENT_INLINE_START;
                    continue;
                }
                tmp2 = blogc_content_parse_inline_internal(
                    src + current, (tmp - src) - current);
                bc_string_append_printf(rv, "<strong>%s</strong>", tmp2);
                current = tmp - src + 1;
                tmp = NULL;
                free(tmp2);
                tmp2 = NULL;
                state = CONTENT_INLINE_START;
                break;

            case CONTENT_INLINE_BACKTICKS:
                if (c == '`') {
                    state = CONTENT_INLINE_BACKTICKS_DOUBLE;
                    break;
                }
                tmp = bc_str_find(src + current, '`');
                if (tmp == NULL || ((tmp - src) >= src_len)) {
                    bc_string_append_c(rv, '`');
                    state = CONTENT_INLINE_START;
                    continue;
                }
                tmp3 = bc_strndup(src + current, (tmp - src) - current);
                tmp2 = blogc_htmlentities(tmp3);
                free(tmp3);
                tmp3 = NULL;
                bc_string_append(rv, "<code>");
                bc_string_append(rv, tmp2);
                bc_string_append(rv, "</code>");
                current = tmp - src;
                tmp = NULL;
                free(tmp2);
                tmp2 = NULL;
                state = CONTENT_INLINE_START;
                break;

            case CONTENT_INLINE_BACKTICKS_DOUBLE:
                tmp = src + current;
                do {
                    tmp = bc_str_find(tmp, '`');
                    if (((tmp - src) < src_len) && *(tmp + 1) == '`') {
                        break;
                    }
                    tmp++;
                } while (tmp != NULL && (tmp - src) < src_len);
                if (tmp == NULL || ((tmp - src) >= src_len)) {
                    bc_string_append_c(rv, '`');
                    bc_string_append_c(rv, '`');
                    state = CONTENT_INLINE_START;
                    continue;
                }
                tmp3 = bc_strndup(src + current, (tmp - src) - current);
                tmp2 = blogc_htmlentities(tmp3);
                free(tmp3);
                tmp3 = NULL;
                bc_string_append(rv, "<code>");
                bc_string_append(rv, tmp2);
                bc_string_append(rv, "</code>");
                current = tmp - src + 1;
                tmp = NULL;
                free(tmp2);
                tmp2 = NULL;
                state = CONTENT_INLINE_START;
                break;

            case CONTENT_INLINE_LINK_START:
                if (c == '[') {
                    state = CONTENT_INLINE_LINK_AUTO;
                    break;
                }
                start_link = current;
                count = 1;
                state = CONTENT_INLINE_LINK_CONTENT;
                break;

            case CONTENT_INLINE_LINK_AUTO:
                tmp = src + current;
                do {
                    tmp = bc_str_find(tmp, ']');
                    if (((tmp - src) < src_len) && *(tmp + 1) == ']') {
                        break;
                    }
                    tmp++;
                } while (tmp != NULL && (tmp - src) < src_len);
                if (tmp == NULL || ((tmp - src) >= src_len)) {
                    bc_string_append_c(rv, '[');
                    bc_string_append_c(rv, '[');
                    state = CONTENT_INLINE_START;
                    continue;
                }
                tmp2 = bc_strndup(src + current, (tmp - src) - current);
                bc_string_append(rv, "<a href=\"");
                bc_string_append_escaped(rv, tmp2);
                bc_string_append(rv, "\">");
                bc_string_append_escaped(rv, tmp2);
                bc_string_append(rv, "</a>");
                current = tmp - src + 1;
                tmp = NULL;
                free(tmp2);
                tmp2 = NULL;
                state = CONTENT_INLINE_START;
                break;

            case CONTENT_INLINE_LINK_CONTENT:
                if (c == '\\') {
                    current++;
                    break;
                }
                if (c == '[') {  // links can be nested :/
                    count++;
                    break;
                }
                if (c == ']') {
                    if (--count == 0) {
                        link1 = bc_strndup(src + start_link, current - start_link);
                        state = CONTENT_INLINE_LINK_URL_START;
                    }
                }
                break;

            case CONTENT_INLINE_LINK_URL_START:
                if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
                    break;
                if (c == '(') {
                    state = CONTENT_INLINE_LINK_URL;
                    start = current + 1;
                    break;
                }
                bc_string_append_c(rv, '[');
                state = CONTENT_INLINE_START;
                current = start_link;
                start_link = 0;
                continue;

            case CONTENT_INLINE_LINK_URL:
                if (c == '\\') {
                    current++;
                    break;
                }
                if (c == ')') {
                    tmp2 = bc_strndup(src + start, current - start);
                    tmp3 = blogc_content_parse_inline(link1);
                    free(link1);
                    link1 = NULL;
                    bc_string_append(rv, "<a href=\"");
                    bc_string_append_escaped(rv, tmp2);
                    bc_string_append_printf(rv, "\">%s</a>", tmp3);
                    free(tmp2);
                    tmp2 = NULL;
                    free(tmp3);
                    tmp3 = NULL;
                    state = CONTENT_INLINE_START;
                    break;
                }
                break;

            case CONTENT_INLINE_IMAGE_START:
                // we use the same variables used for links, because why not?
                if (c == '[') {
                    state = CONTENT_INLINE_IMAGE_ALT;
                    start_link = current + 1;
                    break;
                }
                bc_string_append_c(rv, '!');
                state = CONTENT_INLINE_START;
                continue;

            case CONTENT_INLINE_IMAGE_ALT:
                if (c == '\\') {
                    current++;
                    break;
                }
                if (c == ']') {
                    link1 = bc_strndup(src + start_link, current - start_link);
                    state = CONTENT_INLINE_IMAGE_URL_START;
                }
                break;

            case CONTENT_INLINE_IMAGE_URL_START:
                if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
                    break;
                if (c == '(') {
                    state = CONTENT_INLINE_IMAGE_URL;
                    start = current + 1;
                    break;
                }
                bc_string_append_c(rv, '!');
                bc_string_append_c(rv, '[');
                state = CONTENT_INLINE_START;
                current = start_link;
                start_link = 0;
                continue;

            case CONTENT_INLINE_IMAGE_URL:
                if (c == '\\') {
                    current++;
                    break;
                }
                if (c == ')') {
                    tmp2 = bc_strndup(src + start, current - start);
                    bc_string_append(rv, "<img src=\"");
                    bc_string_append_escaped(rv, tmp2);
                    bc_string_append(rv, "\" alt=\"");
                    bc_string_append_escaped(rv, link1);
                    bc_string_append(rv, "\">");
                    free(tmp2);
                    tmp2 = NULL;
                    free(link1);
                    link1 = NULL;
                    state = CONTENT_INLINE_START;
                    break;
                }
                break;

            case CONTENT_INLINE_ENDASH:
                if (c == '-') {
                    if (is_last) {
                        bc_string_append(rv, "&ndash;");
                        state = CONTENT_INLINE_START;  // wat
                        break;
                    }
                    state = CONTENT_INLINE_EMDASH;
                    break;
                }
                bc_string_append_c(rv, '-');
                state = CONTENT_INLINE_START;
                continue;

            case CONTENT_INLINE_EMDASH:
                if (c == '-') {
                    bc_string_append(rv, "&mdash;");
                    state = CONTENT_INLINE_START;
                    break;
                }
                bc_string_append(rv, "&ndash;");
                state = CONTENT_INLINE_START;
                continue;

            case CONTENT_INLINE_LINE_BREAK_START:
                if (c == ' ') {
                    if (is_last) {
                        bc_string_append(rv, "<br />");
                        state = CONTENT_INLINE_START;  // wat
                        break;
                    }
                    count = 2;
                    state = CONTENT_INLINE_LINE_BREAK;
                    break;
                }
                bc_string_append_c(rv, ' ');
                state = CONTENT_INLINE_START;
                continue;

            case CONTENT_INLINE_LINE_BREAK:
                if (c == ' ') {
                    if (is_last) {
                        bc_string_append(rv, "<br />");
                        state = CONTENT_INLINE_START;  // wat
                        break;
                    }
                    count++;
                    break;
                }
                if (c == '\n' || c == '\r') {
                    bc_string_append_printf(rv, "<br />%c", c);
                    state = CONTENT_INLINE_START;
                    break;
                }
                for (size_t i = 0; i < count; i++)
                    bc_string_append_c(rv, ' ');
                state = CONTENT_INLINE_START;
                continue;
        }
        current++;
    }

    switch (state) {

        // if after the end of the loop we are on any of the following states,
        // we must call the parser again, from start_link
        case CONTENT_INLINE_IMAGE_START:
        case CONTENT_INLINE_IMAGE_ALT:
        case CONTENT_INLINE_IMAGE_URL_START:
        case CONTENT_INLINE_IMAGE_URL:
            bc_string_append_c(rv, '!');

        case CONTENT_INLINE_LINK_CONTENT:
        case CONTENT_INLINE_LINK_URL_START:
        case CONTENT_INLINE_LINK_URL:
            tmp2 = blogc_content_parse_inline(src + start_link);
            bc_string_append_c(rv, '[');
            bc_string_append_escaped(rv, tmp2);  // no need to free, as it wil be done below.
            break;

        // add all the other states here explicitly, so the compiler helps us
        // not missing any new state that should be handled.
        case CONTENT_INLINE_START:
        case CONTENT_INLINE_ASTERISK:
        case CONTENT_INLINE_ASTERISK_DOUBLE:
        case CONTENT_INLINE_UNDERSCORE:
        case CONTENT_INLINE_UNDERSCORE_DOUBLE:
        case CONTENT_INLINE_BACKTICKS:
        case CONTENT_INLINE_BACKTICKS_DOUBLE:
        case CONTENT_INLINE_LINK_START:
        case CONTENT_INLINE_LINK_AUTO:
        case CONTENT_INLINE_ENDASH:
        case CONTENT_INLINE_EMDASH:
        case CONTENT_INLINE_LINE_BREAK_START:
        case CONTENT_INLINE_LINE_BREAK:
            break;
    }

    free(tmp2);
    free(tmp3);
    free(link1);

    return bc_string_free(rv, false);
}


char*
blogc_content_parse_inline(const char *src)
{
    return blogc_content_parse_inline_internal(src, strlen(src));
}


bool
blogc_is_ordered_list_item(const char *str, size_t prefix_len)
{
    if (str == NULL)
        return false;

    if (strlen(str) < 2)
        return false;

    size_t i;

    for (i = 0; str[i] >= '0' && str[i] <= '9'; i++);

    if (i == 0)
        return false;
    if (str[i] != '.')
        return false;

    for (i++; i < prefix_len && (str[i] == ' ' || str[i] == '\t'); i++);

    if (str[i] == '\0')
        return false;

    return i == prefix_len;
}


char*
blogc_content_parse(const char *src, size_t *end_excerpt, char **first_header,
    char **description, char **endl, bc_slist_t **headers)
{
    // src is always nul-terminated.
    size_t src_len = strlen(src);

    size_t current = 0;
    size_t start = 0;
    size_t start2 = 0;
    size_t end = 0;
    size_t eend = 0;
    size_t real_end = 0;

    size_t header_level = 0;
    char *prefix = NULL;
    size_t prefix_len = 0;
    char *tmp = NULL;
    char *tmp2 = NULL;
    char *parsed = NULL;
    char *slug = NULL;

    char *line_ending = NULL;
    bool line_ending_found = false;
    if (endl != NULL) {
        if (*endl != NULL) {
            line_ending_found = true;
        }
        else {
            *endl = bc_malloc(3 * sizeof(char));
        }
        line_ending = *endl;
    }
    else {
        line_ending = bc_malloc(3 * sizeof(char));
    }

    // this isn't empty because we need some reasonable default value in the
    // unlikely case that we need to print some line ending before evaluating
    // the "real" value.
    if (!line_ending_found) {
        line_ending[0] = '\n';
        line_ending[1] = '\0';
    }

    char d = '\0';

    bc_slist_t *lines = NULL;
    bc_slist_t *lines2 = NULL;

    bc_string_t *rv = bc_string_new();
    bc_string_t *tmp_str = NULL;

    blogc_content_parser_state_t state = CONTENT_START_LINE;

    while (current < src_len) {
        char c = src[current];
        bool is_last = current == src_len - 1;

        if (c == '\n' || c == '\r') {
            if ((current + 1) < src_len) {
                if ((c == '\n' && src[current + 1] == '\r') ||
                    (c == '\r' && src[current + 1] == '\n'))
                {
                    if (!line_ending_found) {
                        line_ending[0] = c;
                        line_ending[1] = src[current + 1];
                        line_ending[2] = '\0';
                        line_ending_found = true;
                    }
                    real_end = current;
                    c = src[++current];
                    is_last = current == src_len - 1;
                }
            }
            if (!line_ending_found) {
                line_ending[0] = c;
                line_ending[1] = '\0';
                line_ending_found = true;
            }
        }

        switch (state) {

            case CONTENT_START_LINE:
                if (c == '\n' || c == '\r' || is_last)
                    break;
                start = current;
                if (c == '.') {
                    if (end_excerpt != NULL) {
                        eend = rv->len;  // fuck it
                        state = CONTENT_EXCERPT;
                        break;
                    }
                }
                if (c == '#') {
                    header_level = 1;
                    state = CONTENT_HEADER;
                    break;
                }
                if (c == '*' || c == '+' || c == '-') {
                    start2 = current;
                    state = CONTENT_UNORDERED_LIST_OR_HORIZONTAL_RULE_OR_EMPHASIS;
                    d = c;
                    break;
                }
                if (c >= '0' && c <= '9') {
                    start2 = current;
                    state = CONTENT_ORDERED_LIST;
                    break;
                }
                if (c == ' ' || c == '\t') {
                    start2 = current;
                    state = CONTENT_CODE;
                    break;
                }
                if (c == '<') {
                    state = CONTENT_HTML;
                    break;
                }
                if (c == '>') {
                    state = CONTENT_BLOCKQUOTE;
                    start2 = current;
                    break;
                }
                state = CONTENT_PARAGRAPH;
                break;

            case CONTENT_EXCERPT:
                if (end_excerpt != NULL) {
                    if (c == '.')
                        break;
                    if (c == '\n' || c == '\r') {
                        state = CONTENT_EXCERPT_END;
                        break;
                    }
                }
                eend = 0;
                state = CONTENT_PARAGRAPH;
                break;

            case CONTENT_EXCERPT_END:
                if (end_excerpt != NULL) {
                    if (c == '\n' || c == '\r') {
                        *end_excerpt = eend;
                        state = CONTENT_START_LINE;
                        break;
                    }
                }
                eend = 0;
                state = CONTENT_PARAGRAPH_END;
                break;

            case CONTENT_HEADER:
                if (c == '#') {
                    header_level += 1;
                    break;
                }
                if (c == ' ' || c == '\t') {
                    state = CONTENT_HEADER_TITLE_START;
                    break;
                }
                state = CONTENT_PARAGRAPH;
                break;

            case CONTENT_HEADER_TITLE_START:
                if (c == ' ' || c == '\t')
                    break;
                start = current;
                if (c != '\n' && c != '\r') {
                    state = CONTENT_HEADER_TITLE;
                    break;
                }

            case CONTENT_HEADER_TITLE:
                if (c == '\n' || c == '\r' || is_last) {
                    end = is_last && c != '\n' && c != '\r' ? src_len :
                        (real_end != 0 ? real_end : current);
                    tmp = bc_strndup(src + start, end - start);
                    if (first_header != NULL && *first_header == NULL)
                        *first_header = blogc_htmlentities(tmp);
                    parsed = blogc_content_parse_inline(tmp);
                    slug = blogc_slugify(tmp);
                    if (headers != NULL)
                        *headers = blogc_toctree_append(*headers, header_level, slug, parsed);
                    if (slug == NULL)
                        bc_string_append_printf(rv, "<h%d>%s</h%d>%s",
                            header_level, parsed, header_level, line_ending);
                    else
                        bc_string_append_printf(rv, "<h%d id=\"%s\">%s</h%d>%s",
                            header_level, slug, parsed, header_level,
                            line_ending);
                    free(slug);
                    free(parsed);
                    parsed = NULL;
                    free(tmp);
                    tmp = NULL;
                    state = CONTENT_START_LINE;
                    start = current;
                }
                break;

            case CONTENT_HTML:
                if (c == '\n' || c == '\r' || is_last) {
                    state = CONTENT_HTML_END;
                    end = is_last && c != '\n' && c != '\r' ? src_len :
                        (real_end != 0 ? real_end : current);
                }
                if (!is_last)
                    break;

            case CONTENT_HTML_END:
                if (c == '\n' || c == '\r' || is_last) {
                    tmp = bc_strndup(src + start, end - start);
                    bc_string_append_printf(rv, "%s%s", tmp, line_ending);
                    free(tmp);
                    tmp = NULL;
                    state = CONTENT_START_LINE;
                    start = current;
                }
                else
                    state = CONTENT_HTML;
                break;

            case CONTENT_BLOCKQUOTE:
                if (c == ' ' || c == '\t')
                    break;
                prefix = bc_strndup(src + start, current - start);
                state = CONTENT_BLOCKQUOTE_START;
                break;

            case CONTENT_BLOCKQUOTE_START:
                if (c == '\n' || c == '\r' || is_last) {
                    end = is_last && c != '\n' && c != '\r' ? src_len :
                        (real_end != 0 ? real_end : current);
                    tmp = bc_strndup(src + start2, end - start2);
                    if (bc_str_starts_with(tmp, prefix)) {
                        lines = bc_slist_append(lines, bc_strdup(tmp + strlen(prefix)));
                        state = CONTENT_BLOCKQUOTE_END;
                    }
                    else {
                        state = CONTENT_PARAGRAPH;
                        free(prefix);
                        prefix = NULL;
                        bc_slist_free_full(lines, free);
                        lines = NULL;
                        if (is_last) {
                            free(tmp);
                            tmp = NULL;
                            continue;
                        }
                    }
                    free(tmp);
                    tmp = NULL;
                }
                if (!is_last)
                    break;

            case CONTENT_BLOCKQUOTE_END:
                if (c == '\n' || c == '\r' || is_last) {
                    tmp_str = bc_string_new();
                    for (bc_slist_t *l = lines; l != NULL; l = l->next)
                        bc_string_append_printf(tmp_str, "%s%s", l->data,
                            line_ending);
                    // do not propagate title and description to blockquote parsing,
                    // because we just want paragraphs from first level of
                    // content.
                    tmp = blogc_content_parse(tmp_str->str, NULL, NULL, NULL, endl, NULL);
                    bc_string_append_printf(rv, "<blockquote>%s</blockquote>%s",
                        tmp, line_ending);
                    free(tmp);
                    tmp = NULL;
                    bc_string_free(tmp_str, true);
                    tmp_str = NULL;
                    bc_slist_free_full(lines, free);
                    lines = NULL;
                    free(prefix);
                    prefix = NULL;
                    state = CONTENT_START_LINE;
                    start2 = current;
                }
                else {
                    start2 = current;
                    state = CONTENT_BLOCKQUOTE_START;
                }
                break;

            case CONTENT_CODE:
                if (c == ' ' || c == '\t')
                    break;
                prefix = bc_strndup(src + start, current - start);
                state = CONTENT_CODE_START;
                break;

            case CONTENT_CODE_START:
                if (c == '\n' || c == '\r' || is_last) {
                    end = is_last && c != '\n' && c != '\r' ? src_len :
                        (real_end != 0 ? real_end : current);
                    tmp = bc_strndup(src + start2, end - start2);
                    if (bc_str_starts_with(tmp, prefix)) {
                        lines = bc_slist_append(lines, bc_strdup(tmp + strlen(prefix)));
                        state = CONTENT_CODE_END;
                    }
                    else {
                        state = CONTENT_PARAGRAPH;
                        free(prefix);
                        prefix = NULL;
                        bc_slist_free_full(lines, free);
                        lines = NULL;
                        free(tmp);
                        tmp = NULL;
                        if (is_last)
                            continue;
                        break;
                    }
                    free(tmp);
                    tmp = NULL;
                }
                if (!is_last)
                    break;

            case CONTENT_CODE_END:
                if (c == '\n' || c == '\r' || is_last) {
                    bc_string_append(rv, "<pre><code>");
                    for (bc_slist_t *l = lines; l != NULL; l = l->next) {
                        char *tmp_line = blogc_htmlentities(l->data);
                        if (l->next == NULL)
                            bc_string_append_printf(rv, "%s", tmp_line);
                        else
                            bc_string_append_printf(rv, "%s%s", tmp_line,
                                line_ending);
                        free(tmp_line);
                    }
                    bc_string_append_printf(rv, "</code></pre>%s", line_ending);
                    bc_slist_free_full(lines, free);
                    lines = NULL;
                    free(prefix);
                    prefix = NULL;
                    state = CONTENT_START_LINE;
                    start2 = current;
                }
                else {
                    start2 = current;
                    state = CONTENT_CODE_START;
                }
                break;

            case CONTENT_UNORDERED_LIST_OR_HORIZONTAL_RULE_OR_EMPHASIS:
                if (current == start+1) {
                    if (c == d) { // horizontal rule or '**' emphasis
                        state = CONTENT_HORIZONTAL_RULE_OR_EMPHASIS;
                        break;
                    }
                    else if (c != ' ' && c != '\t' && d == '*') { // is '*' emphasis
                        state = CONTENT_PARAGRAPH;
                        break;
                    }
                }
                if (c == ' ' || c == '\t')
                    break;
                prefix = bc_strndup(src + start, current - start);
                state = CONTENT_UNORDERED_LIST_START;
                break;

            case CONTENT_HORIZONTAL_RULE_OR_EMPHASIS:
                // 3rd '-' or '*' required for a horizontal rule
                if (c == d) {
                    state = CONTENT_HORIZONTAL_RULE;
                    if (is_last)
                        continue;
                    break;
                }
                // is '**' emphasis
                state = CONTENT_PARAGRAPH;
                break;

            case CONTENT_HORIZONTAL_RULE:
                if (c == d && !is_last) {
                    break;
                }
                if (c == '\n' || c == '\r' || is_last) {
                    bc_string_append_printf(rv, "<hr />%s", line_ending);
                    state = CONTENT_START_LINE;
                    start = current;
                    d = '\0';
                    break;
                }
                state = CONTENT_PARAGRAPH;
                break;

            case CONTENT_UNORDERED_LIST_START:
                if (c == '\n' || c == '\r' || is_last) {
                    end = is_last && c != '\n' && c != '\r' ? src_len :
                        (real_end != 0 ? real_end : current);
                    tmp = bc_strndup(src + start2, end - start2);
                    tmp2 = bc_strdup_printf("%-*s", strlen(prefix), "");
                    if (bc_str_starts_with(tmp, prefix)) {
                        if (lines2 != NULL) {
                            tmp_str = bc_string_new();
                            for (bc_slist_t *l = lines2; l != NULL; l = l->next) {
                                if (l->next == NULL)
                                    bc_string_append_printf(tmp_str, "%s", l->data);
                                else
                                    bc_string_append_printf(tmp_str, "%s%s", l->data,
                                        line_ending);
                            }
                            bc_slist_free_full(lines2, free);
                            lines2 = NULL;
                            parsed = blogc_content_parse_inline(tmp_str->str);
                            bc_string_free(tmp_str, true);
                            lines = bc_slist_append(lines, bc_strdup(parsed));
                            free(parsed);
                            parsed = NULL;
                        }
                        lines2 = bc_slist_append(lines2, bc_strdup(tmp + strlen(prefix)));
                    }
                    else if (bc_str_starts_with(tmp, tmp2)) {
                        lines2 = bc_slist_append(lines2, bc_strdup(tmp + strlen(prefix)));
                    }
                    else {
                        state = CONTENT_PARAGRAPH_END;
                        free(tmp);
                        tmp = NULL;
                        free(tmp2);
                        tmp2 = NULL;
                        free(prefix);
                        prefix = NULL;
                        bc_slist_free_full(lines, free);
                        bc_slist_free_full(lines2, free);
                        lines = NULL;
                        if (is_last)
                            continue;
                        break;
                    }
                    free(tmp);
                    tmp = NULL;
                    free(tmp2);
                    tmp2 = NULL;
                    state = CONTENT_UNORDERED_LIST_END;
                }
                if (!is_last)
                    break;

            case CONTENT_UNORDERED_LIST_END:
                if (c == '\n' || c == '\r' || is_last) {
                    if (lines2 != NULL) {
                        // FIXME: avoid repeting the code below
                        tmp_str = bc_string_new();
                        for (bc_slist_t *l = lines2; l != NULL; l = l->next) {
                            if (l->next == NULL)
                                bc_string_append_printf(tmp_str, "%s", l->data);
                            else
                                bc_string_append_printf(tmp_str, "%s%s", l->data,
                                    line_ending);
                        }
                        bc_slist_free_full(lines2, free);
                        lines2 = NULL;
                        parsed = blogc_content_parse_inline(tmp_str->str);
                        bc_string_free(tmp_str, true);
                        lines = bc_slist_append(lines, bc_strdup(parsed));
                        free(parsed);
                        parsed = NULL;
                    }
                    bc_string_append_printf(rv, "<ul>%s", line_ending);
                    for (bc_slist_t *l = lines; l != NULL; l = l->next)
                        bc_string_append_printf(rv, "<li>%s</li>%s", l->data,
                            line_ending);
                    bc_string_append_printf(rv, "</ul>%s", line_ending);
                    bc_slist_free_full(lines, free);
                    lines = NULL;
                    free(prefix);
                    prefix = NULL;
                    state = CONTENT_START_LINE;
                    start2 = current;
                }
                else {
                    start2 = current;
                    state = CONTENT_UNORDERED_LIST_START;
                }
                break;

            case CONTENT_ORDERED_LIST:
                if (c >= '0' && c <= '9')
                    break;
                if (c == '.') {
                    state = CONTENT_ORDERED_LIST_SPACE;
                    break;
                }
                state = CONTENT_PARAGRAPH;
                if (is_last)
                    continue;
                break;

            case CONTENT_ORDERED_LIST_SPACE:
                if (c == ' ' || c == '\t')
                    break;
                prefix_len = current - start;
                state = CONTENT_ORDERED_LIST_START;
                if (c != '\n' && c != '\r' && !is_last)
                    break;

            case CONTENT_ORDERED_LIST_START:
                if (c == '\n' || c == '\r' || is_last) {
                    end = is_last && c != '\n' && c != '\r' ? src_len :
                        (real_end != 0 ? real_end : current);
                    tmp = bc_strndup(src + start2, end - start2);
                    tmp2 = bc_strdup_printf("%-*s", prefix_len, "");
                    if (blogc_is_ordered_list_item(tmp, prefix_len)) {
                        if (lines2 != NULL) {
                            tmp_str = bc_string_new();
                            for (bc_slist_t *l = lines2; l != NULL; l = l->next) {
                                if (l->next == NULL)
                                    bc_string_append_printf(tmp_str, "%s", l->data);
                                else
                                    bc_string_append_printf(tmp_str, "%s%s", l->data,
                                        line_ending);
                            }
                            bc_slist_free_full(lines2, free);
                            lines2 = NULL;
                            parsed = blogc_content_parse_inline(tmp_str->str);
                            bc_string_free(tmp_str, true);
                            lines = bc_slist_append(lines, bc_strdup(parsed));
                            free(parsed);
                            parsed = NULL;
                        }
                        lines2 = bc_slist_append(lines2, bc_strdup(tmp + prefix_len));
                    }
                    else if (bc_str_starts_with(tmp, tmp2)) {
                        lines2 = bc_slist_append(lines2, bc_strdup(tmp + prefix_len));
                    }
                    else {
                        state = CONTENT_PARAGRAPH_END;
                        free(tmp);
                        tmp = NULL;
                        free(tmp2);
                        tmp2 = NULL;
                        free(parsed);
                        parsed = NULL;
                        bc_slist_free_full(lines, free);
                        bc_slist_free_full(lines2, free);
                        lines = NULL;
                        if (is_last)
                            continue;
                        break;
                    }
                    free(tmp);
                    tmp = NULL;
                    free(tmp2);
                    tmp2 = NULL;
                    state = CONTENT_ORDERED_LIST_END;
                }
                if (!is_last)
                    break;

            case CONTENT_ORDERED_LIST_END:
                if (c == '\n' || c == '\r' || is_last) {
                    if (lines2 != NULL) {
                        // FIXME: avoid repeting the code below
                        tmp_str = bc_string_new();
                        for (bc_slist_t *l = lines2; l != NULL; l = l->next) {
                            if (l->next == NULL)
                                bc_string_append_printf(tmp_str, "%s", l->data);
                            else
                                bc_string_append_printf(tmp_str, "%s%s", l->data,
                                    line_ending);
                        }
                        bc_slist_free_full(lines2, free);
                        lines2 = NULL;
                        parsed = blogc_content_parse_inline(tmp_str->str);
                        bc_string_free(tmp_str, true);
                        lines = bc_slist_append(lines, bc_strdup(parsed));
                        free(parsed);
                        parsed = NULL;
                    }
                    bc_string_append_printf(rv, "<ol>%s", line_ending);
                    for (bc_slist_t *l = lines; l != NULL; l = l->next)
                        bc_string_append_printf(rv, "<li>%s</li>%s", l->data,
                            line_ending);
                    bc_string_append_printf(rv, "</ol>%s", line_ending);
                    bc_slist_free_full(lines, free);
                    lines = NULL;
                    free(prefix);
                    prefix = NULL;
                    state = CONTENT_START_LINE;
                    start2 = current;
                }
                else {
                    start2 = current;
                    state = CONTENT_ORDERED_LIST_START;
                }
                break;

            case CONTENT_PARAGRAPH:
                if (c == '\n' || c == '\r' || is_last) {
                    state = CONTENT_PARAGRAPH_END;
                    end = is_last && c != '\n' && c != '\r' ? src_len :
                        (real_end != 0 ? real_end : current);
                }
                if (!is_last)
                    break;

            case CONTENT_PARAGRAPH_END:
                if (c == '\n' || c == '\r' || is_last) {
                    tmp = bc_strndup(src + start, end - start);
                    if (description != NULL && *description == NULL)
                        *description = blogc_fix_description(tmp);
                    parsed = blogc_content_parse_inline(tmp);
                    bc_string_append_printf(rv, "<p>%s</p>%s", parsed,
                        line_ending);
                    free(parsed);
                    parsed = NULL;
                    free(tmp);
                    tmp = NULL;
                    state = CONTENT_START_LINE;
                    start = current;
                }
                else
                    state = CONTENT_PARAGRAPH;
                break;

        }

        current++;
    }

    if (endl == NULL) {
        free(line_ending);
    }

    return bc_string_free(rv, false);
}
