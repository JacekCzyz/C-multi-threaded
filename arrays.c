#include "arrays.h"

struct user user_array[PASS_AMOUNT]; // globalna tablica z danymi uzytkownikow
char **dict_array;
int guessed_indexes[PASS_AMOUNT];

int digits_only(const char *s)
{
    while (*s)
    {
        if (isdigit(*s++) == 0)
            return 0;
    }
    return 1;
}

int dict_length(const char *fname)
{
    FILE *infile = fopen(fname, "r");
    int counter = 0;
    char c;
    for (c = getc(infile); c != EOF; c = getc(infile))
    {
        if (c == '\n')
            counter++;
    }
    fclose(infile);
    return counter;
}


void fill_users(struct user users[PASS_AMOUNT], const char *fname)
{
    FILE *users_file = fopen(fname, "r");
    int size = dict_length(fname);
    char buf[MAX_HASH];
    int counter = 0, j = -1;
    while (fscanf(users_file, "%1023s", buf) == 1)
    {
        if (counter == 0)
        {
            if (digits_only(buf) == 1 && strlen(buf) == 3)
            {
                j++;
                users[j].guessed = false;
                strcpy(users[j].id, buf);
                counter++;
            }
            else
            {
                strcpy(users[j].nick, strcat(users[j].nick, " "));
                strcpy(users[j].nick, strcat(users[j].nick, buf));
                counter = 0;
            }
        }
        else if (counter == 1)
        {
            strcpy(users[j].hash_pwd, buf);
            counter++;
        }
        else if (counter == 2)
        {
            if (digits_only(buf) == 1 && strlen(buf) == 3)
            {
                j++;
                users[j].guessed = false;
                strcpy(users[j].id, buf);
                counter = 1;
            }
            else
            {
                strcpy(users[j].mail, buf);
                counter++;
            }
        }
        else if (counter == 3)
        {
            if (digits_only(buf) == 1 && strlen(buf) == 3)
            {
                j++;
                users[j].guessed = false;
                strcpy(users[j].id, buf);
                counter = 1;
            }
            else
            {
                strcpy(users[j].nick, buf);
                counter = 0;
            }
        }
    }
    fclose(users_file);
}

void make_dictionary(char **array, int line_num, FILE *dictfile)
{
    for (int i = 0; i <= line_num; i++)
    {
        array[i] = (char *)malloc(MAX_PWD_NAME_LENGTH);
    }

    char buf[MAX_PWD_NAME_LENGTH];
    int j = 0;
    while (fscanf(dictfile, "%1023s", buf) == 1)
    {
        strcpy(array[j], buf);
        j++;
    }
}