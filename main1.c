//403106827
//۵۹ * 185   
#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
//#include <ncursesw/ncurses.h>
#include <locale.h>
#include <ctype.h>

#define MAX_LENGTH 50
#define FILE_NAME "names.txt"
#define SCORES_FILE "scores.txt"
#define USERS_PER_PAGE 10
#define MAX_ROOMS 10
#define MAP_WIDTH 170
#define MAP_HEIGHT 40
#define MIN_ROOMS_PER_FLOOR 6
#define MAX_VISIBILITY 5
#define MIN_ROOM_SIZE 6
#define MAX_ROOM_SIZE 12
#define MAX_FLOORS 4 

int i = 0;
int food_count = 0;
int hunger_level = 0;
int gold_count = 0; 
int score = 0;       

typedef struct {
    char name[50], pass[50], email[50];
} Player;

Player player;

const char *options[] = {
    "Start New Game",
    "Load Game",
    "Guest Login",
    "Score Board",
    "Options",
    "Exit",
};

const char *difficulty_levels[] = {
    "Easy",
    "Medium",
    "Hard"
};

const char *colors[] = {
    "Red",
    "Green",
    "Blue"
};

const char *songs[] = {
    "Song 1",
    "Song 2",
    "Song 3"
};
typedef struct {
    int x, y, width, height;
    int visited;
} Room;

typedef struct {
    int width, height;
    char **tiles;
    Room rooms[MAX_ROOMS];
    int room_count;
} GameMap;

typedef struct {
    int x, y, width, height;
} Rect;

typedef struct Node {
    Rect rect;
    struct Node *left;
    struct Node *right;
    Room room;
} Node;

typedef struct {
    GameMap* map;
    int start_x, start_y;
} Floor;

typedef struct {
    int visited;
} RoomStatus;

RoomStatus room_status[MAX_ROOMS];

typedef struct {
    int x, y;
    int hp;
} Character;

Character user = {0, 0, 1000};

#define MAX_WEAPONS 5

typedef struct {
    char name[20];
    char sign[5];
    int n;
} Weapon;

Weapon weapons[] = {
    {"Mace", "m", 1},
    {"Dagger", "d", 0},
    {"Magic Wand", "w", 0},
    {"Normal Arrow", "n", 0},
    {"Sword", "s", 0}
};

typedef struct {
    Weapon inventory[MAX_WEAPONS];
    int weapon_count[MAX_WEAPONS];
} Backpack;

Backpack backpack = {{{"Mace", "\u2692"}}, {1}};

#define MAX_SPELLS 3

typedef struct {
    char name[20];
    char sign[5];
    int count;
} Spell;

Spell spells[] = {
    {"Health", "H", 0},
    {"Speed", "S", 0},
    {"Damage", "D", 0}
};

typedef struct {
    Spell inventory[MAX_SPELLS];
    int spell_count[MAX_SPELLS];
} SpellBook;

SpellBook spellbook = {{{"Health", "H", 0}, {"Speed", "S", 0}, {"Damage", "D", 0}}, {0}};


Floor floors[MAX_FLOORS];
int current_floor = 0;

char previous_tile = '.';


int n_options = sizeof(options) / sizeof(options[0]);
int n_difficulty_levels = sizeof(difficulty_levels) / sizeof(difficulty_levels[0]);
int n_colors = sizeof(colors) / sizeof(colors[0]);
int n_songs = sizeof(songs) / sizeof(songs[0]);

void print_menu(WINDOW *menu_win, const char **menu_items, int n_items, int highlight);
void get_player_info();
void sign_up();
int is_duplicate(const char *filename, const char *name);
int validate_email(const char *email);
void generatePassword(int length, char* password);
void load_game();
void guest_login();
void show_score_board();
void shGameMapow_options_menu();
void set_difficulty();
void change_hero_color();
void select_song();
GameMap* create_map(int width, int height);
void free_map(GameMap* map);
void generate_map(GameMap* map);
void draw_map(GameMap* map, int current_room_idx);
void create_room(GameMap* map, int x, int y, int width, int height);
void dfs_connect_rooms(GameMap* map, int room_index);
int is_overlapping(Room room1, Room room2);
void visit_room(GameMap* map, int room_idx);
void generate_bsp_map(GameMap* map);
bool window_added = false;
void place_character(GameMap* map);
void handle_input();
int is_room_visited(GameMap* map, int room_idx);
void place_stairs(GameMap* map, int exclude_room_index, char stair_symbol);
void connect_single_room(GameMap*, Room, Room);
int check_room(int[], int[], int);
void check_doors_distance(GameMap* map);
void place_food(GameMap* map);
void manage_food_menu();
void update_status();
void place_gold(GameMap* map);
void show_options_menu();
void place_weapons(GameMap* map);
void manage_inventory();
void showMessage();
void manage_spellbook();
void place_spells(GameMap* map);

int main() {
    user.hp = 1000;
    setlocale(LC_ALL, "");
    initscr();
    keypad(stdscr, TRUE);
    start_color();
    cbreak();
    noecho();
    if (has_colors() == FALSE) {  
        endwin();
        printf("Your terminal does not support color\n");
        exit(1);
    }
       srand(time(NULL));

    // init kardan  color ha
    init_color(3, 700, 0, 1000);
    init_pair(10, 3, COLOR_BLACK);
    init_color(5, 1000, 0, 0);
    init_pair(11, 5, COLOR_BLACK);
    attron(COLOR_PAIR(10));
    curs_set(0);
    int highlight = 1;
    int choice = 0;
    int c;

    WINDOW *menu_win;
    int startx = (160) / 2;
    int starty = (35 / 2);

    menu_win = newwin(10, 30, starty, startx);
    keypad(menu_win, TRUE);

    refresh();
    print_menu(menu_win, options, n_options, highlight);

    while (1) {

        c = wgetch(menu_win);
        switch (c) {
            case KEY_UP: 
                if (highlight == 1)
                    highlight = n_options;
                else
                    --highlight;
                break;
            case KEY_DOWN:
                if (highlight == n_options)
                    highlight = 1;
                else
                    ++highlight;
                break;
            case 10: 
                choice = highlight;
                break;
            default:
                break;
        }
        print_menu(menu_win, options, n_options, highlight);
        if (choice != 0)
            break;
    }
    echo();
    clear();
    if (choice == 1) {
        // Start New Game
        get_player_info();
    } else if (choice == 2) {
        load_game();
    } else if (choice == 3) {
        // Guest Login
        guest_login();
    } else if (choice == 4) {
        // Score Board
        show_score_board();
    } else if (choice == 5) {
        // Options
        show_options_menu();
    }
    refresh();
    getch();

    endwin();
    return 0;

}
void print_menu(WINDOW *menu_win, const char **menu_items, int n_items, int highlight) {
    int x, y, i;
    x = 2;
    y = 2;
    box(menu_win, 0, 0);
    for (i = 0; i < n_items; ++i) {
        if (highlight == i + 1) {
            wattron(menu_win, A_REVERSE);
            mvwprintw(menu_win, y, x, "%s", menu_items[i]);
            wattroff(menu_win, A_REVERSE);
        } else {
            mvwprintw(menu_win, y, x, "%s", menu_items[i]);
        }
        ++y;
    }
    wrefresh(menu_win);
}

