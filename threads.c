#include "threads.h"

struct thread_data thread_data_array[NUM_THREADS];
pthread_mutex_t pass_mutex;
pthread_cond_t pass_signal;

void bytes2md5(const char *data, int len, char *md5buf)
{
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    const EVP_MD *md = EVP_md5();
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len, i;
    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, data, len);
    EVP_DigestFinal_ex(mdctx, md_value, &md_len);
    EVP_MD_CTX_free(mdctx);
    for (i = 0; i < md_len; i++)
    {
        snprintf(&(md5buf[i * 2]), 16 * 2, "%02x", md_value[i]);
    }
}

void *onewordprod(void *threadarg)
{
    int taskid, range, max_fix = 10;
    struct thread_data *my_data;
    my_data = (struct thread_data *)threadarg;
    taskid = my_data->thread_id;
    range = my_data->length;
    // wprowadzono kilka buforow do danych sposobow dobierania hasla aby bylo latwiej je odczytac
    char buf[33];      // bufor do haszowania hasła
    char fixed[33];    // bufor na haslo z pre- i post- fixami
    char first_up[33]; // bufor na haslo z pierwsza duza litera
    char all_up[33];   // bufor na haslo ze wszystkimi duzymi literami
    char prefix[1000000];
    char postfix[1000000];
    char buf_dict[33]; // bufor na haslo ze slownika
    char temp[33];     // zmienna tymczasowa
    int l;
    switch (taskid)
    {
    case 1:
        /**************************************************************************************/
        // watki 1 i 2 najpierw sprawdzaja hasla ze slownika po polowie, nastepnie badaja post- i pre- fixy, watek 3 od poczatku bada hasla z oboma, post i pre fixami
        for (int i = 0; i < (PASS_AMOUNT / 2); i++)
        {
            for (int j = 0; j <= range; j++)
            {
                bytes2md5(dict_array[j], strlen(dict_array[j]), buf);

                if (user_array[i].guessed == false && strcmp(buf, user_array[i].hash_pwd) == 0)
                {
                    strcpy(user_array[i].password, dict_array[j]);
                    pthread_mutex_lock(&pass_mutex);

                    for (int ar = 0; ar < PASS_AMOUNT; ar++)
                    {
                        if (guessed_indexes[ar] == -1) // wpisuje na pierwsze wolne miejsce tablicy z indekasmi, numer odgadnietego hasla
                        {
                            guessed_indexes[ar] = i;
                            break;
                        }
                    }
                    pthread_cond_signal(&pass_signal);
                    pthread_mutex_unlock(&pass_mutex);
                    user_array[i].guessed = true;
                    break;
                }

                memset(all_up, 0, 33); // czyszcze bufory przed uzyciem
                memset(temp, 0, 33);
                strcpy(temp, dict_array[j]);
                for (int n = 0; n < strlen(dict_array[j]); n++) // haslo z samymi duzymi literami
                {
                    all_up[n] = toupper(dict_array[j][n]);
                }
                bytes2md5(all_up, strlen(all_up), buf);

                if (user_array[i].guessed == false && strcmp(buf, user_array[i].hash_pwd) == 0)
                {
                    strcpy(user_array[i].password, all_up);
                    pthread_mutex_lock(&pass_mutex);

                    for (int ar = 0; ar < PASS_AMOUNT; ar++)
                    {
                        if (guessed_indexes[ar] == -1) // wpisuje na pierwsze wolne miejsce tablicy z indekasmi, numer odgadnietego hasla
                        {
                            guessed_indexes[ar] = i;
                            break;
                        }
                    }
                    pthread_cond_signal(&pass_signal);
                    pthread_mutex_unlock(&pass_mutex);
                    user_array[i].guessed = true;
                    break;
                }

                memset(first_up, 0, 33); // czyszcze bufory przed uzyciem
                memset(temp, 0, 33);
                strcpy(temp, dict_array[j]);
                strcpy(first_up, temp);
                first_up[0] = toupper(first_up[0]); // tylko pierwsza litera duza
                bytes2md5(first_up, strlen(first_up), buf);
                if (user_array[i].guessed == false && strcmp(buf, user_array[i].hash_pwd) == 0)
                {
                    strcpy(user_array[i].password, first_up);
                    pthread_mutex_lock(&pass_mutex);

                    for (int ar = 0; ar < PASS_AMOUNT; ar++)
                    {
                        if (guessed_indexes[ar] == -1) // wpisuje na pierwsze wolne miejsce tablicy z indekasmi, numer odgadnietego hasla
                        {
                            guessed_indexes[ar] = i;
                            break;
                        }
                    }
                    pthread_cond_signal(&pass_signal);
                    pthread_mutex_unlock(&pass_mutex);
                    user_array[i].guessed = true;
                    break;
                }
            }
        }
        /**************************************************************************************/
        // prefixy
        for (int k = 0; k <= max_fix; k++)
        {
            for (l = 0; l < PASS_AMOUNT; l++)
            {
                for (int m = 0; m < range; m++)
                {
                    sprintf(prefix, "%d", k);
                    strcpy(fixed, strcat(prefix, dict_array[m]));
                    bytes2md5(fixed, strlen(fixed), buf); // haslo z prefixem i samymi malymi literami
                    if (user_array[l].guessed == false && strcmp(buf, user_array[l].hash_pwd) == 0)
                    {
                        strcpy(user_array[l].password, fixed);
                        pthread_mutex_lock(&pass_mutex);

                        for (int ar = 0; ar < PASS_AMOUNT; ar++) // wpisuje na pierwsze wolne miejsce tablicy z indekasmi, numer odgadnietego hasla
                        {
                            if (guessed_indexes[ar] == -1)
                            {
                                guessed_indexes[ar] = l;
                                break;
                            }
                        }
                        pthread_cond_signal(&pass_signal);
                        pthread_mutex_unlock(&pass_mutex);
                        user_array[l].guessed = true;
                        break;
                    }

                    memset(all_up, 0, 33); // czyszcze bufory przed uzyciem
                    memset(temp, 0, 33);
                    strcpy(temp, dict_array[m]);
                    for (int n = 0; n < strlen(dict_array[m]); n++) // haslo z samymi duzymi literami
                    {
                        all_up[n] = toupper(dict_array[m][n]);
                    }
                    bytes2md5(all_up, strlen(all_up), buf);
                    if (user_array[l].guessed == false && strcmp(buf, user_array[l].hash_pwd) == 0)
                    {
                        strcpy(user_array[l].password, all_up);
                        pthread_mutex_lock(&pass_mutex);

                        for (int ar = 0; ar < PASS_AMOUNT; ar++) // wpisuje na pierwsze wolne miejsce tablicy z indekasmi, numer odgadnietego hasla
                        {
                            if (guessed_indexes[ar] == -1)
                            {
                                guessed_indexes[ar] = l;
                                break;
                            }
                        }
                        pthread_cond_signal(&pass_signal);
                        pthread_mutex_unlock(&pass_mutex);
                        user_array[l].guessed = true;
                        break;
                    }

                    memset(first_up, 0, 33); // czyszcze bufory przed uzyciem
                    memset(temp, 0, 33);
                    strcpy(temp, dict_array[m]);
                    strcpy(first_up, temp);
                    for (int n = 0; n < strlen(fixed); n++) // haslo z prefixem i pierwsza duza litera
                    {
                        if (isdigit(fixed[n]) == 0)
                        {
                            first_up[n] = toupper(fixed[n]);
                            break;
                        }
                    }
                    bytes2md5(first_up, strlen(first_up), buf);
                    if (user_array[l].guessed == false && strcmp(buf, user_array[l].hash_pwd) == 0)
                    {
                        strcpy(user_array[l].password, first_up);
                        pthread_mutex_lock(&pass_mutex);

                        for (int ar = 0; ar < PASS_AMOUNT; ar++) // wpisuje na pierwsze wolne miejsce tablicy z indekasmi, numer odgadnietego hasla
                        {
                            if (guessed_indexes[ar] == -1)
                            {
                                guessed_indexes[ar] = l;
                                break;
                            }
                        }
                        pthread_cond_signal(&pass_signal);
                        pthread_mutex_unlock(&pass_mutex);
                        user_array[l].guessed = true;
                        break;
                    }
                }
            }
            if (k == max_fix && max_fix < 1000000)
            {
                max_fix = max_fix * 10;
            }
        }

        /**************************************************************************************/
        break;
    case 2:
        for (int i = (PASS_AMOUNT / 2); i < PASS_AMOUNT; i++)
        {
            for (int j = 0; j <= range; j++)
            {
                bytes2md5(dict_array[j], strlen(dict_array[j]), buf);

                if (user_array[i].guessed == false && strcmp(buf, user_array[i].hash_pwd) == 0)
                {
                    strcpy(user_array[i].password, dict_array[j]);
                    pthread_mutex_lock(&pass_mutex);

                    for (int ar = 0; ar < PASS_AMOUNT; ar++) // wpisuje na pierwsze wolne miejsce tablicy z indekasmi, numer odgadnietego hasla
                    {
                        if (guessed_indexes[ar] == -1)
                        {
                            guessed_indexes[ar] = i;
                            break;
                        }
                    }
                    pthread_cond_signal(&pass_signal);
                    pthread_mutex_unlock(&pass_mutex);
                    user_array[i].guessed = true;
                    break;
                }

                memset(all_up, 0, 33); // czyszcze bufory przed uzyciem
                memset(temp, 0, 33);
                strcpy(temp, dict_array[j]);
                for (int n = 0; n < strlen(dict_array[j]); n++) // haslo z samymi duzymi literami
                {
                    all_up[n] = toupper(dict_array[j][n]);
                }
                bytes2md5(all_up, strlen(all_up), buf);

                if (user_array[i].guessed == false && strcmp(buf, user_array[i].hash_pwd) == 0)
                {
                    strcpy(user_array[i].password, all_up);
                    pthread_mutex_lock(&pass_mutex);

                    for (int ar = 0; ar < PASS_AMOUNT; ar++)
                    {
                        if (guessed_indexes[ar] == -1)
                        {
                            guessed_indexes[ar] = i;
                            break;
                        }
                    }
                    pthread_cond_signal(&pass_signal);
                    pthread_mutex_unlock(&pass_mutex);
                    user_array[i].guessed = true;
                    break;
                }

                memset(first_up, 0, 33); // czyszcze bufory przed uzyciem
                memset(temp, 0, 33);
                strcpy(temp, dict_array[j]);
                strcpy(first_up, temp);
                first_up[0] = toupper(first_up[0]); // tylko pierwsza litera duza
                bytes2md5(first_up, strlen(first_up), buf);
                if (user_array[i].guessed == false && strcmp(buf, user_array[i].hash_pwd) == 0)
                {
                    strcpy(user_array[i].password, first_up);
                    pthread_mutex_lock(&pass_mutex);

                    for (int ar = 0; ar < PASS_AMOUNT; ar++) // wpisuje na pierwsze wolne miejsce tablicy z indekasmi, numer odgadnietego hasla
                    {
                        if (guessed_indexes[ar] == -1)
                        {
                            guessed_indexes[ar] = i;
                            break;
                        }
                    }
                    pthread_cond_signal(&pass_signal);
                    pthread_mutex_unlock(&pass_mutex);
                    user_array[i].guessed = true;
                    break;
                }
            }
        }

        /**************************************************************************************/
        // postfixy
        for (int k = 0; k <= max_fix; k++)
        {
            sprintf(postfix, "%d", k);
            for (l = 0; l < PASS_AMOUNT; l++)
            {

                for (int m = 0; m <= range; m++)
                {
                    strcpy(buf_dict, dict_array[m]);
                    strcpy(fixed, strcat(buf_dict, postfix));
                    bytes2md5(fixed, strlen(fixed), buf);

                    if (user_array[l].guessed == false && strcmp(buf, user_array[l].hash_pwd) == 0)
                    {
                        strcpy(user_array[l].password, fixed);
                        pthread_mutex_lock(&pass_mutex);

                        for (int ar = 0; ar < PASS_AMOUNT; ar++) // wpisuje na pierwsze wolne miejsce tablicy z indekasmi, numer odgadnietego hasla
                        {
                            if (guessed_indexes[ar] == -1)
                            {
                                guessed_indexes[ar] = l;
                                break;
                            }
                        }
                        pthread_cond_signal(&pass_signal);
                        pthread_mutex_unlock(&pass_mutex);
                        user_array[l].guessed = true;
                        break;
                    }

                    memset(all_up, 0, 33); // czyszcze bufory przed uzyciem
                    memset(temp, 0, 33);
                    strcpy(temp, fixed);
                    for (int n = 0; n < strlen(fixed); n++) // haslo z samymi duzymi literami
                    {
                        all_up[n] = toupper(fixed[n]);
                    }
                    bytes2md5(all_up, strlen(all_up), buf);

                    if (user_array[l].guessed == false && strcmp(buf, user_array[l].hash_pwd) == 0)
                    {
                        strcpy(user_array[l].password, all_up);
                        pthread_mutex_lock(&pass_mutex);

                        for (int ar = 0; ar < PASS_AMOUNT; ar++)
                        {
                            if (guessed_indexes[ar] == -1)
                            {
                                guessed_indexes[ar] = l;
                                break;
                            }
                        }
                        pthread_cond_signal(&pass_signal);
                        pthread_mutex_unlock(&pass_mutex);
                        user_array[l].guessed = true;
                        break;
                    }

                    memset(first_up, 0, 33); // czyszcze bufory przed uzyciem
                    memset(temp, 0, 33);
                    strcpy(first_up, fixed);
                    first_up[0] = toupper(first_up[0]); // tylko pierwsza litera duza
                    bytes2md5(first_up, strlen(first_up), buf);
                    if (user_array[l].guessed == false && strcmp(buf, user_array[l].hash_pwd) == 0)
                    {
                        strcpy(user_array[l].password, first_up);
                        pthread_mutex_lock(&pass_mutex);

                        for (int ar = 0; ar < PASS_AMOUNT; ar++) // wpisuje na pierwsze wolne miejsce tablicy z indekasmi, numer odgadnietego hasla
                        {
                            if (guessed_indexes[ar] == -1)
                            {
                                guessed_indexes[ar] = l;
                                break;
                            }
                        }
                        pthread_cond_signal(&pass_signal);
                        pthread_mutex_unlock(&pass_mutex);
                        user_array[l].guessed = true;
                        break;
                    }
                }
                strcpy(buf_dict, ""); // czyszcze zmienna tymczasowa, aby nie przechowywala starych slow
            }
            if (k == max_fix && max_fix < 1000000)
            {
                max_fix = max_fix * 10;
            }
        }
        /**************************************************************************************/

        break;
    case 3:
        // post- i pre- fixy
        for (int i = 0; i <= max_fix; i++)
        {
            for (l = 0; l < max_fix; l++)
            {
                sprintf(postfix, "%d", l);
                for (int j = 0; j < PASS_AMOUNT; j++)
                {
                    for (int k = 0; k <= range; k++)
                    {

                        sprintf(prefix, "%d", i);
                        strcpy(fixed, strcat(prefix, dict_array[k]));

                        strcpy(buf_dict, fixed);
                        strcpy(fixed, strcat(buf_dict, postfix));
                        bytes2md5(fixed, strlen(fixed), buf);

                        if (user_array[j].guessed == false && strcmp(buf, user_array[j].hash_pwd) == 0)
                        {
                            strcpy(user_array[j].password, fixed);
                            pthread_mutex_lock(&pass_mutex);

                            for (int ar = 0; ar < PASS_AMOUNT; ar++) // wpisuje na pierwsze wolne miejsce tablicy z indekasmi, numer odgadnietego hasla
                            {
                                if (guessed_indexes[ar] == -1)
                                {
                                    guessed_indexes[ar] = j;
                                    break;
                                }
                            }
                            pthread_cond_signal(&pass_signal);
                            pthread_mutex_unlock(&pass_mutex);
                            user_array[j].guessed = true;
                            break;
                        }

                        memset(all_up, 0, 33); // czyszcze bufory przed uzyciem
                        memset(temp, 0, 33);
                        strcpy(temp, fixed);
                        for (int n = 0; n < strlen(fixed); n++) // haslo z samymi duzymi literami
                        {
                            all_up[n] = toupper(fixed[n]);
                        }
                        bytes2md5(all_up, strlen(all_up), buf);

                        if (user_array[j].guessed == false && strcmp(buf, user_array[j].hash_pwd) == 0)
                        {
                            strcpy(user_array[j].password, all_up);
                            pthread_mutex_lock(&pass_mutex);

                            for (int ar = 0; ar < PASS_AMOUNT; ar++) // wpisuje na pierwsze wolne miejsce tablicy z indekasmi, numer odgadnietego hasla
                            {
                                if (guessed_indexes[ar] == -1)
                                {
                                    guessed_indexes[ar] = j;
                                    break;
                                }
                            }
                            pthread_cond_signal(&pass_signal);
                            pthread_mutex_unlock(&pass_mutex);
                            user_array[j].guessed = true;
                            break;
                        }

                        memset(first_up, 0, 33); // czyszcze bufory przed uzyciem
                        memset(temp, 0, 33);
                        strcpy(first_up, fixed);
                        for (int n = 0; n < strlen(fixed); n++) // haslo z prefixem i pierwsza duza litera
                        {
                            if (isdigit(fixed[n]) == 0)
                            {
                                first_up[n] = toupper(fixed[n]);
                                break;
                            }
                        }
                        bytes2md5(first_up, strlen(first_up), buf);
                        if (user_array[j].guessed == false && strcmp(buf, user_array[j].hash_pwd) == 0)
                        {
                            strcpy(user_array[j].password, first_up);
                            pthread_mutex_lock(&pass_mutex);

                            for (int ar = 0; ar < PASS_AMOUNT; ar++) // wpisuje na pierwsze wolne miejsce tablicy z indekasmi, numer odgadnietego hasla
                            {
                                if (guessed_indexes[ar] == -1)
                                {
                                    guessed_indexes[ar] = j;
                                    break;
                                }
                            }
                            pthread_cond_signal(&pass_signal);
                            pthread_mutex_unlock(&pass_mutex);
                            user_array[j].guessed = true;
                            break;
                        }
                    }
                    strcpy(buf_dict, ""); // czyszcze bufor, aby nie przechowywal starych slow
                }
            }
            if (i == max_fix && max_fix < 1000000)
            {
                max_fix = max_fix * 10;
                l = 0;
            }
        }
        break;
    default:
        break;
    }
    pthread_exit(NULL);
}

