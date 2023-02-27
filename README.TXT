Struktura plików:
hasz.c - funkcja main
arrays.c - struktury przechowujące dane i funkcje to ich uzupełniania, deklaracja zmiennych globalnych wykorzystanych w programie
threads.c - struktura przekazywana jako argument do wątków oraz wątki producentów i klientów.

Uwagi dot. kompilacji i uruchamiania:
Kompilacja komendą: gcc hasz.c threads.c arrays.c -lssl -lcrypto -pthread
Uruchamianie komendą: ./a.out (użyte zostaną domyślne pliki ze slownikiem i haslami) lub ./a.out plik_slownik plik_hasla
W przypadku drugiego sposobu uruchamiania należy się upewnić, że ilość wczytywanych jest odpowiednia (zmienna PASS_AMOUNT w pliku arrays.h),
co zapewni optymalne wykorzystanie wszystkich producentów. Dwaj producenci haseł jednowyrazowych oraz producenci haseł składających się z dwóch wyrazów sprawdzają
hasła dla danych części listy użytkowników, na przykład producent jednowyrazowy pierwszy sprawdza dla użytkowników 0-500, a drugi 500-1000. W wypadku kiedy zmienna PASS_AMOUNT
jest zbyt duża w porównaniu do realnej ilości haseł do odgadnięcia, producenci mogą sprawdzać puste pozycje. Domyślnie wartość ustawiona to 182, tyle haseł jest w pliku testowym.
Program znajdował wszystkie hasła zawarte w testach 1-4 na stronie https://kcir.pwr.edu.pl/~krzewnicka/?page=haszowanie, przy teście 5 program działa poprawnie.

W pliku zip znajduje się plik z listą użytkowników z testu 5 oraz plik słownikowy z testu 4, które są uznawane za domyślne. Nie zastosowano tutaj słownika z testu piątego,
ponieważ jest zbyt duży aby móc go wysłać na eportal.