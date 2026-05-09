#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <windows.h>

#define MAX_ERRORS 6
#define MAX_WORD_LENGTH 20
#define SCORES_FILE "top_scores.txt"
#define MAX_SCORES 10

typedef struct {
    char player_name[50];
    int score;
    char word[MAX_WORD_LENGTH + 1];
} Score;

const char *words[] = {
    "ordinateur", "programmation", "algorithme", "variable", "fonction",
    "boucle", "condition", "pointeur", "structure", "tableau",
    "compilation", "debugging", "interface", "database", "internet",
    "securite", "cryptographie", "recursion", "iteration", "exception"
};

#define NUM_WORDS (sizeof(words) / sizeof(words[0]))

void clear_screen() {
    system("cls");
}

void display_logo() {
    printf("\n");
    printf("\n\n\n");

printf("                         _   ______   _   _     _____   ______   _   _   _____   _    _ \n");

printf("                        | | |  ____| | | | |   |  __ \\ |  ____| | \\ | | |  __ \\ | |  | |\n");

printf("                        | | | |__    | | | |   | |__) || |__    |  \\| | | |  | || |  | |\n");

printf("                    _   | | |  __|   | | | |   |  ___/ |  __|   | . ` | | |  | || |  | |\n");

printf("                   | |__| | | |____  | |_| |   | |     | |____  | |\\  | | |__| || |__| |\n");

printf("                    \\____/  |______|  \\___/    |_|     |______| |_| \\_| |_____/  \\____/ \n");

printf("\n\n\n\n");
}

void display_menu() {
    clear_screen();
    display_logo();
    printf("1. Commencer une nouvelle partie\n");
    printf("2. Voir les meilleurs scores\n");
    printf("3. Instructions\n");
    printf("4. Quitter\n\n");
    printf("Votre choix : ");
}

void display_instructions() {
    clear_screen();
    display_logo();
    printf("======== COMMENT JOUER ========\n\n");
    printf("Objectif :\n");
    printf("- Devinez le mot cache en proposant des lettres\n");
    printf("- Vous avez %d tentatives maximum\n", MAX_ERRORS);
    printf("- Chaque lettre incorrecte enleve une tentative\n\n");
    printf("Regles :\n");
    printf("- Une lettre a la fois\n");
    printf("- Pas de doublon (meme lettre 2 fois)\n");
    printf("- Majuscules et minuscules acceptes\n\n");
    printf("Scoring :\n");
    printf("- Points = Tentatives restantes x Longueur du mot x 10\n");
    printf("- Exemple : 4 tentatives x 10 lettres x 10 = 400 points\n\n");
    printf("================================\n");
    printf("Appuyez sur Entree pour continuer...");
    getchar();
}

void display_hangman(int errors) {
    printf("\n");
    printf("   ------\n");
    printf("   |    |\n");
    printf("   |    %s\n", errors >= 1 ? "O" : " ");
    printf("   |   %s%s%s\n", 
        errors >= 2 ? "/" : " ",
        errors >= 3 ? "|" : " ",
        errors >= 4 ? "\\" : " ");
    printf("   |   %s%s\n",
        errors >= 5 ? "/" : " ",
        errors >= 6 ? "\\" : " ");
    printf("   |\n");
    printf("  _|_\n\n");
}

const char* choose_random_word() {
    srand((unsigned)time(NULL) + rand());
    return words[rand() % NUM_WORDS];
}

void load_scores(Score* scores, int* count) {
    FILE* file = fopen(SCORES_FILE, "r");
    *count = 0;
    
    if (file == NULL) {
        return;
    }
    
    while (*count < MAX_SCORES && 
           fscanf(file, "%49s %d %19s", scores[*count].player_name, 
                   &scores[*count].score, scores[*count].word) == 3) {
        (*count)++;
    }
    fclose(file);
}

void save_scores(const Score* scores, int count) {
    FILE* file = fopen(SCORES_FILE, "w");
    if (file == NULL) {
        printf("Erreur lors de la sauvegarde des scores.\n");
        return;
    }
    
    for (int i = 0; i < count && i < MAX_SCORES; i++) {
        fprintf(file, "%s %d %s\n", scores[i].player_name, 
                scores[i].score, scores[i].word);
    }
    fclose(file);
}

void add_score(Score* scores, int* count, const char* name, int score, const char* word) {
    Score new_score;
    strcpy(new_score.player_name, name);
    new_score.score = score;
    strcpy(new_score.word, word);
    
    int i = *count;
    while (i > 0 && scores[i - 1].score < score) {
        scores[i] = scores[i - 1];
        i--;
    }
    
    scores[i] = new_score;
    if (*count < MAX_SCORES) (*count)++;
    
    save_scores(scores, *count);
}

