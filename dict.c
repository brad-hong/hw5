#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "dict.h"

struct dict_item {
  // Each word is at most 100 bytes.
  char word[100];
  // The length of each word.
  size_t len;
};

struct dict_t {
  // The path to the underlying file
  char *path;
  // The file descriptor of the opened file. Set by dictionary_open_map.
  int fd;
  // How many items the mmaped file should store (this will determine the size).
  // There are ~500k words in /usr/share/dict/words.
  size_t num_items;
  // A pointer to the first item.
  struct dict_item *base;
};

// Construct a new dict_t struct.
// data_file is where to write the data,
// num_items is how many items this data file should store.
struct dict_t* dictionary_new(char *data_file, size_t num_items) {
    struct dict_t* dict = malloc(sizeof(struct dict_t));
    dict->num_items = num_items;
    dict->path = malloc(strlen(data_file));
    strcpy(dict->path, data_file);
    return dict;
}

// Computes the size of the underlying file based on the # of items and the size
// of a dict_item.
size_t dictionary_len(struct dict_t *dict) {
  return dict->num_items * sizeof(struct dict_item);
}

// This is a private helper function. It should:
// Open the underlying path (dict->path), ftruncate it to the appropriate length
// (dictionary_len), then mmap it.
int dictionary_open_map(struct dict_t *dict) {
    size_t len = dictionary_len(dict);
    int fd = open(dict->path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }
    int fd2 = ftruncate(fd, len);
    if (fd2 == -1) {
         perror("truncate");
         return EXIT_FAILURE;
    }
    dict->base = malloc(len);
    void *mapping = mmap(dict->base, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapping == MAP_FAILED) {
        perror("mmap");
        return EXIT_FAILURE;
    }

    dict->fd = fd;
    return fd;
}

int dictionary_generate(struct dict_t *dict, char *input) {
    char line[100] = { 0 };
    File *file = fopen(input, "r");
    if (file == NULL) {
        fprintf(stderr, "Invalid file %s\n", input);
        return -1;
    } else {
        int count = 0;
        while (fgets(line, 100, file) != NULL && count < dict->num_items) {
            dict_t->base[count].word = line;
            dict_t->base[count].len = strlen(line) - 1;
            count++;
        }
    }
    fclose(fp);
    msync(dict->base, dictionary_len(dict), MS_SYNC);
    return 1;
}

// load a dictionary that was generated by calling generate.
int dictionary_load(struct dict_t *dict) {
    dictionary_open_map(dict);
}

// Unmaps the given dictionary.
// Free/destroy the underlying dict. Does NOT delete the database file.
void dictionary_close(struct dict_t *dict) {
    free(dict->base);
    free(dict);
}

// returns pointer to word if it exists, null otherwise
char* dictionary_exists(struct dict_t *dict, char *word) {
    char* result;
    for (int i = 0; i < dict_t->num_items; i++) {
        result = dict->base[i].word;
        if (strcmp(word, result) == 0) {
            return result;
        }
    }
    return NULL;
}

//// Count of words with len > n
int dictionary_larger_than(struct dict_t *dict, size_t n) {
    int count = 0;
    for (int i = 0; i < dict_t->num_items; i++) {
        int word_length = dict->base[i].len;
        if (word_length > n) {
            count++;
        }
    }
    return count;
}

// Count of words with len < n
int dictionary_smaller_than(struct dict_t *dict, size_t n);
    int count = 0;
    for (int i = 0; i < dict_t->num_items; i++) {
        int word_length = dict->base[i].len;
        if (word_length < n) {
            count++;
        }
    }
    return count;

// Count of words with len == n
int dictionary_equal_to(struct dict_t *dict, size_t n) {
    int count = 0;
    for (int i = 0; i < dict_t->num_items; i++) {
        int word_length = dict->base[i].len;
        if (word_length == n) {
            count++;
        }
    }
    return count;
}

// The rest of the functions should be whatever is left from the header that
// hasn't been defined yet.
// Good luck!