void show_options_menu() {
    int highlight = 1;
    int choice = 0;
    int c;

    WINDOW *options_win;
    int startx = (160) / 2;
    int starty = (35 / 2);

    options_win = newwin(10, 30, starty, startx);
    keypad(options_win, TRUE);

    const char *options_menu[] = {
        "Set Difficulty",
        "Change Hero Color",
        "Select Song",
        "Back"
    };

    int n_options_menu = sizeof(options_menu) / sizeof(options_menu[0]);

    print_menu(options_win, options_menu, n_options_menu, highlight);

    while (1) {
        c = wgetch(options_win);
        switch (c) {
            case KEY_UP: 
                if (highlight == 1)
                    highlight = n_options_menu;
                else
                    --highlight;
                break;
            case KEY_DOWN:
                if (highlight == n_options_menu)
                    highlight = 1;
                else
                    ++highlight;
                break;
            case 10: 
                choice = highlight;
                break;
            default:
                break;
        }
        print_menu(options_win, options_menu, n_options_menu, highlight);
        if (choice != 0)
            break;
    }
    echo();
    clear();
    if (choice == 1) {
        set_difficulty();
    } else if (choice == 2) {
        change_hero_color();
    } else if (choice == 3) {
        select_song();
    } else if (choice == 4) {
    }
    refresh();
    getch();

    clear();
    refresh();
    main();
}

void set_difficulty() {
    int highlight = 1;
    int choice = 0;
    int c;

    WINDOW *difficulty_win;
    int startx = (160) / 2;
    int starty = (35 / 2);

    difficulty_win = newwin(10, 30, starty, startx);
    keypad(difficulty_win, TRUE);

    print_menu(difficulty_win, difficulty_levels, n_difficulty_levels, highlight);

    while (1) {
        c = wgetch(difficulty_win);
        switch (c) {
            case KEY_UP: 
                if (highlight == 1)
                    highlight = n_difficulty_levels;
                else
                    --highlight;
                break;
            case KEY_DOWN:
                if (highlight == n_difficulty_levels)
                    highlight = 1;
                else
                    ++highlight;
                break;
            case 10: 
                choice = highlight;
                break;
            default:
                break;
        }
        print_menu(difficulty_win, difficulty_levels, n_difficulty_levels, highlight);
        if (choice != 0)
            break;
    }
    echo();
    clear();
    mvprintw(0, 0, "Difficulty set to %s", difficulty_levels[choice - 1]);
    refresh();
    getch();
}

void change_hero_color() {
    int highlight = 1;
    int choice = 0;
    int c;

    WINDOW *color_win;
    int startx = (160) / 2;
    int starty = (35 / 2);

    color_win = newwin(10, 30, starty, startx);
    keypad(color_win, TRUE);

    print_menu(color_win, colors, n_colors, highlight);

    while (1) {
        c = wgetch(color_win);
        switch (c) {
            case KEY_UP: 
                if (highlight == 1)
                    highlight = n_colors;
                else
                    --highlight;
                break;
            case KEY_DOWN:
                if (highlight == n_colors)
                    highlight = 1;
                else
                    ++highlight;
                break;
            case 10: 
                choice = highlight;
                break;
            default:
                break;
        }
        print_menu(color_win, colors, n_colors, highlight);
        if (choice != 0)
            break;
    }
    echo();
    clear();
    mvprintw(0, 0, "Hero color set to %s", colors[choice - 1]);
    refresh();
    getch();
}

void select_song() {
    int highlight = 1;
    int choice = 0;
    int c;

    WINDOW *song_win;
    int startx = (160) / 2;
    int starty = (35 / 2);

    song_win = newwin(10, 30, starty, startx);
    keypad(song_win, TRUE);

    print_menu(song_win, songs, n_songs, highlight);

    while (1) {
        c = wgetch(song_win);
        switch (c) {
            case KEY_UP: 
                if (highlight == 1)
                    highlight = n_songs;
                else
                    --highlight;
                break;
            case KEY_DOWN:
                if (highlight == n_songs)
                    highlight = 1;
                else
                    ++highlight;
                break;
            case 10: 
                choice = highlight;
                break;
            default:
                break;
        }
        print_menu(song_win, songs, n_songs, highlight);
        if (choice != 0)
            break;
    }
    echo();
    clear();
    mvprintw(0, 0, "Song selected: %s", songs[choice - 1]);
    refresh();
    getch();
}

