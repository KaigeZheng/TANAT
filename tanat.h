/*
 * TANAT - Text ANAlyze Toolkit
 * Author: Kaige Zheng
 * Email: kambrikg@outlook.com
 * Homepage: https://github.com/KaigeZheng/tanat
 * License: MIT License
 * Description: Header file, declares functions for the text analysis toolkit.
 */
#ifndef TANAT_H
#define TANAT_H

#define MAX_WORD 65536
#define TOP_N 10
#define WORD_TABLE_SIZE 1024

void analyze_file(const char *filepath, int show_report, const char *report_path, const char *color);
void replace_in_file(const char *filepath, const char *old, const char *newstr);
void diff_files(const char *file1, const char *file2);
void analyze_dir(const char *dirpath, int show_report, const char *report_path);
int is_word_boundary(char c);
void print_highlighted_word(const char *word, const char *color);
void print_line_with_highlight(const char *line, char top_words[TOP_N][MAX_WORD], int top_n, const char *color);

#endif
