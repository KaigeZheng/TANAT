/*
 * TANAT - Text ANAlyze Toolkit
 * Author: Kaige Zheng
 * Email: kambrikg@outlook.com
 * Homepage: https://github.com/KaigeZheng/tanat
 * License: MIT License
 * Description: An open-source toolkit for text analysis, supporting character, word, line statistics and high-frequency word analysis.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include "tanat.h"

/**
 * struct WordFreq
 * 用于存储单词及其出现频率
 * @word    单词内容（小写，最长MAX_WORD-1字符）
 * @count   该单词出现的次数
 **/
struct WordFreq {
    char word[MAX_WORD];
    int count;
};

/**
 * struct GlobalStat
 * 用于存储全局统计信息（字符数、单词数、行数和词频数）
 * @chars           字符总数
 * @words           单词总数
 * @lines           行数
 * @word_table      词频表（存储所有出现过的单词及其频率）
 * @word_table_size 词频表中实际单词数量
 **/
struct GlobalStat {
    int chars;
    int words;
    int lines;
    struct WordFreq word_table[WORD_TABLE_SIZE];
    int word_table_size;
};

/*
 * add_word
 * 功能：将单词添加到词频表中，若已存在则计数加一。
 * 输入：table - 词频表数组，size - 当前表大小指针，word - 单词字符串
 */
void add_word(struct WordFreq *table, int *size, const char *word) {
    for (int i = 0; i < *size; ++i) {
        if (strcmp(table[i].word, word) == 0) {
            table[i].count++;
            return;
        }
    }
    if (*size < WORD_TABLE_SIZE) {
        strcpy(table[*size].word, word);
        table[*size].count = 1;
        (*size)++;
    }
}

/*
 * cmp_word
 * 功能：用于qsort的比较函数，按词频降序排列。
 * 输入：a, b - 指向WordFreq的指针
 */
int cmp_word(const void *a, const void *b) {
    return ((struct WordFreq*)b)->count - ((struct WordFreq*)a)->count;
}

/*
 * add_word_global
 * 功能：将单词添加到全局统计结构体的词频表中。
 * 输入：stat - 全局统计结构体指针，word - 单词字符串
 */
void add_word_global(struct GlobalStat *stat, const char *word) {
    for (int i = 0; i < stat->word_table_size; ++i) {
        if (strcmp(stat->word_table[i].word, word) == 0) {
            stat->word_table[i].count++;
            return;
        }
    }
    if (stat->word_table_size < WORD_TABLE_SIZE) {
        strcpy(stat->word_table[stat->word_table_size].word, word);
        stat->word_table[stat->word_table_size].count = 1;
        stat->word_table_size++;
    }
}

// 高亮颜色代码
const char *get_color_code(const char *color) {
    // 支持的颜色：yellow, red, green, blue, magenta, cyan, white, black, gray, pink, orange, brown, purple, brightred, brightgreen, brightblue, brightmagenta, brightcyan, brightwhite, lightgray, darkgray
    if (strcmp(color, "yellow") == 0) return "\033[1;33m";
    if (strcmp(color, "red") == 0) return "\033[1;31m";
    if (strcmp(color, "green") == 0) return "\033[1;32m";
    if (strcmp(color, "blue") == 0) return "\033[1;34m";
    if (strcmp(color, "magenta") == 0) return "\033[1;35m";
    if (strcmp(color, "cyan") == 0) return "\033[1;36m";
    if (strcmp(color, "white") == 0) return "\033[1;37m";
    if (strcmp(color, "black") == 0) return "\033[1;30m";
    if (strcmp(color, "gray") == 0) return "\033[1;38m";
    if (strcmp(color, "pink") == 0) return "\033[1;95m";
    if (strcmp(color, "orange") == 0) return "\033[38;5;208m";
    if (strcmp(color, "brown") == 0) return "\033[38;5;94m";
    if (strcmp(color, "purple") == 0) return "\033[38;5;93m";
    if (strcmp(color, "brightred") == 0) return "\033[1;91m";
    if (strcmp(color, "brightgreen") == 0) return "\033[1;92m";
    if (strcmp(color, "brightblue") == 0) return "\033[1;94m";
    if (strcmp(color, "brightmagenta") == 0) return "\033[1;95m";
    if (strcmp(color, "brightcyan") == 0) return "\033[1;96m";
    if (strcmp(color, "brightwhite") == 0) return "\033[1;97m";
    if (strcmp(color, "lightgray") == 0) return "\033[0;37m";
    if (strcmp(color, "darkgray") == 0) return "\033[1;90m";
    return "\033[1;33m"; // 默认黄色
}