/**************************************************************************************/
void *twowordprod(void *threadarg)
{
    int taskid, range, space_pos;
    struct thread_data *my_data;
    my_data = (struct thread_data *)threadarg;
    taskid = my_data->thread_id;
    range = my_data->length;

    char buf[50];
    char buf_dict1[50];
    char buf_dict2[50];
    char buf_dict3[50];
    int counter;
    switch (taskid)
    {
    case 4:
        for (int i = 0; i < PASS_AMOUNT; i++)
        {
            for (int j = 0; j <= (int)(range * 0.333); j++)
            {

                memset(buf_dict1, 0, 50); // czyszcze bufory przed uzyciem
                strcpy(buf_dict1, dict_array[j]);
                strcat(buf_dict1, " ");
                for (int k = 0; k <= range; k++)
                {
                    memset(buf_dict3, 0, 50);
                    memset(buf_dict2, 0, 50);
                    strcpy(buf_dict2, buf_dict1);
                    strcpy(buf_dict3, dict_array[k]);
                    strcat(buf_dict2, buf_dict3);
                    bytes2md5(buf_dict2, strlen(buf_dict2), buf);
                    if (user_array[i].guessed == false && strcmp(buf, user_array[i].hash_pwd) == 0)
                    {
                        strcpy(user_array[i].password, buf_dict2);
                        pthread_mutex_lock(&pass_mutex);

                        for (int ar = 0; ar < PASS_AMOUNT; ar++) // wpisuje na pierwsze wolne miejsce tablicy z indekasmi, numer odgadnietego hasla
                        {
                            if (guessed_indexes[ar] == -1)
                            {
                                guessed_indexes[ar] = i;
                                break;
                            }
                        }
                        pthread_cond_signal(&pass_signal);
                        pthread_mutex_unlock(&pass_mutex);
                        user_array[i].guessed = true;
                        break;
                    }
                }
            }
        }
        break;
        /**************************************************************************************/

    case 5:
        for (int i = 0; i < PASS_AMOUNT; i++)
        {
            for (int j = (int)(range * 0.333); j <= (int)(range * 0.666); j++)
            {

                memset(buf_dict1, 0, 50); // czyszcze bufory przed uzyciem
                strcpy(buf_dict1, dict_array[j]);
                strcat(buf_dict1, " ");

                for (int k = 0; k <= range; k++)
                {
                    memset(buf_dict3, 0, 50);
                    memset(buf_dict2, 0, 50);
                    strcpy(buf_dict2, buf_dict1);
                    strcpy(buf_dict3, dict_array[k]);
                    strcat(buf_dict2, buf_dict3);
                    bytes2md5(buf_dict2, strlen(buf_dict2), buf);

                    if (user_array[i].guessed == false && strcmp(buf, user_array[i].hash_pwd) == 0)
                    {
                        strcpy(user_array[i].password, buf_dict2);
                        pthread_mutex_lock(&pass_mutex);

                        for (int ar = 0; ar < PASS_AMOUNT; ar++) // wpisuje na pierwsze wolne miejsce tablicy z indekasmi, numer odgadnietego hasla
                        {
                            if (guessed_indexes[ar] == -1)
                            {
                                guessed_indexes[ar] = i;
                                break;
                            }
                        }
                        pthread_cond_signal(&pass_signal);
                        pthread_mutex_unlock(&pass_mutex);
                        user_array[i].guessed = true;
                        break;
                    }
                }
            }
        }
        break;
        /**************************************************************************************/

    case 6:
        for (int i = 0; i < PASS_AMOUNT; i++)
        {
            for (int j = (int)(range * 0.666); j < range; j++)
            {

                memset(buf_dict1, 0, 50); // czyszcze bufory przed uzyciem
                strcpy(buf_dict1, dict_array[j]);
                strcat(buf_dict1, " ");

                for (int k = 0; k <= range; k++)
                {
                    memset(buf_dict3, 0, 50);
                    memset(buf_dict2, 0, 50);
                    strcpy(buf_dict2, buf_dict1);
                    strcpy(buf_dict3, dict_array[k]);
                    strcat(buf_dict2, buf_dict3);
                    bytes2md5(buf_dict2, strlen(buf_dict2), buf);
                    if (user_array[i].guessed == false && strcmp(buf, user_array[i].hash_pwd) == 0)
                    {
                        strcpy(user_array[i].password, buf_dict2);
                        pthread_mutex_lock(&pass_mutex);

                        for (int ar = 0; ar < PASS_AMOUNT; ar++) // wpisuje na pierwsze wolne miejsce tablicy z indekasmi, numer odgadnietego hasla
                        {
                            if (guessed_indexes[ar] == -1)
                            {
                                guessed_indexes[ar] = i;
                                break;
                            }
                        }
                        pthread_cond_signal(&pass_signal);
                        pthread_mutex_unlock(&pass_mutex);
                        user_array[i].guessed = true;
                        break;
                    }
                }
            }
        }
        break;
    default:
        break;
    }
    pthread_exit(NULL);
}