void get_player_info() {
    echo();  
    char name[MAX_LENGTH], pass[MAX_LENGTH], email[MAX_LENGTH], temp[MAX_LENGTH];

    mvprintw(0, 0, "Enter your name: ");
    refresh();
    while (1) {
        scanw("%s", player.name);
        if (player.name != NULL && !(is_duplicate(FILE_NAME, player.name))) {
            FILE* f;
            f = fopen(FILE_NAME, "a");
            fprintf(f, "%s\n", player.name);
            fclose(f);
            break;
        } else {
            printw("Duplicate name. Try again.");
        }
        refresh();
    }

    int has_digit = 0;
    int has_upper = 0;
    int has_lower = 0;
    echo();
    while (!(has_digit && has_upper && has_lower)) {
        attron(COLOR_PAIR(10));
        mvprintw(1, 0, "Enter your password (or press 'r' to generate a random password): ");
        refresh();  
        scanw("%s", temp);
        
        if (strcmp(temp, "r") == 0) {
            attroff(COLOR_PAIR(10));
            generatePassword(10, player.pass); 
        } else {
            strcpy(player.pass, temp);
        }

        if (strlen(player.pass) < 7) {
            attron(COLOR_PAIR(11));
            mvprintw(2, 0, "Error: password must be at least 7 characters long.");
            attroff(COLOR_PAIR(11));
        } else {
            clrtoeol();
            for (int i = 0; i < strlen(player.pass); ++i) {
                if (isdigit(player.pass[i])) {
                    has_digit = 1;
                }
                if (isupper(player.pass[i])) {
                    has_upper = 1;
                }
                if (islower(player.pass[i])) {
                    has_lower = 1;
                }
            }
        }

        if (has_digit && has_upper && has_lower) {
        } else {
            attron(COLOR_PAIR(11));
            mvprintw(3, 0, "Error: password must contain at least one digit, one uppercase letter, and one lowercase letter.");
            attroff(COLOR_PAIR(11));
        }
        clrtoeol();
    }

    while (1) {
        attron(COLOR_PAIR(10));
        mvprintw(4, 0, "Enter your email: ");
        refresh();
        scanw("%s", player.email);

        if (validate_email(player.email)) {
            clrtoeol() ;
            break;
        } else {
            attron(COLOR_PAIR(11));
            mvprintw(5, 0, "Error: Invalid email format. Please enter a valid email.");
            attroff(COLOR_PAIR(11));
        }
        refresh();
    }

    showMessage("New Game Started! Welcome, ", 3);

    for(int i = 0; i < MAX_FLOORS; ++i) {
        GameMap* game_map = create_map(MAP_WIDTH, MAP_HEIGHT);
        generate_bsp_map(game_map);
        floors[i].map = game_map;
    }
    current_floor = 0;
    place_character(floors[current_floor].map);
    place_food(floors[current_floor].map);
    place_gold(floors[current_floor].map);
    place_weapons(floors[current_floor].map); 
    place_spells(floors[current_floor].map); 
    visit_room(floors[current_floor].map, 0);
    draw_map(floors[current_floor].map, 0);
    refresh();
    handle_input();
    for (int i = 0; i < MAX_FLOORS; ++i) {
        free_map(floors[i].map);
    }
}
int is_duplicate(const char *filename, const char *name) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return 0;
    }
    char line[MAX_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, name) == 0) {
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

int validate_email(const char *email) {
    int at_idx = -1;
    int dot_idx = -1;
    int length = strlen(email);

    for (int i = 0; i < length; i++) {
        if (email[i] == '@') {
            at_idx = i;
            break;
        }
    }

    if (at_idx != -1) {
        for (int i = at_idx + 1; i < length; i++) {
            if (email[i] == '.') {
                dot_idx = i;
                break;
            }
        }
    }

    if (at_idx > 0 && dot_idx > at_idx + 1 && dot_idx < length - 1) {
        return 1;
    } else {
        return 0;
    }
}

void generatePassword(int length, char* password) {
    char characters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()_+";
    srand(time(0)); // Seed the random number generator

    for (int i = 0; i < length; i++) {
        int idx = rand() % (sizeof(characters) - 1);
        password[i] = characters[idx];
    }
    password[length] = '\0'; // Null-terminate the string

    mvprintw(2,0,"Generated Password: %s", password);
    
    refresh();
}

void load_game() {
    echo();  // Turn on echoing of characters
    char name[MAX_LENGTH], pass[MAX_LENGTH];

    mvprintw(0, 0, "Enter your name: ");
    refresh();
    scanw("%s", name);

    mvprintw(1, 0, "Enter your password: ");
    refresh();
    scanw("%s", pass);

    FILE *file = fopen(FILE_NAME, "r");
    if (file == NULL) {
        mvprintw(2, 0, "Error: The user not found, please start a new game.");
        refresh();
        return;
    }

    Player temp;
    bool found = false;
    while (fscanf(file, "%s %s %s", temp.name, temp.pass, temp.email) != EOF) {
        if (strcmp(temp.name, name) == 0 && strcmp(temp.pass, pass) == 0) {
            found = true;
            break;
        }
    }
    fclose(file);

   if (found) {
        mvprintw(2, 0, "Login successful! Loading game...");
        refresh();
        sleep(1);

        for (int i = 0; i < MAX_FLOORS; ++i) {
            GameMap* game_map = create_map(MAP_WIDTH, MAP_HEIGHT);
            generate_bsp_map(game_map);
            floors[i].map = game_map;
        }
        current_floor = 0;
        place_character(floors[current_floor].map);
        place_food(floors[current_floor].map);
        place_gold(floors[current_floor].map);
        place_weapons(floors[current_floor].map);
        place_spells(floors[current_floor].map); 
        visit_room(floors[current_floor].map, 0);
        draw_map(floors[current_floor].map, 0);
        refresh();
        handle_input();
        for (int i = 0; i < MAX_FLOORS; ++i) {
            free_map(floors[i].map);
        }
    } else {
        mvprintw(2, 0, "Error: Invalid username or password.");
        refresh();
        getch();
    }
    clear();
}
void guest_login() {
    strcpy(player.name, "Guest");
    strcpy(player.pass, "");
    strcpy(player.email, "");
    mvprintw(0, 0, "Logged in as Guest.");
    refresh();
    sleep(2);

    showMessage("Guest Login Successful! Enjoy your game.", 3);

    for (int i = 0; i < MAX_FLOORS; ++i) {
        GameMap* game_map = create_map(MAP_WIDTH, MAP_HEIGHT);
        generate_bsp_map(game_map);
        floors[i].map = game_map;
    }
    current_floor = 0;
    place_character(floors[current_floor].map);
    place_food(floors[current_floor].map);
    place_gold(floors[current_floor].map);
    place_weapons(floors[current_floor].map); 
    place_spells(floors[current_floor].map); 
    visit_room(floors[current_floor].map, 0);
    draw_map(floors[current_floor].map, 0);
    refresh();
    handle_input();
    for (int i = 0; i < MAX_FLOORS; ++i) {
        free_map(floors[i].map);
    }
}
void show_score_board() {
    clear();
    FILE *file = fopen(SCORES_FILE, "r");
    if (file == NULL) {
        mvprintw(0, 0, "Error: Unable to open scores file.");
        refresh();
        getch();
        return;
    }
    typedef struct {   //؟؟؟؟؟؟؟؟؟؟؟؟؟؟؟؟؟؟؟؟؟؟؟؟؟؟؟؟؟؟؟؟؟؟؟؟؟؟؟؟؟؟؟؟؟؟؟؟؟
        char name[MAX_LENGTH];
        int score;
        int gold;
        int games_completed;
        time_t first_game_time;
    } PlayerScore;

    PlayerScore players[100];
    int player_count = 0;

    while (fscanf(file, "%s %d %d %d %ld", players[player_count].name, &players[player_count].score, &players[player_count].gold, &players[player_count].games_completed, &players[player_count].first_game_time) != EOF) {
        player_count++;
    }
    fclose(file);

    for (int i = 0; i < player_count - 1; i++) {
        for (int j = i + 1; j < player_count; j++) {
            if (players[i].score < players[j].score) {
                PlayerScore temp = players[i];
                players[i] = players[j];
                players[j] = temp;
            }
        }
    }

    int current_page = 0;
    int total_pages = (player_count + USERS_PER_PAGE - 1) / USERS_PER_PAGE;

    while (1) {
        clear();
        mvprintw(0, 0, "Score Board (Page %d of %d)", current_page + 1, total_pages);
        mvprintw(1, 0, "Rank  Username       Score   Gold    Games   Experience");

        int start_idx = current_page * USERS_PER_PAGE;
        int end_idx = start_idx + USERS_PER_PAGE;
        if (end_idx > player_count) {
            end_idx = player_count;
        }

        for (int i = start_idx; i < end_idx; i++) {
            char experience_str[20];
            time_t now = time(NULL);
            int experience = (now - players[i].first_game_time) / (60 * 60 * 24);
            sprintf(experience_str, "%d days", experience);

            if (i < 3) {
                attron(COLOR_PAIR(11));
                if (i == 0) {
                    mvprintw(i - start_idx + 2, 0, "🏆 Legend %-5d %-15s %-7d %-7d %-7d %-10s", i + 1, players[i].name, players[i].score, players[i].gold, players[i].games_completed, experience_str);
                } else if (i == 1) {
                    mvprintw(i - start_idx + 2, 0, "🥈 Goat   %-5d %-15s %-7d %-7d %-7d %-10s", i + 1, players[i].name, players[i].score, players[i].gold, players[i].games_completed, experience_str);
                } else if (i == 2) {
                    mvprintw(i - start_idx + 2, 0, "🥉 Legend %-5d %-15s %-7d %-7d %-7d %-10s", i + 1, players[i].name, players[i].score, players[i].gold, players[i].games_completed, experience_str);
                }
                attroff(COLOR_PAIR(11));
            } else if (strcmp(players[i].name, player.name) == 0) {
                attron(A_BOLD);
                mvprintw(i - start_idx + 2, 0, "%-5d %-15s %-7d %-7d %-7d %-10s <- You", i + 1, players[i].name, players[i].score, players[i].gold, players[i].games_completed, experience_str);
                attroff(A_BOLD);
            } else {
                mvprintw(i - start_idx + 2, 0, "%-5d %-15s %-7d %-7d %-7d %-10s", i + 1, players[i].name, players[i].score, players[i].gold, players[i].games_completed, experience_str);
            }
        }

        mvprintw(15, 0, "Use arrow keys to navigate pages, 'q' to quit.");
        refresh();

        int ch = getch();
        if (ch == 'q') {
            break;
        } else if (ch == KEY_LEFT) {
            if (current_page > 0) {
                current_page--;
            }
        } else if (ch == KEY_RIGHT) {
            if (current_page < total_pages - 1) {
                current_page++;
            }
        }
    }

    clear();
}

GameMap* create_map(int width, int height) {
    GameMap* map = (GameMap*)malloc(sizeof(GameMap));
    map->width = width;
    map->height = height;
    map->room_count = 0;
    map->tiles = (char**)malloc(height * sizeof(char*));
    for (int i = 0; i < height; ++i) {
        map->tiles[i] = (char*)malloc(width * sizeof(char));
        memset(map->tiles[i], ' ', width);
    }
    return map;
}

void free_map(GameMap* map) {
    for (int i = 0; i < map->height; ++i) {
        free(map->tiles[i]);
    }
    free(map->tiles);
    free(map);
}

// int check_room(int x[], int y[], int count) {
//     for (int j = 0; j < count; j++) {
//         for (int q = j + 1; q < count; q++) {
//             // بررسی فاصله بین اتاق‌ها
//             if (abs(x[j] - x[q]) < 35 || abs(y[j] - y[q]) < 35) {
//                 return 0;
//             }
//         }
//     }
//     return 1;
// }
void generate_map(GameMap* map) {
    // پاک کردن نقشه
    for (int y = 0; y < map->height; ++y) {
        for (int x = 0; x < map->width; ++x) {
            map->tiles[y][x] = ' ';
        }
    }

    int attempts = 0;
    int xx[MAX_ROOMS];
    int yy[MAX_ROOMS];
    while (map->room_count < MAX_ROOMS && attempts < 500) {
        int width = MIN_ROOM_SIZE + rand() % (MAX_ROOM_SIZE - MIN_ROOM_SIZE + 1);  
        int height = MIN_ROOM_SIZE + rand() % (MAX_ROOM_SIZE - MIN_ROOM_SIZE + 1); 
        int x = rand() % (MAP_WIDTH - width - 1);
        int y = rand() % (MAP_HEIGHT - height - 1);
        xx[map->room_count] = x;
        yy[map->room_count] = y;

        // بررسی فاصله بین اتاق‌ها
        // int a = check_room(xx, yy, map->room_count + 1);
        // while (a == 0) {
        //     xx[map->room_count] = rand() % (MAP_WIDTH - width - 1);
        //     yy[map->room_count] = rand() % (MAP_HEIGHT - height - 1);
        //     a = check_room(xx, yy, map->room_count + 1);
        // }

        if (width == 0 || height == 0) {
            attempts++;
            continue;
        }

        Room new_room = {x, y, width, height, 0};
        bool overlapping = false;
        for (int i = 0; i < map->room_count; ++i) {
            if (is_overlapping(new_room, map->rooms[i]) || abs(new_room.x - map->rooms[i].x) < 10 || abs(new_room.y - map->rooms[i].y) < 10) {
                overlapping = true;
                break;
            }
        }
        if (!overlapping) {
            create_room(map, x, y, width, height);
            map->rooms[map->room_count++] = new_room;
        }
        attempts++;
    }  

    for (int i = 0; i < map->room_count - 1; ++i) {
        dfs_connect_rooms(map, i);
    }

    //؟؟؟؟؟؟؟
    check_doors_distance(map);
}

void showMessage(const char *message, int duration) {
    move(0, 0);
    clrtoeol();

    mvprintw(0, 0, "%s", message);
    refresh();

    usleep(duration * 1000000);

    move(0, 0);
    clrtoeol();
    refresh();
}

void draw_map(GameMap* map, int highlight) {
    //???
    for (int y = 0; y < 2; ++y) {
        move(y, 0);
        //clrtoeol();
    }

    for (int y = 0; y < map->height; ++y) {
        for (int x = 0; x < map->width; ++x) {
            mvaddch(y + 2, x, map->tiles[y][x]);

//vared kardan namad har character              
            if(map->tiles[y][x] == '^') {
                mvaddch(y + 2, x, '.');
            }
            else if(map->tiles[y][x] == '6') {
                mvaddch(y + 2, x, '^');
            }
            else if(map->tiles[y][x] == 'g') {
                mvprintw(y + 2, x, "\U00002666");
            }
            else if(map->tiles[y][x] == 'G') {
                mvprintw(y + 2, x, "\U00002660");
            }
            else if(map->tiles[y][x] == 'd') {
                mvprintw(y + 2, x, "\U0001F5E1");
            }
            else if(map->tiles[y][x] == 'w') {
                mvprintw(y + 2, x, "\U000023B0");
            }
            else if(map->tiles[y][x] == 'n') {
                mvprintw(y + 2, x, "\U000027B3");
            }
            else if(map->tiles[y][x] == 's') {
                mvprintw(y + 2, x, "\U00002694");
            }
            else if(map->tiles[y][x] == 'H') {
                mvprintw(y + 2, x, "\U00010CE2");
            }
            else if(map->tiles[y][x] == 'S') {
                mvprintw(y + 2, x, "\U0000209B");
            }
            else if(map->tiles[y][x] == 'D') {
                mvprintw(y + 2, x, "\U00010927");
            }
        }
    }

    mvaddch(user.y + 2, user.x, '@');
}

void create_room(GameMap* map, int x, int y, int width, int height) {   
    if (width > MAX_ROOM_SIZE) {
        width = MAX_ROOM_SIZE;
    }
    if (height > MAX_ROOM_SIZE) {
        height = MAX_ROOM_SIZE;
    }

    bool obstacle_added = false;

    for (int i = y; i < y + height; ++i) {
        for (int j = x; j < x + width; ++j) {
           
            if (i == y || i == y + height - 1) {
                // if (!window_added && rand() % 100 < 2 && j != x + 1 && j != x + width - 2 && !obstacle_added) {
                //     map->tiles[i][j] = '=';
                //     window_added = true;
               // }
               //else {
                    map->tiles[i][j] = '-';
                }
            
            else if (j == x || j == x + width - 1) {   //اینجا مثل \ایینی بیاییم شرط بزاریم که راهرو نباشه چهار طرفش
                if (!window_added && rand() % 100 < 2 && i != y + 1 && i != y + height - 2 && !obstacle_added) {
                    map->tiles[i][j] = '=';
                    window_added = true;
                } else {
                    map->tiles[i][j] = '|';
                }
            }

            else {
                map->tiles[i][j] = '.';
            }
        }
    }
     for (int i = 0; i < 2; ++i) {
        int trap_x = x + 1 + rand() % (width - 2);  
        int trap_y = y + 1 + rand() % (height - 2);
        map->tiles[trap_y][trap_x] = '^'; 
    }

    int obstacle_x = x + 1 + rand() % (width - 2); 
    int obstacle_y = y + 1 + rand() % (height - 2);
    if((map->tiles[obstacle_y - 1][obstacle_x] != '+') && (map->tiles[obstacle_y][obstacle_x - 1] != '+') && (map->tiles[obstacle_y + 1][obstacle_x] != '+')&&(map->tiles[obstacle_y][obstacle_x + 1] != '+') ){
        map->tiles[obstacle_y][obstacle_x] = 'O';
    }
    obstacle_added = true;
}

void dfs_connect_rooms(GameMap* map, int room_index) {
    room_status[room_index].visited = 1;
    Room room1 = map->rooms[room_index];

    for (int i = 0; i < map->room_count; i++) {
        if (!room_status[i].visited) {
            Room room2 = map->rooms[i];
            connect_single_room(map, room1, room2);
            dfs_connect_rooms(map, i);
        }
    }

    
}

void connect_single_room(GameMap* map, Room room1, Room room2) {
    int x1 = room1.x + room1.width / 2;
    int y1 = room1.y + room1.height / 2;
    int x2 = room2.x + room2.width / 2;
    int y2 = room2.y + room2.height / 2;

    bool intersection = false;

    while (x1 != x2) {
        if (x1 >= 0 && x1 < map->width && y1 >= 0 && y1 < map->height) {
            if (map->tiles[y1][x1] == '|' || map->tiles[y1][x1] == '-') {
                if (!intersection) {
                    map->tiles[y1][x1] = '+';
                    intersection = true;
                }
            } else if (map->tiles[y1][x1] == ' ') {
                map->tiles[y1][x1] = '#';
                intersection = false;
            }
        }
        x1 += (x2 > x1) ? 1 : -1;
    }
    while (y1 != y2) {
        if (x1 >= 0 && x1 < map->width && y1 >= 0 && y1 < map->height) {
            if (map->tiles[y1][x1] == '|' || map->tiles[y1][x1] == '-') {
                if (!intersection) {
                    map->tiles[y1][x1] = '+';
                    intersection = true;
                }
            } else if (map->tiles[y1][x1] == ' ') {
                map->tiles[y1][x1] = '#';
                intersection = false;
            }
        }
        y1 += (y2 > y1) ? 1 : -1;
    }
}

int is_overlapping(Room room1, Room room2) {
    return !(room1.x + room1.width <= room2.x || room1.x >= room2.x + room2.width ||
             room1.y + room1.height <= room2.y || room1.y >= room2.y + room2.height);
}

void visit_room(GameMap* map, int room_idx) {
    map->rooms[room_idx].visited = 1;
}
int is_room_visited(GameMap* map, int room_idx) {
    return map->rooms[room_idx].visited;
}

Node* create_node(int x, int y, int width, int height) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->rect.x = x;
    node->rect.y = y;
    node->rect.width = width;
    node->rect.height = height;
    node->left = NULL;
    node->right = NULL;
    return node;
}