/*
 * print_highlighted_word
 * 功能：在终端高亮显示关键词。
 * 输入：word - 关键词字符串
 * color - 颜色字符串
 */
void print_highlighted_word(const char *word, const char *color) {
    printf("%s%s\033[0m", get_color_code(color), word);
}

/*
 * is_word_boundary
 * 功能：判断字符是否为单词边界。
 * 输入：c - 字符
 * 返回：1为单词边界，0为非单词边界
 */
int is_word_boundary(char c) {
    return !isalnum((unsigned char)c);
}

/*
 * print_line_with_highlight
 * 功能：在一行中高亮所有高频词。
 * 输入：line - 文本行，top_words - 高频词数组，top_n - 高频词数量
 * color - 高亮颜色
 */
void print_line_with_highlight(const char *line, char top_words[TOP_N][MAX_WORD], int top_n, const char *color) {
    int len = strlen(line);
    int i = 0;
    while (i < len) {
        int matched = 0;
        for (int k = 0; k < top_n; ++k) {
            int wlen = strlen(top_words[k]);
            if (wlen > 0 && strncasecmp(&line[i], top_words[k], wlen) == 0) {
                // 检查单词边界
                char before = (i == 0) ? ' ' : line[i-1];
                char after = line[i+wlen];
                if (is_word_boundary(before) && is_word_boundary(after)) {
                    print_highlighted_word(top_words[k], color);
                    i += wlen;
                    matched = 1;
                    break;
                }
            }
        }
        if (!matched) {
            putchar(line[i]);
            i++;
        }
    }
}

/*
 * analyze_file
 * 功能：统计单个文件的字符数、单词数、行数和高频词，并输出报告。
 * 输入：filepath - 文件路径，show_report - 是否输出到文件，report_path - 报告文件路径
 */
