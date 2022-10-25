#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef struct snake {
    int x;
    int y;
    struct snake *tail;
} snake;

typedef struct food {
    int x;
    int y;
    int value;
    struct food *naester;
} food;

//IO
void anzahl_parameter_ueberpruefen(int argc);
void dateien_laden(int argc, char **argv, int level, FILE **eingabeDatei, FILE **ausgabeDatei, FILE **levelDatei, char **levelDateiName);

//Feld
int zeilen_zaehlen(FILE *textDatei);
char **speicher_fuer_feld_allocieren(int zeilen, int spalten);
void feld_freigeben(char **feld, int zeilenGesamt);
int feld_einlesen(FILE *eingabeDatei, char **feld);
void feld_ausgeben(FILE *ausgabeDatei, char **feld, int zeilen, int spalten, snake *snakeKopf);

//Snake
snake *snake_erstellen(int x, int y);
void snake_und_food_auslesen(char **feld, int zeilenGesamt, int spaltenGesamt, snake **snake, food **foodListe);
void snake_freigeben(snake *snake);

snake *snake_wachsen_lassen(snake *snake, int amount);

int ein_befehl_auswerten(snake *snake, char dir, food **foodList, int *anzahlBefehle, char **feld, FILE *ausgabeDatei);
void ein_feld_bewegen(snake *snake, int x, int y);

int ueberpruefe_schlange_getroffen(snake *snake);
int ueberpruefe_wand_getroffen(char **feld, int x, int y);
int ueberpruefe_food_getroffen(food **foodList, int x, int y);

//Food
void food_Anhaengen(food **foodListe, int x, int y, int value);
void food_freigeben(food *foodListe);