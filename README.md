# TANAT

TANAT (Text ANAlyze Toolkit) is a lightweight command-line utility for analyzing and processing text files. It offers a variety of features for statistics, word frequency analysis, content replacement, and more:

### Usage

```
tanat [-p <file path>] [-o <output report path>] [-c <old> <new>] [-d <file1> <file2>] [-r <directory path>]
```

### Function

Default Mode
By default, TANAT analyzes the text content of the current directory, reporting the number of characters, words, lines, and the top 10 most frequent words, which are highlighted directly in the terminal output.

-p Argument: Specify File Path
Allows you to provide a specific file path with -p <file> for targeted analysis.

-o Argument: Output Report to File
Use -o <output_path> to save the analysis report to a specified file instead of (or in addition to) printing to the terminal.

-c Argument: Simple Text Replacement
Enables find-and-replace functionality via -c <old_string> <new_string>, modifying occurrences of a string in the file.

-d Argument: File Difference Comparison
Compares two files using -d <file1> <file2>, similar to tools like diff or vimdiff, and shows content differences line by line.

-r Argument: Recursive Directory Processing
With -r <directory>, TANAT will recursively analyze all text files in the specified directory and generate a cumulative report.

### How to build

```
make
```
