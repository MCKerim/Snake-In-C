#include "snake.h"

int main(int argc, char **argv){
    anzahl_parameter_ueberpruefen(argc);

    int level = 0;
    while(level == 0 || level < ((argc-1) / 3)){
        FILE *eingabeDatei = stdin;
        FILE *ausgabeDatei = stdout;
        FILE *levelDatei = NULL;
        char *levelDateiName = "level/1.txt";

        dateien_laden(argc, argv, level, &eingabeDatei, &ausgabeDatei, &levelDatei, &levelDateiName);

        const int spaltenGesamt = 80;
        const int zeilenGesamt = zeilen_zaehlen(levelDatei);
        char **feld = speicher_fuer_feld_allocieren(zeilenGesamt, spaltenGesamt);

        freopen(levelDateiName, "r", levelDatei);
        feld_einlesen(levelDatei, feld);
        fclose(levelDatei);

        snake *snakeKopf = NULL;
        food *foodListe = NULL;
        snake_und_food_auslesen(feld, zeilenGesamt, spaltenGesamt, &snakeKopf, &foodListe);

        feld_ausgeben(ausgabeDatei, feld, zeilenGesamt, spaltenGesamt, snakeKopf);

        char dir;
        int anzahlBefehle = 0;
        while(fscanf(eingabeDatei, "%1c/n", &dir) != EOF){
            int ergebniss = ein_befehl_auswerten(snakeKopf, dir, &foodListe, &anzahlBefehle, feld, ausgabeDatei);
            //Feld ausgeben wenn keine Falsche Eingabe gemacht wurde
            if(ergebniss != -1){
                feld_ausgeben(ausgabeDatei, feld, zeilenGesamt, spaltenGesamt, snakeKopf);
            }

            //Spiel beenden wenn 1 Fall eintritt
            if(ergebniss == 1){
                fprintf(ausgabeDatei, "Gewonnen!\n");
                break;
            }else if(ergebniss == 2){
                fprintf(ausgabeDatei, "Die Schlange hat sich selbst gefressen!\n");
                break;
            }else if(ergebniss == 3){
                fprintf(ausgabeDatei, "Die Schlange ist in die Wand gelaufen!\n");
                break;
            }
        }

        snake_freigeben(snakeKopf);
        snakeKopf = NULL;
        food_freigeben(foodListe);
        foodListe = NULL;
        feld_freigeben(feld, zeilenGesamt);
        feld = NULL;

        if(eingabeDatei != stdin)
            fclose(eingabeDatei);

        if(ausgabeDatei != stdout) 
            fclose(ausgabeDatei);

        level++;
    }
    return 0;
}

void anzahl_parameter_ueberpruefen(int argc){
    //Ueberpruefen ob Parameteranzahl ein vielfaches von 3 ist
    if((argc-1) % 3 != 0){
        fprintf(stderr, "ERROR: Falsche Parameteranzahl.\n");
        exit(1);
    }
}

void dateien_laden(int argc, char **argv, int level, FILE **eingabeDatei, FILE **ausgabeDatei, FILE **levelDatei, char **levelDateiName){
    if(argc > 1 && strcmp(argv[1 + (level * 3)], "-")){
        *eingabeDatei = fopen(argv[1 + (level * 3)], "r+");
            
        if(errno == EISDIR){
            fprintf(stderr, "ERROR: Eingabedatei konnte nicht gelesen werden.\n");
            if(*eingabeDatei != NULL){
                fclose(*eingabeDatei);
            }
            exit(3);
        }
        if(*eingabeDatei == NULL){
            fprintf(stderr, "ERROR: Eingabedatei konnte nicht geöffnet werden.\n");
            exit(2);
        }
    }

    if(argc > 1 && strcmp(argv[2 + (level * 3)], "-")){
        *ausgabeDatei = fopen(argv[2 + (level * 3)], "w");
        if(*ausgabeDatei == NULL){
            fclose(*eingabeDatei);
            fprintf(stderr, "ERROR: Ausgabedatei konnte nicht geöffnet werden.\n");
            exit(2);
        }
    }

    if(argc > 1 && strcmp(argv[3 + (level * 3)], "-")){
        *levelDateiName = argv[3 + (level * 3)];
    }

    *levelDatei = fopen(*levelDateiName, "r+");

    if(errno == EISDIR){
        fprintf(stderr, "ERROR: Leveldatei konnte nicht gelesen werden.\n");
        if(*levelDatei != NULL){
            fclose(*levelDatei);
        }
        fclose(*eingabeDatei);
        fclose(*ausgabeDatei);
        exit(3);
    }
    if(*levelDatei == NULL){
        fprintf(stderr, "ERROR: Leveldatei konnte nicht geöffnet werden.\n");
        fclose(*eingabeDatei);
        fclose(*ausgabeDatei);
        exit(2);
    }
}

