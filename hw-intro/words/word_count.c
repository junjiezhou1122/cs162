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

word_count provides lists of words and associated count

Functional methods take the head of a list as first arg.
Mutators take a reference to a list as first arg.
*/

#include "word_count.h"
#include <string.h>

/* Basic utilities */

char *new_string(char *str) {
  char *new_str = (char *) malloc(strlen(str) + 1);
  if (new_str == NULL) {
    return NULL;
  }
  return strcpy(new_str, str);
}

int init_words(WordCount **wclist) {
  if (wclist == NULL) {
    return 1;  // Error: invalid input
  }
  *wclist = NULL;
  return 0;
}

ssize_t len_words(WordCount *wchead) {
  /* Return -1 if any errors are
     encountered in the body of
     this function.
  */
    size_t len = 0;
    WordCount *p = wchead;

    while (p != NULL) {
      len++;
      p = p -> next;
    }

    return len;
}

WordCount *find_word(WordCount *wchead, char *word) {
  if (wchead == NULL || word == NULL) {
    return NULL;
  }
  WordCount *p = wchead;
  while (p != NULL) {
    if (p->word != NULL && strcmp(p->word, word) == 0) {
      return p;
    }
    p = p->next;
  }
  return NULL;
}

int add_word(WordCount **wclist, char *word) {
    if (strlen(word) < 2) {
      return 0;
    }

    if (wclist == NULL || word == NULL) {
        return 1;  // Error: invalid input
    }

    WordCount *wc = find_word(*wclist, word);
    if (wc != NULL) {
        wc->count++;
        return 0;
    }

    // Word not found, create a new WordCount node
    WordCount *new_wc = malloc(sizeof(WordCount));
    if (new_wc == NULL) {
        return 1;  // Memory allocation failed
    }

    new_wc->word = strdup(word);  // Duplicate the word to avoid issues with static buffers
    if (new_wc->word == NULL) {
        free(new_wc);
        return 1;  // Memory allocation failed
    }

    new_wc->count = 1;

    // Insert the new node at the beginning of the list
    new_wc->next = *wclist;
    *wclist = new_wc;

    return 0;
}

void fprint_words(WordCount *wchead, FILE *ofile) {
  /* print word counts to a file */
  WordCount *wc;
  for (wc = wchead; wc; wc = wc->next) {
    fprintf(ofile, "%i\t%s\n", wc->count, wc->word);
  }
}