void display_top_scores() {
    clear_screen();
    display_logo();
    
    Score scores[MAX_SCORES];
    int count;
    load_scores(scores, &count);
    
    printf("========== MEILLEURS SCORES ==========\n\n");
    
    if (count == 0) {
        printf("Aucun score enregistre pour le moment.\n\n");
    } else {
        printf("Rang | Joueur                    | Score | Mot\n");
        printf("====================================================\n");
        for (int i = 0; i < count; i++) {
            printf(" %2d  | %-25s | %5d | %s\n", 
                   i + 1, scores[i].player_name, scores[i].score, scores[i].word);
        }
        printf("\n");
    }
    
    printf("Appuyez sur Entree pour revenir au menu...");
    getchar();
}

int check_letter(const char* word, char* guessed, char letter) {
    int found = 0;
    for (int i = 0; word[i]; i++) {
        if (tolower(word[i]) == tolower(letter)) {
            guessed[i] = word[i];
            found = 1;
        }
    }
    return found;
}

int is_word_guessed(const char* guessed) {
    for (int i = 0; guessed[i]; i++) {
        if (guessed[i] == '_') return 0;
    }
    return 1;
}

void play_game(Score* scores, int* count) {
    clear_screen();
    display_logo();
    
    char player_name[50];
    printf("Entrez votre nom : ");
    fgets(player_name, sizeof(player_name), stdin);
    player_name[strcspn(player_name, "\n")] = 0;
    
    if (strlen(player_name) == 0) {
        strcpy(player_name, "Joueur");
    }
    
    printf("\nChargement du jeu");
    for (int i = 0; i < 3; i++) {
        printf(".");
        fflush(stdout);
        Sleep(500);
    }
    printf("\n\n");
    
    const char* word = choose_random_word();
    int word_len = strlen(word);
    char guessed[MAX_WORD_LENGTH + 1];
    char used_letters[26] = {0};
    
    for (int i = 0; i < word_len; i++) {
        guessed[i] = '_';
    }
    guessed[word_len] = 0;
    
    int errors = 0;
    
    printf("Bienvenue %s !\n", player_name);
    printf("Vous avez %d tentatives pour trouver le mot.\n\n", MAX_ERRORS);
    Sleep(1500);
    
    while (errors < MAX_ERRORS && !is_word_guessed(guessed)) {
        clear_screen();
        display_logo();
        display_hangman(errors);
        
        printf("MOT : ");
        for (int i = 0; guessed[i]; i++) {
            printf("%c ", guessed[i]);
        }
        printf("\n\n");
        
        printf("TENTATIVES : %d/%d\n", MAX_ERRORS - errors, MAX_ERRORS);
        
        printf("LETTRES : ");
        int has_letters = 0;
        for (int i = 0; i < 26; i++) {
            if (used_letters[i]) {
                printf("%c ", 'a' + i);
                has_letters = 1;
            }
        }
        if (!has_letters) printf("(aucune)");
        printf("\n\n");
        
        printf("Proposez une lettre : ");
        
        char letter;
        scanf(" %c", &letter);
        letter = tolower(letter);
        
        if (!isalpha(letter)) {
            printf("ERREUR : Veuillez entrer une lettre valide.\n");
            Sleep(1500);
            continue;
        }
        
        if (used_letters[letter - 'a']) {
            printf("ERREUR : Vous avez deja propose cette lettre !\n");
            Sleep(1500);
            continue;
        }
        
        used_letters[letter - 'a'] = 1;
        
        if (check_letter(word, guessed, letter)) {
            printf("\n>>> BONNE LETTRE ! <<<\n");
        } else {
            printf("\n>>> LETTRE INCORRECTE ! <<<\n");
            errors++;
        }
        
        Sleep(1500);
    }
    
    clear_screen();
    display_logo();
    display_hangman(errors);
    
    if (is_word_guessed(guessed)) {
        printf("================================================\n");
        printf("         FELICITATIONS ! VOUS AVEZ GAGNE !\n");
        printf("================================================\n\n");
        printf("Le mot etait : %s\n\n", word);
        
        int score = (MAX_ERRORS - errors) * word_len * 10;
        printf("Votre score : %d points\n\n", score);
        
        add_score(scores, count, player_name, score, word);
        printf("(Score enregistre !)\n");
    } else {
        printf("================================================\n");
        printf("           DOMMAGE ! VOUS AVEZ PERDU !\n");
        printf("================================================\n\n");
        printf("Le mot etait : %s\n\n", word);
    }
    
    printf("Appuyez sur Entree pour revenir au menu...");
    getchar();
}

int main() {
    Score scores[MAX_SCORES];
    int scores_count = 0;
    int choice;
    
    load_scores(scores, &scores_count);
    
    while (1) {
        display_menu();
        scanf("%d", &choice);
        getchar();
        
        switch(choice) {
            case 1:
                play_game(scores, &scores_count);
                break;
            case 2:
                display_top_scores();
                break;
            case 3:
                display_instructions();
                break;
            case 4:
                printf("\nMerci d'avoir joue ! A bientot !\n");
                Sleep(1500);
                return 0;
            default:
                printf("ERREUR : Choix invalide ! Reessayez.\n");
                Sleep(1500);
        }
    }
    
    return 0;
}