void analyze_file(const char *filepath, int show_report, const char *report_path, const char *color) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) { printf("无法打开文件: %s\n", filepath); return; }
    int chars = 0, words = 0, lines = 0;
    char buf[4096];
    struct WordFreq *word_table = malloc(sizeof(struct WordFreq) * WORD_TABLE_SIZE);
    if (!word_table) {
        printf("内存分配失败\n");
        fclose(fp);
        return;
    }
    int word_table_size = 0;
    while (fgets(buf, sizeof(buf), fp)) {
        lines++;
        chars += strlen(buf);
        char *p = buf;
        while (*p) {
            while (*p && !isalnum(*p)) p++;
            char word[MAX_WORD]; int idx = 0;
            while (*p && isalnum(*p)) {
                word[idx++] = tolower(*p);
                p++;
            }
            if (idx > 0) {
                word[idx] = '\0';
                add_word(word_table, &word_table_size, word);
                words++;
            }
        }
    }
    fclose(fp);
    qsort(word_table, word_table_size, sizeof(struct WordFreq), cmp_word);
    // 保存高频词
    char top_words[TOP_N][MAX_WORD] = {0};
    int real_top = (word_table_size < TOP_N) ? word_table_size : TOP_N;
    for (int i = 0; i < real_top; ++i) {
        memcpy(top_words[i], word_table[i].word, MAX_WORD - 1);
        top_words[i][MAX_WORD - 1] = '\0';
    }
    // 始终输出到文件（如有）
    if (show_report && report_path) {
        FILE *out = fopen(report_path, "w");
        if (out) {
            fprintf(out, "==============================\n");
            fprintf(out, "  文本分析报告\n");
            fprintf(out, "==============================\n");
            fprintf(out, "文件: %s\n", filepath);
            fprintf(out, "------------------------------\n");
            fprintf(out, "字符总数 : %d\n", chars);
            fprintf(out, "单词总数 : %d\n", words);
            fprintf(out, "行    数 : %d\n", lines);
            fprintf(out, "------------------------------\n");
            fprintf(out, "前 %d 高频词：\n", TOP_N);
            fprintf(out, "+----------------------+--------+\n");
            fprintf(out, "| 词语                 | 频率   |\n");
            fprintf(out, "+----------------------+--------+\n");
            for (int i = 0; i < TOP_N && i < word_table_size; ++i) {
                fprintf(out, "| %-20s | %-6d |\n", word_table[i].word, word_table[i].count);
            }
            fprintf(out, "+----------------------+--------+\n");
            fprintf(out, "==============================\n");
            fclose(out);
        }
    }
    // 始终高亮打印统计报告到终端
    printf("==============================\n");
    printf("  文本分析报告\n");
    printf("==============================\n");
    printf("文件: %s\n", filepath);
    printf("------------------------------\n");
    printf("字符总数 : %d\n", chars);
    printf("单词总数 : %d\n", words);
    printf("行    数 : %d\n", lines);
    printf("------------------------------\n");
    printf("前 %d 高频词：\n", TOP_N);
    printf("+----------------------+--------+\n");
    printf("| 词语                 | 频率   |\n");
    printf("+----------------------+--------+\n");
    for (int i = 0; i < TOP_N && i < word_table_size; ++i) {
        printf("| ");
        print_highlighted_word(word_table[i].word, color);
        int len = strlen(word_table[i].word);
        for (int j = len; j < 20; ++j) printf(" ");
        printf(" | %-6d |\n", word_table[i].count);
    }
    printf("+----------------------+--------+\n");
    printf("==============================\n");
    // 输出高亮文本内容
    printf("\n原文内容（高频词高亮）：\n");
    fp = fopen(filepath, "r");
    if (fp) {
        while (fgets(buf, sizeof(buf), fp)) {
            print_line_with_highlight(buf, top_words, real_top, color);
        }
        fclose(fp);
    }
    free(word_table);
}

/*
 * replace_in_file
 * 功能：将文件中所有old字符串替换为newstr。
 * 输入：filepath - 文件路径，old - 需替换的字符串，newstr - 替换后的字符串
 */
void replace_in_file(const char *filepath, const char *old, const char *newstr) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) { printf("无法打开文件: %s\n", filepath); return; }
    FILE *tmp = fopen(".tmp_tanat", "w");
    char buf[4096];
    int replaced = 0;
    while (fgets(buf, sizeof(buf), fp)) {
        char *pos = buf;
        while ((pos = strstr(pos, old))) {
            fwrite(buf, 1, pos - buf, tmp);
            fwrite(newstr, 1, strlen(newstr), tmp);
            pos += strlen(old);
            memmove(buf, pos, strlen(pos) + 1);
            pos = buf;
            replaced = 1;
        }
        fputs(buf, tmp);
    }
    fclose(fp); fclose(tmp);
    rename(".tmp_tanat", filepath);
    if (replaced) {
        printf("[替换完成] 文件: %s | '%s' => '%s'\n", filepath, old, newstr);
    }
}

/*
 * diff_files
 * 功能：比较两个文件的内容，输出不同之处。
 * 输入：file1, file2 - 两个文件路径
 */
void diff_files(const char *file1, const char *file2) {
    FILE *fp1 = fopen(file1, "r");
    FILE *fp2 = fopen(file2, "r");
    if (!fp1 || !fp2) { printf("无法打开文件\n"); return; }
    char buf1[4096], buf2[4096];
    int line = 1;
    while (fgets(buf1, sizeof(buf1), fp1) && fgets(buf2, sizeof(buf2), fp2)) {
        if (strcmp(buf1, buf2) != 0) {
            printf("第%d行不同:\n< %s> %s\n", line, buf1, buf2);
        }
        line++;
    }
    while (fgets(buf1, sizeof(buf1), fp1)) {
        printf("第%d行文件1多余: %s", line++, buf1);
    }
    while (fgets(buf2, sizeof(buf2), fp2)) {
        printf("第%d行文件2多余: %s", line++, buf2);
    }
    fclose(fp1); fclose(fp2);
}