bool split_node(Node* node) {
    if (node->left != NULL || node->right != NULL) {
        return false;
    }

    if (node->rect.width == 0 || node->rect.height == 0) {
        return false;
    }

    bool split_horizontally = rand() % 2;

    if (node->rect.width != 0 && node->rect.width / (float)node->rect.height >= 1.25) {
        split_horizontally = false;
    } else if (node->rect.height != 0 && node->rect.height / (float)node->rect.width >= 1.25) {
        split_horizontally = true;
    }

    int max = (split_horizontally ? node->rect.height : node->rect.width) - MIN_ROOM_SIZE;
    if (max <= MIN_ROOM_SIZE) {
        return false;
    }

    int split = (rand() % (max - MIN_ROOM_SIZE)) + MIN_ROOM_SIZE;

    if (split_horizontally) {
        node->left = create_node(node->rect.x, node->rect.y, node->rect.width, split);
        node->right = create_node(node->rect.x, node->rect.y + split, node->rect.width, node->rect.height - split);
    } else {
        node->left = create_node(node->rect.x, node->rect.y, split, node->rect.height);
        node->right = create_node(node->rect.x + split, node->rect.y, node->rect.width - split, node->rect.height);
    }

    return true;
}
void create_rooms(Node* node, GameMap* map) {
    if (node->left != NULL || node->right != NULL) {
        if (node->left != NULL) {
            create_rooms(node->left, map);
        }
        if (node->right != NULL) {
            create_rooms(node->right, map);
        }
    } else {
        int width, height, x, y;
        int attempts = 0;
        bool valid_room = false;

        while (!valid_room && attempts < 100) {
            width = MIN_ROOM_SIZE + rand() % (node->rect.width - MIN_ROOM_SIZE + 1);
            height = MIN_ROOM_SIZE + rand() % (node->rect.height - MIN_ROOM_SIZE + 1); 

            if (width > MAX_ROOM_SIZE) {
                width = MAX_ROOM_SIZE;
            }
            if (height > MAX_ROOM_SIZE) {
                height = MAX_ROOM_SIZE;
            }

            x = node->rect.x + rand() % (node->rect.width - width + 1);
            y = node->rect.y + rand() % (node->rect.height - height + 1);

            if (width == 0 || height == 0) {
                attempts++;
                continue;
            }
            if (x + width >= map->width) {
                width = map->width - x - 1;
            }
            if (y + height >= map->height) {
                height = map->height - y - 1;
            }

            Room new_room = {x, y, width, height, 0};
            bool overlapping = false;
            for (int i = 0; i < map->room_count; ++i) {
                if (is_overlapping(new_room, map->rooms[i]) || abs(new_room.x - map->rooms[i].x) < 4 || abs(new_room.y - map->rooms[i].y) < 4) {
                    overlapping = true;
                    break;
                }
            }
            if (!overlapping) {
                node->room = new_room;
                create_room(map, x, y, width, height);
                map->rooms[map->room_count++] = node->room;
                valid_room = true;
            }
            attempts++;
        }
    }
}
void generate_bsp_map(GameMap* map) {
    Node* root = create_node(0, 0, map->width, map->height);
    Node* nodes[256]; 
    int node_count = 0;
    nodes[node_count++] = root;

    while (node_count < MAX_ROOMS) {
        bool split = false;
        for (int i = 0; i < node_count; ++i) {
            if (split_node(nodes[i])) {
                nodes[node_count++] = nodes[i]->left;
                nodes[node_count++] = nodes[i]->right;
                split = true;
                break;
            }
        }
        if (!split) {
            break;
        }
    }

    create_rooms(root, map);

    while (map->room_count < MIN_ROOMS_PER_FLOOR) {
        for (int i = 0; i < node_count; ++i) {
            if (map->room_count >= MIN_ROOMS_PER_FLOOR) {
                break;
            }
            create_rooms(nodes[i], map);
        }
    }
    for (int i = 0; i < MAX_ROOMS; i++) {
        room_status[i].visited = 0;
    }
    dfs_connect_rooms(map, 0);
}
void place_character(GameMap* map) {
    int room_index = rand() % map->room_count;
    Room start_room = map->rooms[room_index];
    user.x = start_room.x + start_room.width / 2;
    user.y = start_room.y + start_room.height / 2;
    previous_tile = map->tiles[user.y][user.x]; 
    map->tiles[user.y][user.x] = '@';

    floors[current_floor].map = map;   //ذخیره اطلاعات طبقه قبلی
    floors[current_floor].start_x = user.x;
    floors[current_floor].start_y = user.y;

    // if (current_floor < MAX_FLOORS - 1) {//قرار دادن راه پله ها
    //     place_stairs(map, room_index, '⇧');
    // }
    // if (current_floor > 0) {
    //     place_stairs(map, room_index, '⇩');
    // }
}
void handle_input() {
    char ch;
    while (ch != 'Q') {
        if(user.hp < 0) {
            break;
        }

        update_status();
        ch = getchar(); 
        int new_x = user.x;
        int new_y = user.y;
        int ox = user.x;
        int oy = user.y;
        switch (ch) {
            case 'w':   
                new_y--;
                break;
            case 's':
                new_y++;
                break;
            case 'a': 
                new_x--;
                break;
            case 'd':
                new_x++;
                break;
            case 'q':  
                new_x--;
                new_y--;
                break;
            case 'e':
                new_x++;
                new_y--;
                break;
            case 'E':
                manage_food_menu();
                break;
            case 'i':      
                manage_inventory();
                break;
            case 'I':
                manage_spellbook();
                break;
            case 'z': 
                new_x--;
                new_y++;
                break;
            case 'x':    
                new_x++;
                new_y++;
                break;
            case 'f': {
                char c = getch();
                if (c == 'w') {
                    while (floors[current_floor].map->tiles[new_y - 1][new_x] == '.' ||
                           floors[current_floor].map->tiles[new_y - 1][new_x] == '#' ||
                           floors[current_floor].map->tiles[new_y - 1][new_x] == '+') {
                        new_y--;
                        user.hp--;
                    }
                } 
                else if (c == 's') {
                    while (floors[current_floor].map->tiles[new_y + 1][new_x] == '.' ||
                           floors[current_floor].map->tiles[new_y + 1][new_x] == '#' ||
                           floors[current_floor].map->tiles[new_y + 1][new_x] == '+') {
                        new_y++;
                        user.hp--;
                    }
                } else if (c == 'a') {
                    while (floors[current_floor].map->tiles[new_y][new_x - 1] == '.' ||
                           floors[current_floor].map->tiles[new_y][new_x - 1] == '#' ||
                           floors[current_floor].map->tiles[new_y][new_x - 1] == '+') {
                        new_x--;
                        user.hp--;
                    }
                } else if (c == 'd') {
                    while (floors[current_floor].map->tiles[new_y][new_x + 1] == '.' ||
                           floors[current_floor].map->tiles[new_y][new_x + 1] == '#' ||
                           floors[current_floor].map->tiles[new_y][new_x + 1] == '+') {
                        new_x++;
                        user.hp--;
                    }
                }
                break;
            }

            default:
                break;
        }

        if (new_x >= 0 && new_x < floors[current_floor].map->width && new_y >= 0 && new_y < floors[current_floor].map->height) {
            char target = floors[current_floor].map->tiles[new_y][new_x];
            if (target == '.' || target == '#' || target == '+' || target == '⇧' || target == '⇩' || target == '^' || target == '6' || target == 'F' || target == 'g' || target == 'G' || target == 'F' || target == 'd'
               || target == 'w' || target == 'n' || target == 's' || target == 'H' || target == 'S' || target == 'D') {
                floors[current_floor].map->tiles[user.y][user.x] = previous_tile;
                previous_tile = target;
                user.x = new_x;
                user.y = new_y;
                floors[current_floor].map->tiles[user.y][user.x] = '@'; 

                if (target == 'g') {
                    int gold_amount = rand() % 20 + 1;
                    gold_count += gold_amount;
                    score += gold_amount;
                    move(0, 0);
                    clrtoeol(); 
                    mvprintw(0, 0, "You picked up %d gold! Total gold: %d", gold_amount, gold_count);
                    refresh();
                    previous_tile = '.';
                } 
                else if (target == 'G') {
                    int black_gold_amount = rand() % 5 + 1;
                    gold_count += black_gold_amount * 5;
                    score += black_gold_amount * 5;
                    move(0, 0);
                    clrtoeol(); 
                    mvprintw(0, 0, "You picked up %d black gold! Total gold: %d", black_gold_amount, gold_count);
                    refresh();
                    previous_tile = '.';
                }
                else if (target == '^') {
                    floors[current_floor].map->tiles[oy][ox] = '6';
                    user.hp -= 10; 
                    move(0, 0);
                    clrtoeol(); 
                    mvprintw(0, 0, "You stepped on a trap! HP: %d", user.hp);
                    refresh();
                }
                else if(target == 'F') {
                    food_count++;
                    previous_tile = '.';
                }
                else if(target == 'd') {
                    
                    char c = getchar();
                    if(c == 'p') { 
                    weapons[1].n++;
                    mvprintw(0, 0, "You picked up a Dagger! Total: %d", weapons[1].n);
                    refresh();    
                   
                    previous_tile = '.';
                    }
                }
                else if(target == 'w') {
                   
                    char c = getchar();
                    if(c == 'p') { 
                    weapons[2].n++;
                    mvprintw(0, 0, "You picked up a Magic Wand! Total: %d", weapons[2].n);
                    refresh();
                   
                    previous_tile = '.';
                }
                }
                else if(target == 'n') {
                    
                     char c = getchar();
                    if(c == 'p') {
                    weapons[3].n++;
                    mvprintw(0, 0, "You picked up a Normal Arrow! Total: %d", weapons[3].n);
                    refresh();    
                    
                    previous_tile = '.';
                }
                }
                else if(target == 's') {
                   
                     char c = getchar();
                    if(c == 'p') { 
                    weapons[4].n++;
                    mvprintw(0, 0, "You picked up a Sword! Total: %d", weapons[4].n);
                    refresh();
                   
                    previous_tile = '.';
                }
                }
                else if(target == 'H') {
                    
                   char c = getchar();
                    if(c == 'p') {
                   spells[0].count++;
                   mvprintw(0, 0, "You picked up a Health Spell! Total: %d", spells[0].count);
                    refresh(); 
                    
                    previous_tile = '.';
                }
                }
                else if(target == 'S') {
                    
                    char c = getchar();
                    if(c == 'p') {
                    spells[1].count++;
                  mvprintw(0, 0, "You picked up a Speed Spell! Total: %d", spells[1].count);
                    refresh();  
                    
                    previous_tile = '.';
                }
                }
                else if(target == 'D') {
                   
                    char c = getchar();
                    if(c == 'p') {
                    spells[2].count++;
                     mvprintw(0, 0, "You picked up a Damage Spell! Total: %d", spells[2].count);
                    refresh();
                    
                    previous_tile = '.';
                }
                }
            }
        }

        hunger_level++;
        if (hunger_level > 100) {
            hunger_level = 100;
            user.hp -= 1;
        }

        draw_map(floors[current_floor].map, 0);
        refresh();
    }
}
// برای پله به طبقه بعدی
// void place_stairs(GameMap* map, int exclude_room_index, char stair_symbol) {
//     int room_index;
//     do {
//         room_index = rand() % map->room_count;
//     } while (room_index == exclude_room_index);
    