void handle_sighup(int signo) // funkcja wywolywana przy otrzymaniu sygnalu sighup - wyswietla ile odgadnieto hasel
{
    int index, counter = 0;
    pthread_mutex_lock(&pass_mutex);
    for (int i = 0; i < PASS_AMOUNT; i++)
    {
        if (guessed_indexes[i] != -1)
            counter++;
        else
            break;
    }
    pthread_mutex_unlock(&pass_mutex);
    printf("Odgadnieto w sumie %d hasel\n", counter);
}

void *client(void *threadarg)
{
    signal(SIGHUP, handle_sighup);
    int taskid;
    struct thread_data *my_data;
    my_data = (struct thread_data *)threadarg;
    taskid = my_data->thread_id;

    int count = 0, index = 0;
    pthread_mutex_lock(&pass_mutex);
    while (count < PASS_AMOUNT)
    {
        pthread_cond_wait(&pass_signal, &pass_mutex); // czeka na sygnal od watkow, jestli otrzyma wypisuje haslo wpisana najdalej w tablicy, to jest ostatnie otrzymane

        for (int i = 0; i < PASS_AMOUNT; i++)
        {
            if ((i + 1) != PASS_AMOUNT)
            {
                if (guessed_indexes[i] != -1 && guessed_indexes[i + 1] == -1)
                {
                    index = guessed_indexes[i];
                    printf("Odgadnięte hasło:\n %s %s %s %s -> %s\n", user_array[index].id, user_array[index].hash_pwd, user_array[index].mail, user_array[index].nick, user_array[index].password);
                    break;
                }
            }
        }

        count++;
    }
    pthread_mutex_lock(&pass_mutex);
    pthread_exit(NULL);
}