/*

  Word Count using dedicated lists

*/

/*
Copyright © 2019 University of California, Berkeley

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <assert.h>
#include <getopt.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

#include "word_count.h"

int extract_word(WordCount **wclist, FILE *infile);
char *get_word(FILE* infile);

/* Global data structure tracking the words encountered */
WordCount *word_counts = NULL;

/* The maximum length of each word in a file */
#define MAX_WORD_LEN 64

/*
 * 3.1.1 Total Word Count
 *
 * Returns the total amount of words found in infile.
 * Useful functions: fgetc(), isalpha().
 */
int num_words(FILE* infile) {
    int num_words = 0;
    int ch;
    int char_count = 0;
    bool in_word = false;
    FILE* input = (infile == NULL) ? stdin : infile;

    while ((ch = fgetc(input)) != EOF) {
        if (isalpha(ch)) {
            char_count++;
            in_word = true;
        } else {
            if (in_word && char_count > 1) {
                num_words++;
            }
            in_word = false;
            char_count = 0;
        }
    }

    // Handle case where file ends with a word
    if (in_word && char_count > 1) {
        num_words++;
    }

    // Clear any error flags on stdin
    if (input == stdin) {
        clearerr(stdin);
    }

    return num_words;
}

/*
 * 3.1.2 Word Frequency Count
 *
 * Given infile, extracts and adds each word in the FILE to `wclist`.
 * Useful functions: fgetc(), isalpha(), tolower(), add_word().
 * 
 * As mentioned in the spec, your code should not panic or
 * segfault on errors. Thus, this function should return
 * 1 in the event of any errors (e.g. wclist or infile is NULL)
 * and 0 otherwise.
 */
int count_words(WordCount **wclist, FILE *infile) {
    if (wclist == NULL || infile == NULL) {
        return 1;
    }
    return extract_word(wclist, infile);
}

int extract_word(WordCount **wclist, FILE *infile) {
    if (wclist == NULL || infile == NULL) {
        return 1;  // Error: invalid input
    }
    char* word;
    while((word = get_word(infile)) != NULL) {
        if (add_word(wclist, word) != 0) {
            return 1;  // Error occurred while adding word
        }
    }
    return 0;
}

char *get_word(FILE* infile) {
    static char word[MAX_WORD_LEN + 1];  // +1 for null terminator
    int c;
    int len = 0;

    // Skip non-alphabetic characters
    while ((c = fgetc(infile)) != EOF && !isalpha(c));

    if (c == EOF) {
        return NULL;  // End of file reached
    }

    // Read the word
    do {
        if (len < MAX_WORD_LEN) {
            word[len++] = tolower(c);
        }
        c = fgetc(infile);
    } while (c != EOF && isalpha(c));

    word[len] = '\0';  // Null-terminate the string

    return (len > 0) ? word : NULL;
}
/*
 * Comparator to sort list by frequency.
 * Useful function: strcmp().
 */
static bool wordcount_less(const WordCount *wc1, const WordCount *wc2) {
    if (wc1->count != wc2->count) {
        return wc1->count < wc2->count;
    }
    return strcmp(wc1->word, wc2->word) < 0;
}

// In trying times, displays a helpful message.
static int display_help(void) {
	printf("Flags:\n"
	    "--count (-c): Count the total amount of words in the file, or STDIN if a file is not specified. This is default behavior if no flag is specified.\n"
	    "--frequency (-f): Count the frequency of each word in the file, or STDIN if a file is not specified.\n"
	    "--help (-h): Displays this help message.\n");
	return 0;
}

/*
 * Handle command line flags and arguments.
 */
int main (int argc, char *argv[]) {

  // Count Mode (default): outputs the total amount of words counted
  bool count_mode = true;
 

  // Freq Mode: outputs the frequency of each word
  bool freq_mode = false;


  // Variables for command line argument parsing
  int i;
  static struct option long_options[] =
  {
      {"count", no_argument, 0, 'c'},
      {"frequency", no_argument, 0, 'f'},
      {"help", no_argument, 0, 'h'},
      {0, 0, 0, 0}
  };

  // Sets flags
  while ((i = getopt_long(argc, argv, "cfh", long_options, NULL)) != -1) {
      switch (i) {
          case 'c':
              count_mode = true;
              freq_mode = false;
              break;
          case 'f':
              count_mode = false;
              freq_mode = true;
              break;
          case 'h':
              return display_help();
      }
  }

  if (!count_mode && !freq_mode) {
    printf("Please specify a mode.\n");
    return display_help();
  }

  /* Create the empty data structure */
  if (init_words(&word_counts) != 0) {
      fprintf(stderr, "Failed to initialize word counts\n");
      return 1;
  }

  int total_words = 0;
  FILE *current_file = NULL;

  int process_input(FILE *input, bool is_stdin) {
    if (count_mode) {
        total_words += num_words(input);
    } else if (freq_mode) {
        if (count_words(&word_counts, input) != 0) {
            fprintf(stderr, "Error counting words from %s\n", 
                    is_stdin ? "stdin" : "file");
            return 1;
        }
    }
    return 0;
  }

  if ((argc - optind) < 1) {
    // No input file specified, read from STDIN
    if (process_input(stdin, true) != 0) {
        return 1;
    }
  } else {
    // Process all specified files
    for (int i = optind; i < argc; i++) {
        current_file = fopen(argv[i], "r");
        if (current_file == NULL) {
            fprintf(stderr, "Error opening file: %s\n", argv[i]);
            continue;  // Skip to the next file if there's an error
        }
        
        if (process_input(current_file, false) != 0) {
            fclose(current_file);
            return 1;
        }
        
        fclose(current_file);
    }
  }

  if (count_mode) {
      printf("The total number of words is: %i\n", total_words);
  } else if (freq_mode) {
      wordcount_sort(&word_counts, wordcount_less);
      printf("The frequencies of each word are: \n");
      fprint_words(word_counts, stdout);
  }

  return 0;
}