/*
 * analyze_file_sum
 * 功能：统计单个文件并累加到全局统计结构体。
 * 输入：filepath - 文件路径，stat - 全局统计结构体指针
 */
void analyze_file_sum(const char *filepath, struct GlobalStat *stat) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) return;
    char buf[4096];
    while (fgets(buf, sizeof(buf), fp)) {
        stat->lines++;
        stat->chars += strlen(buf);
        char *p = buf;
        while (*p) {
            while (*p && !isalnum(*p)) p++;
            char word[MAX_WORD]; int idx = 0;
            while (*p && isalnum(*p)) {
                word[idx++] = tolower(*p);
                p++;
            }
            if (idx > 0) {
                word[idx] = '\0';
                add_word_global(stat, word);
                stat->words++;
            }
        }
    }
    fclose(fp);
}

/*
 * is_text_file
 * 功能：判断文件名是否为文本文件（.txt/.md/.log）。
 * 输入：filename - 文件名
 * 返回：1为文本文件，0为非文本文件
 */
int is_text_file(const char *filename) {
    const char *ext = strrchr(filename, '.');
    if (!ext) return 0;
    return strcmp(ext, ".txt") == 0 || strcmp(ext, ".md") == 0 || strcmp(ext, ".log") == 0;
}

/*
 * output_global_report
 * 功能：输出全局统计报告。
 * 输入：stat - 全局统计结构体指针，show_report - 是否输出到文件，report_path - 报告文件路径
 */
void output_global_report(const struct GlobalStat *stat, int show_report, const char *report_path) {
    // struct WordFreq sorted[WORD_TABLE_SIZE]; ! 当WORD_TABLE_SIZE过大时会Segmentation fault (core dumped)
    struct WordFreq *sorted = malloc(sizeof(struct WordFreq) * WORD_TABLE_SIZE);
    if (!sorted) {
        printf("内存分配失败\n");
        return;
    }
    memcpy(sorted, stat->word_table, sizeof(struct WordFreq) * stat->word_table_size);
    qsort(sorted, stat->word_table_size, sizeof(struct WordFreq), cmp_word);
    // 始终输出到文件（如有）
    if (show_report && report_path) {
        FILE *out = fopen(report_path, "w");
        if (out) {
            fprintf(out, "==============================\n");
            fprintf(out, "  Text Analysis Summary Report\n");
            fprintf(out, "==============================\n");
            fprintf(out, "Total characters : %d\n", stat->chars);
            fprintf(out, "Total words      : %d\n", stat->words);
            fprintf(out, "Total lines      : %d\n", stat->lines);
            fprintf(out, "------------------------------\n");
            fprintf(out, "Top %d frequent words:\n", TOP_N);
            fprintf(out, "+----------------------+--------+\n");
            fprintf(out, "| Word                | Count  |\n");
            fprintf(out, "+----------------------+--------+\n");
            for (int i = 0; i < TOP_N && i < stat->word_table_size; ++i) {
                fprintf(out, "| %-20s | %-6d |\n", sorted[i].word, sorted[i].count);
            }
            fprintf(out, "+----------------------+--------+\n");
            fprintf(out, "==============================\n");
            fclose(out);
        }
    }
    // 始终高亮打印到终端
    printf("==============================\n");
    printf("  Text Analysis Summary Report\n");
    printf("==============================\n");
    printf("Total characters : %d\n", stat->chars);
    printf("Total words      : %d\n", stat->words);
    printf("Total lines      : %d\n", stat->lines);
    printf("------------------------------\n");
    printf("Top %d frequent words:\n", TOP_N);
    printf("+----------------------+--------+\n");
    printf("| Word                | Count  |\n");
    printf("+----------------------+--------+\n");
    for (int i = 0; i < TOP_N && i < stat->word_table_size; ++i) {
        printf("| ");
        print_highlighted_word(sorted[i].word, "yellow");
        int len = strlen(sorted[i].word);
        for (int j = len; j < 20; ++j) printf(" ");
        printf(" | %-6d |\n", sorted[i].count);
    }
    printf("+----------------------+--------+\n");
    printf("==============================\n");
    free(sorted);
}