//     Room stair_room = map->rooms[room_index];
//     int stair_x = stair_room.x + rand() % stair_room.width;
//     int stair_y = stair_room.y + rand() % stair_room.height;
//     map->tiles[stair_y][stair_x] = stair_symbol;
// }


void check_doors_distance(GameMap* map) {  //؟؟؟؟؟؟؟؟؟؟//
    for (int i = 0; i < map->room_count; ++i) {
        Room room = map->rooms[i];
        int door_x[10], door_y[10];
        int door_count = 0;

        for (int y = room.y; y < room.y + room.height; ++y) {
            for (int x = room.x; x < room.x + room.width; ++x) {
                if (map->tiles[y][x] == '+') {
                    door_x[door_count] = x;
                    door_y[door_count] = y;
                    door_count++;
                }
            }
        }

        for (int j = 0; j < door_count; ++j) {
            for (int k = j + 1; k < door_count; ++k) {
                int dist_x = abs(door_x[j] - door_x[k]);
                int dist_y = abs(door_y[j] - door_y[k]);
                if (dist_x < 10 || dist_y < 10) {
    
                    int new_x = room.x + rand() % room.width;
                    int new_y = room.y + rand() % room.height;
                    map->tiles[door_y[k]][door_x[k]] = '#'; // جایگزینی در قبلی با راهرو؟؟؟؟؟؟؟؟؟؟؟؟
                    map->tiles[new_y][new_x] = '+';
                    door_x[k] = new_x;
                    door_y[k] = new_y;
                    k = -1; // شروع مجدد بررسی از ابتدای لیست درها
                }
            }
        }
    }
}
void place_food(GameMap* map) {
    for (int i = 0; i < map->room_count; ++i) {
        Room room = map->rooms[i];
        int food_x = room.x + 1 + rand() % (room.width - 2);
        int food_y = room.y + 1 + rand() % (room.height - 2);
        if(map->tiles[food_y][food_x] != '.') 
         map->tiles[food_y][food_x] = 'F';
    }
}
void manage_food_menu() {
    clear();
    mvprintw(0, 0, "Food Menu (E to exit)");
    mvprintw(1, 0, "Your hunger level: %d", hunger_level);
    mvprintw(2, 0, "You have %d food items", food_count);
    mvprintw(3, 0, "enter 1 to use the food");
    mvprintw(0, 84, "hunger:");
    for(int i = 0 ; i < (hunger_level / 10); i++) {
        mvaddstr(0,91 + i,"\U000025A3");
    }

    refresh();
    while (1) {
        char c = getch();
        if (c == 'E') {
            break;
        }
        if (c == '1' && food_count > 0) {
            food_count--;
            hunger_level -= 20;
            if (hunger_level < 0) hunger_level = 0;
            user.hp += 10;

            mvprintw(3, 0, "You ate a food item! HP: %d, Hunger Level: %d", user.hp, hunger_level);
            refresh();
        }
    }
}
void update_status() {
    // نمایش HP
    
    mvprintw(0, 179, "HP:   ");
    if (user.hp >= 1000) {
        mvprintw(0, 182, "%d", user.hp);
    } else if (user.hp >= 100) {
        mvprintw(0, 182, " %d", user.hp);
    } else if (user.hp >= 10) {
        mvprintw(0, 182, "  %d", user.hp);
    } else {
        mvprintw(0, 182, "   %d", user.hp);
    }

    // نمایش تعداد غذاها
    mvprintw(1, 177, "Food:  ");
    if (food_count >= 1000) {
        mvprintw(1, 182, "%d", food_count);
    } else if (food_count >= 100) {
        mvprintw(1, 182, " %d", food_count);
    } else if (food_count >= 10) {
        mvprintw(1, 182, "  %d", food_count);
    } else {
        mvprintw(1, 182, "   %d", food_count);
    }

    // نمایش سطح گرسنگی
    mvprintw(2, 175, "Hunger:");
    if (hunger_level >= 1000) {
        mvprintw(2, 182, "%d", hunger_level);
    } else if (hunger_level >= 100) {
        mvprintw(2, 182, " %d", hunger_level);
    } else if (hunger_level >= 10) {
        mvprintw(2, 182, "  %d", hunger_level);
    } else {
        mvprintw(2, 182, "   %d", hunger_level);
    }

    // نمایش تعداد طلاها
    mvprintw(3, 177, "Gold:  ");
    if (gold_count >= 1000) {
        mvprintw(3, 182, "%d", gold_count);
    } else if (gold_count >= 100) {
        mvprintw(3, 182, " %d", gold_count);
    } else if (gold_count >= 10) {
        mvprintw(3, 182, "  %d", gold_count);
    } else {
        mvprintw(3, 182, "   %d", gold_count);
    }

    // نمایش امتیاز
    mvprintw(4, 176, "Score: ");
    if (score >= 1000) {
        mvprintw(4, 182, "%d", score);
    } else if (score >= 100) {
        mvprintw(4, 182, " %d", score);
    } else if (score >= 10) {
        mvprintw(4, 182, "  %d", score);
    } else {
        mvprintw(4, 182, "   %d", score);
    }

    refresh();
}
void place_gold(GameMap* map) {
    for (int i = 0; i < map->room_count; ++i) {
        Room room = map->rooms[i];
        int gold_x = room.x + 1 + rand() % (room.width - 2);
        int gold_y = room.y + 1 + rand() % (room.height - 2);
        if(map->tiles[gold_y][gold_x] == '.')
            map->tiles[gold_y][gold_x] = 'g';

        if (rand() % 100 < 5) { 
            int black_gold_x = room.x + 1 + rand() % (room.width - 2);
            int black_gold_y = room.y + 1 + rand() % (room.height - 2);
            if(map->tiles[black_gold_y][black_gold_x] == '.')
                map->tiles[black_gold_y][black_gold_x] = 'G'; 
                
        }
    }
}
void place_weapons(GameMap* map) {
    for (int i = 0; i < map->room_count; ++i) {
        Room room = map->rooms[i];
        int weapon_x = room.x + 1 + rand() % (room.width - 2);
        int weapon_y = room.y + 1 + rand() % (room.height - 2);
        int weapon_index;

        // Ensure the selected weapon is not Mace
        do {
            weapon_index = rand() % MAX_WEAPONS;
        } while (strcmp(weapons[weapon_index].name, "Mace") == 0);

        if (map->tiles[weapon_y][weapon_x] == '.') {
            map->tiles[weapon_y][weapon_x] = weapons[weapon_index].sign[0];
        }
    }
}
void place_spells(GameMap* map) {
    for (int i = 0; i < map->room_count; ++i) {
        Room room = map->rooms[i];
        int spell_x = room.x + 1 + rand() % (room.width - 2);
        int spell_y = room.y + 1 + rand() % (room.height - 2);
        int spell_index = rand() % MAX_SPELLS;
        if(map->tiles[spell_y][spell_x] == '.')
            map->tiles[spell_y][spell_x] = spells[spell_index].sign[0];
    }
}
void manage_spellbook() {
    clear();
    mvprintw(0, 0, "SpellBook (Press 'I' to exit)");
    for (int i = 0; i < MAX_SPELLS; ++i) {
        mvprintw(i + 1, 0, "%s: %d", spells[i].name, spells[i].count);
    }
    refresh();
    while (1) {
        char c = getch();
        if (c == 'I' || c == 'i') {
            break;
        }
    }
}
void manage_inventory() {
    clear();
    mvprintw(0, 0, "Inventory (Press 'I' to exit)");
    for (int i = 0; i < MAX_WEAPONS; ++i) {
        mvprintw(i + 1, 0, "%s: %d", weapons[i].name, backpack.weapon_count[i]);
    }
    refresh();
    while (1) {
        char c = getch();
        if (c == 'I' || c == 'i') {
            break;
        }
    }
}