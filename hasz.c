#include "threads.h"

int main(int argc, char *argv[])
{

  /**************************************************************************/
  // Inicjalizacja tablic z uzytkownikami i slownikowej
  int dict_lines;
  if (argc == 1)
  {
    FILE *dict_file = fopen("dict.txt", "r");
    dict_lines = dict_length("dict.txt");
    dict_array = malloc(dict_lines * sizeof(char *)); // wyrazenia ze slownika
    make_dictionary(dict_array, dict_lines, dict_file);
    fclose(dict_file);

    fill_users(user_array, "users.txt");
  }
  else if (argc == 3)
  {
    FILE *dict_file = fopen(argv[1], "r");
    dict_lines = dict_length(argv[1]);
    dict_array = malloc(dict_lines * sizeof(char *)); // wyrazenia ze slownika
    make_dictionary(dict_array, dict_lines, dict_file);
    fclose(dict_file);

    fill_users(user_array, argv[2]);
  }
  else
  {
    printf("niepoprawna ilosc argumentow. format to: './a.out' lub './a.out plik_slownik.txt plik_hasla.txt");
  }
  for (int i = 0; i < PASS_AMOUNT; i++)
  {
    guessed_indexes[i] = -1; // uzupełniam tablicę indeksol odgadnietych haseł takimi indeksami z tablicy userow, których na pewno nie ma.
  }

  /**************************************************************************/
  printf("--------------------------------------------------------------------------------------\n");

  pthread_t threads[NUM_THREADS];
  int *taskids[NUM_THREADS];
  int rc, t;
  for (t = 0; t < NUM_THREADS; t++)
  {
    thread_data_array[t].thread_id = t;
    thread_data_array[t].length = dict_lines;
    printf("Creating thread %d\n", t);

    if (t == 0)
      rc = pthread_create(&threads[t], NULL, client, (void *)&thread_data_array[t]);
    else if (t < 4 && t > 0)
      rc = pthread_create(&threads[t], NULL, onewordprod, (void *)&thread_data_array[t]);
    else
      rc = pthread_create(&threads[t], NULL, twowordprod, (void *)&thread_data_array[t]);

    if (rc)
    {
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
  }

  for (int i = 0; i < NUM_THREADS; i++)
  {
    pthread_join(threads[i], NULL);
  }

  pthread_exit(NULL);

  free(dict_array);
  return 0;
}
