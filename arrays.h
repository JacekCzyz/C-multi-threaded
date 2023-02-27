#pragma once
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <signal.h>
#include <stdlib.h>

#define NUM_THREADS 7
#define PASS_AMOUNT 182
#define MAX_PWD_NAME_LENGTH 40
#define MAX_HASH 33

struct user
{
    char id[4];
    char hash_pwd[MAX_HASH];
    char nick[MAX_PWD_NAME_LENGTH];
    char mail[MAX_PWD_NAME_LENGTH];
    char password[MAX_PWD_NAME_LENGTH];
    bool guessed;
};

extern struct user user_array[]; // globalna tablica z danymi uzytkownikow
extern char **dict_array;
extern int guessed_indexes[];

int digits_only(const char *s);

int dict_length(const char *fname);

void fill_users(struct user users[PASS_AMOUNT], const char *fname);

void make_dictionary(char **array, int line_num, FILE *dictfile);