void snake_und_food_auslesen(char **feld, int zeilenGesamt, int spaltenGesamt, snake **snake, food **foodListe){
    for(int zeile=0; zeile < zeilenGesamt; zeile++){
        for(int spalte=0; spalte < spaltenGesamt; spalte++){
            if(feld[zeile][spalte] == 'O'){
                //Snake Gefunden
                *snake = snake_erstellen(spalte, zeile);
                feld[zeile][spalte] = ' ';
            }else if(feld[zeile][spalte] >= 49 && feld[zeile][spalte] <= 57){
                //Food Gefunden
                int value = atoi(&feld[zeile][spalte]);
                food_Anhaengen(&(*foodListe), spalte, zeile, value);
            }
        }
    }

    if(*snake == NULL){
        fprintf(stderr, "Keine Schlange im Level gefunden.\n");
        exit(5);
    }
}

int zeilen_zaehlen(FILE *textDatei){
    int zeilenGesamt = 0;
    char currentChar;
    while(fscanf(textDatei, "%c", &currentChar) == 1){
        if(currentChar == '\n'){
            zeilenGesamt++;
        }
    }
    return zeilenGesamt;
}

char **speicher_fuer_feld_allocieren(int zeilen, int spalten){
    char **feld = malloc(zeilen * sizeof(int *));
    if(feld == NULL){
        fprintf(stderr, "ERROR: Nicht genug Speicherplatz.\n");
        exit(5);
    }

    for(int i=0; i < zeilen; i++){
        feld[i] = malloc(spalten * sizeof(char));
        if(feld[i] == NULL){
            fprintf(stderr, "ERROR: Nicht genug Speicherplatz.\n");
            exit(5);
        }
    }
    return feld;
}

void feld_freigeben(char **feld, int zeilenGesamt){
    if(feld == NULL){
        return;
    }

    for(int i=0; i < zeilenGesamt; i++){
        if(feld[i] != NULL){
            free(feld[i]);
        }
    }
    free(feld);
}

int feld_einlesen(FILE *levelDatei, char **feld){
    int currentZeile = 0;
    int currentCharNumber = 0;
    char currentChar;
    while(fscanf(levelDatei, "%c", &currentChar) == 1){
        if(currentChar == '\n'){
            currentCharNumber = 0;
            currentZeile++;
        }else{
            feld[currentZeile][currentCharNumber] = currentChar;
            currentCharNumber++;
        }
    }
    return 0;
}

void feld_ausgeben(FILE *ausgabeDatei, char **feld, int zeilen, int spalten, snake *snakeKopf){
    //Snake in das Feld einfügen
    snake *current = snakeKopf;
    while(current != NULL){
        feld[current->y][current->x] = 'O';
        current = current->tail;
    }
    //Feld ausgeben
    for(int zeile=0; zeile < zeilen; zeile++){
        for(int spalte=0; spalte < spalten; spalte++){
            fprintf(ausgabeDatei, "%c", feld[zeile][spalte]);
        }
        fprintf(ausgabeDatei, "\n");
    }
    //Snake wieder aus dem Feld entfernen
    current = snakeKopf;
    while(current != NULL){
        feld[current->y][current->x] = ' ';
        current = current->tail;
    }       
}