/*
 * analyze_dir
 * 功能：递归统计目录下所有文本文件，输出总报告。
 * 输入：dirpath - 目录路径，show_report - 是否输出到文件，report_path - 报告文件路径
 */
void analyze_dir(const char *dirpath, int show_report, const char *report_path) {
    struct GlobalStat stat = {0};
    DIR *dir = opendir(dirpath);
    if (!dir) { printf("无法打开目录: %s\n", dirpath); return; }
    struct dirent *entry;
    char path[1024];
    while ((entry = readdir(dir))) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
            snprintf(path, sizeof(path), "%s/%s", dirpath, entry->d_name);
            analyze_dir(path, 0, NULL); // 递归子目录，统计在全局stat中
        } else if (is_text_file(entry->d_name)) {
            snprintf(path, sizeof(path), "%s/%s", dirpath, entry->d_name);
            analyze_file_sum(path, &stat);
        }
    }
    closedir(dir);
    output_global_report(&stat, show_report, report_path);
}

/*
 * print_usage
 * 功能：输出命令行用法说明。
 */
void print_usage() {
    printf("用法: tanat [-p 文件路径] [-o 输出报告路径] [-c old new] [-h color] [-d 文件1 文件2] [-r 目录路径]\n");
}

/*
 * replace_in_dir
 * 功能：递归替换目录下所有文本文件中的指定字符串。
 * 输入：dirpath - 目录路径，old - 需替换的字符串，newstr - 替换后的字符串
 */
void replace_in_dir(const char *dirpath, const char *old, const char *newstr) {
    DIR *dir = opendir(dirpath);
    if (!dir) { printf("无法打开目录: %s\n", dirpath); return; }
    struct dirent *entry;
    char path[1024];
    while ((entry = readdir(dir))) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
            snprintf(path, sizeof(path), "%s/%s", dirpath, entry->d_name);
            replace_in_dir(path, old, newstr);
        } else if (is_text_file(entry->d_name)) {
            snprintf(path, sizeof(path), "%s/%s", dirpath, entry->d_name);
            printf("[处理文件] %s\n", path);
            replace_in_file(path, old, newstr);
        }
    }
    closedir(dir);
}

/*
 * main
 * 功能：主程序，解析命令行参数并调度各功能。
 */
int main(int argc, char *argv[]) {
    char color[32] = "yellow";
    char *filepath = NULL, *report_path = NULL, *dirpath = NULL;
    int show_report = 0;
    int i = 1;
    char *replace_old = NULL, *replace_new = NULL;
    int do_replace = 0;

    if (argc == 1) {
        analyze_file("input.txt", 0, NULL, color);
        return 0;
    }

    while (i < argc) {
        if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            filepath = argv[++i];
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            report_path = argv[++i];
            show_report = 1;
        } else if (strcmp(argv[i], "-c") == 0 && i + 2 < argc && argv[i+1][0] != '-') {
            // 仅用于替换 old new
            replace_old = argv[++i];
            replace_new = argv[++i];
            do_replace = 1;
        } else if (strcmp(argv[i], "-h") == 0 && i + 1 < argc) {
            strncpy(color, argv[++i], sizeof(color)-1);
            color[sizeof(color)-1] = '\0';
        } else if (strcmp(argv[i], "-d") == 0 && i + 2 < argc) {
            diff_files(argv[i+1], argv[i+2]);
            return 0;
        } else if (strcmp(argv[i], "-r") == 0 && i + 1 < argc) {
            dirpath = argv[++i];
        }
        i++;
    }

    if (dirpath && do_replace) {
        replace_in_dir(dirpath, replace_old, replace_new);
        return 0;
    }
    if (dirpath) {
        analyze_dir(dirpath, show_report, report_path);
        return 0;
    }
    if (filepath && do_replace) {
        replace_in_file(filepath, replace_old, replace_new);
        return 0;
    }
    if (filepath) {
        analyze_file(filepath, show_report, report_path, color);
        return 0;
    }
    print_usage();
    return 0;
}