int ein_befehl_auswerten(snake *snake, char dir, food **foodList, int *anzahlBefehle, char **feld, FILE *ausgabeDatei){
    int x = 0;
    int y = 0;
    switch (dir){
        case 'w':
            x = 0;
            y = -1;
        break;

        case 's':
            x = 0;
            y = 1;
        break;

        case 'a':
            x = -1;
            y = 0;
        break;
        
        case 'd':
            x = 1;
            y = 0;
        break;
    
        default:
            return -1;
        break;
    }

    *anzahlBefehle = (*anzahlBefehle) + 1;
    fprintf(ausgabeDatei, "%d\n", *anzahlBefehle);
    
    ein_feld_bewegen(snake, x, y);

    if(ueberpruefe_schlange_getroffen(snake)){
        return 2;
    }else if(ueberpruefe_wand_getroffen(feld, snake->y, snake->x)){
        return 3;
    }

    int amountOfFood = ueberpruefe_food_getroffen(foodList, snake->x, snake->y);
    if(amountOfFood){
        snake_wachsen_lassen(snake, amountOfFood);
    }

    //Alles an Food gegessen
    if(*foodList == NULL){
        return 1;
    }
    return 0;
}

void ein_feld_bewegen(snake *snake, int x, int y){
    struct snake *current = snake;
    int lastX = current->x;
    int lastY = current->y;

    current->x += x;
    current->y += y;

    int nx = lastX;
    int ny = lastY;
    while(current->tail != NULL){
        current = current->tail;
        lastX = current->x;
        lastY = current->y;

        current->x = nx;
        current->y = ny;

        nx = lastX;
        ny = lastY;
    }
}

int ueberpruefe_wand_getroffen(char **feld, int zeile, int spalte){
    return feld[zeile][spalte] == 'X';
}

int ueberpruefe_food_getroffen(food **foodList, int x, int y){
    food *head = *foodList;
    if(head == NULL){
        return 0;
    }

    if(head->x == x && head->y == y){
        int value = head->value;
        *foodList = head->naester;
        free(head);
        return value;
    }

    food *current = *foodList;
    while(current->naester != NULL){
        if(current->naester->x == x && current->naester->y == y){
            food *temp = current->naester;
            current->naester = temp->naester;
            int value = temp->value;
            free(temp);

            return value;
        }
        current = current->naester;
    }
    return 0;
}

int ueberpruefe_schlange_getroffen(snake *snake){
    if(snake == NULL){
        return 0;
    }

    struct snake *current = snake->tail;
    
    while(current != NULL){
        if(snake->x == current->x && snake->y == current->y){
            return 1;
        }
        current = current->tail;
    }
    return 0;
}

snake *snake_erstellen(int x, int y){
    snake *snake = malloc(sizeof(struct snake));
    if(snake == NULL){
        fprintf(stderr, "ERROR: Nicht genug Speicherplatz.\n");
        exit(5);
    }
    snake->x = x;
    snake->y = y;
    snake->tail = NULL;
    return snake;
}

snake *snake_wachsen_lassen(snake *snake, int amount){
    struct snake *current = snake;

    while(current->tail != NULL){
        current = current->tail;
    }

    for(int i=0; i < amount; i++){
        struct snake *newBodyPart = malloc(sizeof(struct snake));
        if(newBodyPart == NULL){
            fprintf(stderr, "ERROR: Nicht genug Speicherplatz.\n");
            exit(5);
        }

        newBodyPart->x = current->x;
        newBodyPart->y = current->y;
        newBodyPart->tail = NULL;

        current->tail = newBodyPart;
        current = current->tail;
    }
    return snake;
}

void snake_freigeben(snake *snake){
    if(snake == NULL){
        return;
    }

    struct snake *current = snake;
    while(current->tail != NULL){
        struct snake *temp = current;
        current = current->tail;
        free(temp);
    }
    free(current);
}

void food_Anhaengen(food **foodListe, int x, int y, int value){
    food *newFood = malloc(sizeof(food));
    if(newFood == NULL){
        fprintf(stderr, "ERROR: Nicht genug Speicherplatz.\n");
        exit(5);
    }
    newFood->x = x;
    newFood->y = y;
    newFood->value = value;
    newFood->naester = NULL;

    if(*foodListe == NULL){
        *foodListe = newFood;
    }else{
        food *current = *foodListe;
        while(current->naester != NULL){
            current = current->naester;
        }
        current->naester = newFood;
    }
}

void food_freigeben(food *foodListe){
    if(foodListe == NULL){
        return;
    }

    food *current = foodListe;
    while(current->naester != NULL){
        food *temp = current;
        current = current->naester;
        free(temp);
    }
    free(current);
}