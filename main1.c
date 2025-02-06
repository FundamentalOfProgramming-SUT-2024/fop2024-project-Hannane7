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
#define MAX_SPELLS 3
#define MAX_WEAPONS 5
#define MAX_MONSTERS 50

int i = 0;
int food_count = 0, y_food = 0, x_food = 0, z_food = 0;
int hunger_level = 0;
int gold_count = 0; 
int score = 0;       
int h = 0, s = 0, d = 0,h_count = 0, s_count = 0, d_count = 0, step = 0;


typedef struct {
    int rank;
    char username[MAX_LENGTH];
    int score;
    int gold;
    int completed_games;
    int experience;
    char title[MAX_LENGTH];
    char status[MAX_LENGTH];
} ScoreEntry;
typedef struct {
    char name[50], pass[50], email[50];
    int games_completed;
    time_t first_game_time;
} Player;

Player player;

const char *options[] = {
    "Start New Game",
    "Load Game",
    "Guest Login",
    "Score Board",
    "Options",
    "Profile", 
    "Exit",
};


int n_options = sizeof(options) / sizeof(options[0]);
    

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

typedef struct {
    char name[20];
    char sign[5];
    int count;
} Spell;

Spell spells[] = {
    {"Health", "H", 0},
    {"Speed", "V", 0},
    {"Damage", "N", 0}
};

typedef struct {
    Spell inventory[MAX_SPELLS];
    int spell_count[MAX_SPELLS];
} SpellBook;

SpellBook spellbook = {{{"Health", "H", 0}, {"Speed", "S", 0}, {"Damage", "D", 0}}, {0}};

typedef struct {
    char type;
    int x, y;
    int hp;
    int damage;
    int follow_steps;
    int max_follow_steps;
    bool exhausted;
    bool active; // اضافه شد
    char previous_tile; // ذخیره خانه قبلی
} Monster;

Monster monsters[MAX_MONSTERS];
int monster_count = 0;

Floor floors[MAX_FLOORS];
int current_floor = 0;

char previous_tile = '.';
int difficulty_level = 1;

//int n_options = sizeof(options) / sizeof(options[0]);
int n_difficulty_levels = sizeof(difficulty_levels) / sizeof(difficulty_levels[0]);
int n_colors = sizeof(colors) / sizeof(colors[0]);
int n_songs = sizeof(songs) / sizeof(songs[0]);
bool full_map_view = false;
bool visited[MAP_HEIGHT][MAP_WIDTH];
bool player_in_new_room = false;
int hero_color = 12;

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
void init_monster(Monster *monster, char type, int x, int y); 
void spawn_monster(GameMap* map, char type, int x, int y);
void move_monsters(GameMap* map);
void mark_visited(GameMap* map, int x, int y);
void initialize_visited();
void reveal_room(GameMap* map, int room_idx) ;
void reveal_corridors(GameMap* map, int x, int y) ;
void make_scoreboard();
void scoreboard();
void move_monster_towards_player(GameMap* map, Monster *monster);
bool is_player_in_room(GameMap* map, int x, int y);
void move_snake(GameMap* map, Monster *snake);
void show_profile();
void show_game_over();
//void show_score_board();
void save_score(const char *filename, const char *player_name, int score);
int load_scores(const char *filename, ScoreEntry *scores, int max_scores);
void show_score_board() ;

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

    // init colors 
    
  
    init_pair(10, 3, COLOR_BLACK);
    init_color(5, 1000, 0, 0);
    init_pair(11, 5, COLOR_BLACK);
    init_color(15, 1000, 1000, 0);
    init_pair (13, 15,COLOR_BLACK);
    init_color(3, 700, 0, 1000);
    init_color(8, 1000, 650, 0);
    init_color(9,961, 961, 863);
    init_pair(15, 8, COLOR_BLACK);
    init_pair(16, 9, COLOR_BLACK);
    init_pair(17, COLOR_BLUE, COLOR_BLACK);
    init_color(1, 400, 200, 800); 
    init_pair(18, 1, COLOR_BLACK);
    init_color(2, 972, 931, 1000); 
    init_pair(19, 2, COLOR_BLACK);
    init_color(30, 805, 522, 247); 
    init_pair(20, 30, COLOR_BLACK);
    init_color(31, 1000, 400, 400); 
    init_pair(21, 31, COLOR_BLACK);
    init_color(33, 1000, 270, 0);
    init_pair(23, 33, COLOR_BLACK);
    init_color(32, 98, 98, 439);
    init_pair(22, 32, COLOR_BLACK);
    init_color(34, 223, 1000, 78); 
    init_pair(24, 34, COLOR_BLACK);
    init_color(35, 800, 0, 1000);
    init_pair(25, 35, COLOR_BLACK);
    init_color(36, 1000, 200, 600); 
    init_pair(26, 36, COLOR_BLACK);
    init_color(37,823,411,400);
    init_pair(27, 37, COLOR_BLACK);
    init_color(28, 300, 300, 300); 
    init_pair(28, 28, COLOR_BLACK);
    init_color(70,502, 502, 502);
    init_pair(29, 70, COLOR_BLACK);
    init_pair(12, COLOR_GREEN, COLOR_BLACK);
    init_color(40, 0,0,900);
    init_pair(41, 40, COLOR_BLACK);
    init_color(42, 566, 934, 566); // سبز روشن
    init_pair(43, 40, COLOR_BLACK);
    init_color(44, 1000, 843, 0); // طلایی
    init_pair(45, 44, COLOR_BLACK);
    init_color(46, 541, 169, 887); // بنفش روشن
    init_pair(47, 46, COLOR_BLACK);
    init_color(48, 647, 165, 165); // قرمز قهوه‌ای
    init_pair(49, 48, COLOR_BLACK);
    init_color(50, 753, 753, 753); // خاکستری روشن
    init_pair(51, 50, COLOR_BLACK);
    init_color(89, 250, 0, 0);
    init_pair(90, 7, COLOR_BLACK);
    init_color(91, 100,0, 900);
    init_color(93, 1000,1000,1000);
    init_pair(92,91 ,93);

    initialize_visited();

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
    attron(COLOR_PAIR(41)); 
    print_menu(menu_win, options, n_options, highlight);
    attroff(COLOR_PAIR(41));
    
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
        get_player_info();
    } else if (choice == 2) {
        load_game("savefile.dat");
    } else if (choice == 3) {
        guest_login();
    }if (choice == 4) {
    show_score_board();
    } else if (choice == 5) {
        show_options_menu();
    } else if (choice == 6) {
        show_profile();
    } else if (choice == 7) {
        endwin();
        exit(0);
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
            wattron(menu_win, COLOR_PAIR(92));
            mvwprintw(menu_win, y, x, "%s", menu_items[i]);
            wattroff(menu_win, A_REVERSE);
            wattroff(menu_win, COLOR_PAIR(92));
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
    attron(COLOR_PAIR(41));
    print_menu(options_win, options_menu, n_options_menu, highlight);
    attroff(COLOR_PAIR(41));
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
        attron(COLOR_PAIR(41));
        print_menu(options_win, options_menu, n_options_menu, highlight);
        attroff(COLOR_PAIR(41));
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

void show_profile() { 
    clear();
    mvprintw(0, 0, "Profile:");
   // mvprintw(1, 0, "Username: %s", player.name);
    mvprintw(1, 0, "Username: %s", "hannane");
    //mvprintw(2, 0, "Email: %s", player.email);
    mvprintw(2, 0, "Email: %s", "hannanenzmd@gmail.com");
    mvprintw(3, 0, "player's score:%d", 300);
    mvprintw(4, 0, "player's gold:%d", 200);
    mvprintw(5, 0, "player's exp:%d", 10);

    refresh();
    getch();
    clear();
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

    difficulty_level = choice;

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

    if (choice == 1) {
        hero_color = 11;
    } else if (choice == 2) {
        hero_color = 12;
    } else if (choice == 3) {
        hero_color =41 ;
    }

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
    attron(COLOR_PAIR(10));
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
            move (0,17);
            clrtoeol();
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

        if (!(has_digit) || !(has_upper) || !(has_lower)) {  
            attron(COLOR_PAIR(11));
            mvprintw(3, 0, "Error: password must contain at least one digit, one uppercase letter, and one lowercase letter.");
            attroff(COLOR_PAIR(11));
        } else {
          move(3,0);
          clrtoeol();
        }
        clrtoeol();
        refresh();
    }

    while (1) {
        attron(COLOR_PAIR(10));
        mvprintw(3, 0, "Enter your email: ");
        refresh();
        scanw("%s", player.email);

        if (validate_email(player.email)) {
            clrtoeol();
            break;
        } else {
            attron(COLOR_PAIR(11));
            mvprintw(5, 0, "Error: Invalid email format. Please enter a valid email.");
            attroff(COLOR_PAIR(11));
        }
        refresh();
    }
    clear();
    showMessage("New Game Started! Welcome, ", 3);

    if (difficulty_level == 1) {
        user.hp = 1000;
    } else if (difficulty_level == 2) {
        user.hp = 600;
    } else if (difficulty_level == 3) {
        user.hp = 300;
    }

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
    srand(time(0));

    for (int i = 0; i < length; i++) {
        int idx = rand() % (sizeof(characters) - 1);
        password[i] = characters[idx];
    }
    password[length] = '\0'; 

    mvprintw(2,0,"Generated Password: %s", password);
    
    refresh();
}

void load_game() {
    echo();
    char name[MAX_LENGTH], pass[MAX_LENGTH];

    mvprintw(0, 0, "Enter your name (or press 'F' to recover password): ");
    refresh();
    scanw("%s", name);

    if (strcmp(name, "F") == 0) {
        char email[MAX_LENGTH];
        mvprintw(1, 0, "Enter your email: ");
        refresh();
        scanw("%s", email);

        FILE *file = fopen(FILE_NAME, "r");
        if (file == NULL) {
            attron(COLOR_PAIR(11)); 
            mvprintw(2, 0, "Error: Unable to open file.");
            attroff(COLOR_PAIR(11)); 
            refresh();
            getch();
            clear();
            main(); 
            return;
        }

        Player temp;
        bool email_found = false;
        while (fscanf(file, "%s %s %s", temp.name, temp.pass, temp.email) != EOF) {
            if (strcmp(temp.email, email) == 0) {
                email_found = true;
                break;
            }
        }
        fclose(file);

        if (!email_found) {
            attron(COLOR_PAIR(11));
            mvprintw(2, 0, "Error: Email not found.");
            attroff(COLOR_PAIR(11));
            getch();
            clear();
            main();
            return;
        }

        mvprintw(2, 0, "Your password is: %s", temp.pass);
        refresh();
        getch();
        clear();
        main();
        return;
    }

    FILE *file = fopen(FILE_NAME, "r");
    if (file == NULL) {
        attron(COLOR_PAIR(11)); 
        mvprintw(1, 0, "Error: Unable to open file.");
        attroff(COLOR_PAIR(11)); 
        refresh();
        getch();
        clear();
        main(); 
        return;
    }

    Player temp;
    bool user_exists = false;
    while (fscanf(file, "%s %s %s", temp.name, temp.pass, temp.email) != EOF) {
        if (strcmp(temp.name, name) == 0) {
            user_exists = true;
            break;
        }
    }
    fclose(file);

    if (!user_exists) {
        attron(COLOR_PAIR(11));
        mvprintw(1, 0, "Error: Username not found, please start a new game.");
        attroff(COLOR_PAIR(11));
        getch();
        clear();
        main();
        return;
    }
    mvprintw(1, 0, "Enter your password: ");
    refresh();
    scanw("%s", pass);

    if (strcmp(temp.pass, pass) != 0) {
        attron(COLOR_PAIR(11));
        mvprintw(2, 0, "Error: Invalid password.");
        attroff(COLOR_PAIR(11)); 
        refresh();
        getch();
        clear();
        main();
        return;
    }

    mvprintw(2, 0, "Login successful! Loading game...");
    refresh();
    sleep(1);

    strcpy(player.name, temp.name); 
    strcpy(player.email, temp.email);  

    if (difficulty_level == 1) {
        user.hp = 1000;
    } else if (difficulty_level == 2) {
        user.hp = 600;
    } else if (difficulty_level == 3) {
        user.hp = 300;
    }

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

    if (difficulty_level == 1) {
        user.hp = 1000;
    } else if (difficulty_level == 2) {
        user.hp = 600;
    } else if (difficulty_level == 3) {
        user.hp = 300;
    }

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

void show_game_over() {
    clear();
    attron(COLOR_PAIR(11));
    mvprintw(LINES / 2, (COLS - strlen("Game Over!")) / 2, "Game Over!");
    attroff(COLOR_PAIR(11));
    refresh();
    
    save_score("scores.txt", player.name, score);
    getch();
    endwin(); 
    exit(0); 
}

// void show_score_board() {
//     clear();
//     FILE *file = fopen(SCORES_FILE, "r");
//     if (file == NULL) {
//         mvprintw(0, 0, "Error: Unable to open scores file.");
//         refresh();
//         getch();
//         return;
//     }
//     typedef struct {  
//         char name[MAX_LENGTH];
//         int score;
//         int gold;
//         int games_completed;
//         time_t first_game_time;
//     } PlayerScore;

//     PlayerScore players[100];
//     int player_count = 0;

//     while (fscanf(file, "%s %d %d %d %ld", players[player_count].name, &players[player_count].score, &players[player_count].gold, &players[player_count].games_completed, &players[player_count].first_game_time) != EOF) {
//         player_count++;
//     }
//     fclose(file);

//     for (int i = 0; i < player_count - 1; i++) {
//         for (int j = i + 1; j < player_count; j++) {
//             if (players[i].score < players[j].score) {
//                 PlayerScore temp = players[i];
//                 players[i] = players[j];
//                 players[j] = temp;
//             }
//         }
//     }

//     int current_page = 0;
//     int total_pages = (player_count + USERS_PER_PAGE - 1) / USERS_PER_PAGE;

//     while (1) {
//         clear();
//         mvprintw(0, 0, "Score Board (Page %d of %d)", current_page + 1, total_pages);
//         mvprintw(1, 0, "Rank  Username       Score   Gold    Games   Experience");

//         int start_idx = current_page * USERS_PER_PAGE;
//         int end_idx = start_idx + USERS_PER_PAGE;
//         if (end_idx > player_count) {
//             end_idx = player_count;
//         }

//         for (int i = start_idx; i < end_idx; i++) {
//             char experience_str[20];
//             time_t now = time(NULL);
//             int experience = (now - players[i].first_game_time) / (60 * 60 * 24);
//             sprintf(experience_str, "%d days", experience);

//             if (i < 3) {
//                 attron(COLOR_PAIR(11));
//                 if (i == 0) {
//                     mvprintw(i - start_idx + 2, 0, "🏆 Legend %-5d %-15s %-7d %-7d %-7d %-10s", i + 1, players[i].name, players[i].score, players[i].gold, players[i].games_completed, experience_str);
//                 } else if (i == 1) {
//                     mvprintw(i - start_idx + 2, 0, "🥈 Goat   %-5d %-15s %-7d %-7d %-7d %-10s", i + 1, players[i].name, players[i].score, players[i].gold, players[i].games_completed, experience_str);
//                 } else if (i == 2) {
//                     mvprintw(i - start_idx + 2, 0, "🥉 Legend %-5d %-15s %-7d %-7d %-7d %-10s", i + 1, players[i].name, players[i].score, players[i].gold, players[i].games_completed, experience_str);
//                 }
//                 attroff(COLOR_PAIR(11));
//             } else if (strcmp(players[i].name, player.name) == 0) {
//                 attron(A_BOLD);
//                 mvprintw(i - start_idx + 2, 0, "%-5d %-15s %-7d %-7d %-7d %-10s <- You", i + 1, players[i].name, players[i].score, players[i].gold, players[i].games_completed, experience_str);
//                 attroff(A_BOLD);
//             } else {
//                 mvprintw(i - start_idx + 2, 0, "%-5d %-15s %-7d %-7d %-7d %-10s", i + 1, players[i].name, players[i].score, players[i].gold, players[i].games_completed, experience_str);
//             }
//         }

//         mvprintw(15, 0, "Use arrow keys to navigate pages, 'q' to quit.");
//         refresh();

//         int ch = getch();
//         if (ch == 'q') {
//             break;
//         } else if (ch == KEY_LEFT) {
//             if (current_page > 0) {
//                 current_page--;
//             }
//         } else if (ch == KEY_RIGHT) {
//             if (current_page < total_pages - 1) {
//                 current_page++;
//             }
//         }
//     }

//     clear();
// }

void save_score(const char *filename, const char *player_name, int score) {
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        perror("Failed to open score file for saving");
        return;
    }

    fprintf(file, "%s %d\n", player_name, score);
    fclose(file);
}

int load_scores(const char *filename, ScoreEntry *scores, int max_scores) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open score file for loading");
        return 0;
    }

    int count = 0;
    char buffer[256];
    fgets(buffer, sizeof(buffer), file);  // Skip the header line

    while (count < max_scores && fgets(buffer, sizeof(buffer), file)) {
        int items_read = sscanf(buffer, "%d %s %d %d %d %d %s %s", 
                                &scores[count].rank, scores[count].username, &scores[count].score, &scores[count].gold, 
                                &scores[count].completed_games, &scores[count].experience, scores[count].title, scores[count].status);
        // If title or status are missing, set them to empty strings
        if (items_read < 7) {
            strcpy(scores[count].title, "");
        }
        if (items_read < 8) {
            strcpy(scores[count].status, "");
        }
        count++;
    }

    fclose(file);
    return count;
}
void show_score_board() {
    clear();
    mvprintw(0, 0, "-------------------------------------------------------------------------------------------------------------------");
    mvprintw(1, 0, "| Rank | Username   | Score | Gold  | Completed Games | Experience | Title      | Status |");
    mvprintw(2, 0, "-------------------------------------------------------------------------------------------------------------------");

    ScoreEntry scores[USERS_PER_PAGE];
    int score_count = load_scores(SCORES_FILE, scores, USERS_PER_PAGE);

    for (int i = 0; i < score_count; i++) {
        if (i == 0) {
            attron(COLOR_PAIR(45) | A_BOLD);
        } else if (i == 1) {
            attron(COLOR_PAIR(23) | A_BOLD); 
        } else if (i == 2) {
            attron(COLOR_PAIR(26) | A_BOLD); 
        } else {
            attron(COLOR_PAIR(47));
        }

        mvprintw(i + 3, 0, "| %-4d | %-10s | %-5d | %-5d | %-15d | %-10d | %-10s | %-6s |", 
                 scores[i].rank, scores[i].username, scores[i].score, scores[i].gold, scores[i].completed_games, scores[i].experience, scores[i].title, scores[i].status);
        mvprintw(i + 4, 0, "-------------------------------------------------------------------------------------------------------------------");
        
        attroff(COLOR_PAIR(45) | A_BOLD);
        attroff(COLOR_PAIR(23) | A_BOLD);
        attroff(COLOR_PAIR(26) | A_BOLD);
        attroff(COLOR_PAIR(47));
    }

    refresh();
    getch();
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
    for (int y = 0; y < 2; ++y) {
        move(y, 0);
    }

    for (int y = 0; y < map->height; ++y) {
        for (int x = 0; x < map->width; ++x) {
            if (full_map_view || visited[y][x]) {
                char tile = map->tiles[y][x];
                
                switch (tile) {
                    case 'f':  
                    case 'p':
                    attron(COLOR_PAIR(43));
                      mvaddch(y + 2, x, 'f');
                      attroff(COLOR_PAIR(43));
                      attron(COLOR_PAIR(10));
                        break;
                    case 'x':
                    attron(COLOR_PAIR(45));
                     mvprintw(y + 2, x, "\U00001598");
                    attroff(COLOR_PAIR(45));
                    attron(COLOR_PAIR(10));
                        break;
                    case 'm':
                    attron(COLOR_PAIR(47));
                    mvprintw(y + 2, x, "\U000015F0");
                    attroff(COLOR_PAIR(47));
                    attron(COLOR_PAIR(10));
                        break;
                         case 'O':
                    attron(COLOR_PAIR(49));
                     mvaddch(y + 2, x, 'O');
                    attroff(COLOR_PAIR(49));
                    attron(COLOR_PAIR(10));
                        break;
                    case '-':
                        attron(COLOR_PAIR(27));
                        mvaddch(y + 2, x, '-');
                        attroff(COLOR_PAIR(27));
                        attron(COLOR_PAIR(10));
                        break;
                    case '|':
                        attron(COLOR_PAIR(27)); // تغییر رنگ دیوارهای عمودی
                        mvaddch(y + 2, x, '|');
                        attroff(COLOR_PAIR(27));
                        attron(COLOR_PAIR(10)); // بازگرداندن رنگ پیش‌فرض
                        break;
                    case '#':
                        attron(COLOR_PAIR(29)); // تغییر رنگ راهروها به سفید
                        mvaddch(y + 2, x, '#');
                        attroff(COLOR_PAIR(29));
                        attron(COLOR_PAIR(10));
                        break;
                    case '^':
                        attron(COLOR_PAIR(28)); // تغییر رنگ راهروها به سفید
                        mvaddch(y + 2, x, '.');
                        attroff(COLOR_PAIR(28)); // تغییر رنگ راهروها به سفید
                        attron(COLOR_PAIR(10)); // تغییر رنگ راهروها به سفید

                        break;
                    case '6':
                        attron(COLOR_PAIR(11));
                        mvaddch(y + 2, x, '^');
                        attroff(COLOR_PAIR(11));
                        attron(COLOR_PAIR(10));
                        break;
                    case 'g':
                        attron(COLOR_PAIR(13));
                        mvprintw(y + 2, x, "\U00002666");
                        attroff(COLOR_PAIR(13));
                        attron(COLOR_PAIR(10));
                        break;
                    case 'B':
                        attron(COLOR_PAIR(90));
                        mvprintw(y + 2, x, "\U00002660");
                        attroff(COLOR_PAIR(90));
                        attron(COLOR_PAIR(10));
                        break;
                    case 'd':
                        attron(COLOR_PAIR(15));
                        mvprintw(y + 2, x, "\U0001F5E1");
                        attroff(COLOR_PAIR(15));
                        attron(COLOR_PAIR(10));
                        break;
                    case 'w':
                        attron(COLOR_PAIR(16));
                        mvprintw(y + 2, x, "\U000023B0");
                        attroff(COLOR_PAIR(16));
                        attron(COLOR_PAIR(10));
                        break;
                    case 'n':
                        attron(COLOR_PAIR(17));
                        mvprintw(y + 2, x, "\U000027B3");
                        attroff(COLOR_PAIR(17));
                        attron(COLOR_PAIR(10));
                        break;
                    case 's':
                        attron(COLOR_PAIR(18));
                        mvprintw(y + 2, x, "\U00002694");
                        attroff(COLOR_PAIR(18));
                        attron(COLOR_PAIR(10));
                        break;
                    case 'H':
                        attron(COLOR_PAIR(19));
                        mvprintw(y + 2, x, "\U00010CE2");
                        attroff(COLOR_PAIR(19)); 
                        attron(COLOR_PAIR(10));
                        break;
                    case 'V':
                        attron(COLOR_PAIR(20));
                        mvprintw(y + 2, x, "\U0000209B");
                        attroff(COLOR_PAIR(20));
                        attron(COLOR_PAIR(10));
                        break;
                    case 'N':
                        attron(COLOR_PAIR(21));
                        mvprintw(y + 2, x, "\U00010927");
                        attroff(COLOR_PAIR(21));
                        attron(COLOR_PAIR(10));
                        break;
                    case 'D':
                        attron(COLOR_PAIR(22));
                        mvaddch(y + 2, x, 'D');
                        attroff(COLOR_PAIR(22));
                        attron(COLOR_PAIR(10));
                        break;
                    case 'F':
                        attron(COLOR_PAIR(23));
                        mvaddch(y + 2, x, 'F');
                        attroff(COLOR_PAIR(23));
                        attron(COLOR_PAIR(10));
                        break;
                    case 'G':
                        attron(COLOR_PAIR(24));
                        mvaddch(y + 2, x, 'G');
                        attroff(COLOR_PAIR(24)); 
                        attron(COLOR_PAIR(10));
                        break;
                    case 'S':
                        attron(COLOR_PAIR(25));
                        mvaddch(y + 2, x, 'S');
                        attroff(COLOR_PAIR(25));
                        attron(COLOR_PAIR(10));
                        break;
                    case 'U':
                        attron(COLOR_PAIR(26));
                        mvaddch(y + 2, x, 'U');
                        attroff(COLOR_PAIR(26));
                        attron(COLOR_PAIR(10));
                        break;
                    case '.':
                        attron(COLOR_PAIR(28)); // تغییر رنگ کف زمین
                        mvaddch(y + 2, x, '.');
                        attroff(COLOR_PAIR(28));
                        attron(COLOR_PAIR(10));
                        break;
                    default:
                        mvaddch(y + 2, x, tile);
                        break;
                }
            } else {
                mvaddch(y + 2, x, ' ');
            }
        }
    }
     attroff(COLOR_PAIR(10));
    attron(COLOR_PAIR(hero_color)); // استفاده از رنگ انتخابی برای کاراکتر
    mvprintw(user.y + 2, user.x, "@");
    refresh();
    attroff(COLOR_PAIR(hero_color));
    attron(COLOR_PAIR(10));
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
                    map->tiles[i][j] = '-';
                }
            
            else if (j == x || j == x + width - 1) { 
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

    for (int i = 0; i < map->room_count; ++i) {
        Room room = map->rooms[i];
        int monster_x = room.x + 1 + rand() % (room.width - 2);
        int monster_y = room.y + 1 + rand() % (room.height - 2);
        char monster_types[] = {'D', 'F', 'G', 'S', 'U'};
        char monster_type = monster_types[rand() % 5];
        spawn_monster(map, monster_type, monster_x, monster_y);
    }
}
void place_character(GameMap* map) {
    int room_index = rand() % map->room_count;
    Room start_room = map->rooms[room_index];
    user.x = start_room.x + start_room.width / 2;
    user.y = start_room.y + start_room.height / 2;
    previous_tile = map->tiles[user.y][user.x]; 
    map->tiles[user.y][user.x] = '@';

    floors[current_floor].map = map; 
    floors[current_floor].start_x = user.x;
    floors[current_floor].start_y = user.y;
}
void handle_input() {
    char ch;
    while (ch != 'Q') {
        if (user.hp <= 0) {
            show_game_over(); 
        }
        if (h_count != 10 && h == 1) {
            h_count++;
        } else {
            h_count = 0;
            h = 0;
        }
        if (s_count != 10 && s == 1) {
            s_count++;
        } else {
            s_count = 0;
            s = 0;
        }
        if (d_count != 10 && d == 1) {
            d_count++;
        } else {
            d_count = 0;
            d = 0;
        }
        if (hunger_level < 100 && user.hp + 15 < 1000) {
            if (h) {
                user.hp += 30;
            } else {
                user.hp += 15;
            }
        }
        update_status();
        ch = getchar();
        int new_x = user.x;
        int new_y = user.y;
        int ox = user.x;
        int oy = user.y;
        switch (ch) {
            case 'w':
                if (s) {
                    new_y -= 2;
                } else {
                    new_y--;
                }
                break;
            case 's':
                if (s) {
                    new_y += 2;
                } else {
                    new_y++;
                }
                break;
            case 'a':
                if (s) {
                    new_x -= 2;
                } else {
                    new_x--;
                }
                break;
            case 'd':
                if (s) {
                    new_x += 2;
                } else {
                    new_x++;
                }
                break;
            case 'q':
                if (s) {
                    new_y -= 2;
                    new_x -= 2;
                } else {
                    new_y--;
                    new_x--;
                }
                break;
            case 'e':
                if (s) {
                    new_y -= 2;
                    new_x += 2;
                } else {
                    new_y--;
                    new_x++;
                }
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
                if (s) {
                    new_y += 2;
                    new_x -= 2;
                } else {
                    new_y++;
                    new_x--;
                }
                break;
            case 'x':
                if (s) {
                    new_y += 2;
                    new_x += 2;
                } else {
                    new_y++;
                    new_x++;
                }
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
                } else if (c == 's') {
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
            case 'M':
                full_map_view = !full_map_view;
                break;
            default:
                break;
        }

        if (new_x >= 0 && new_x < floors[current_floor].map->width && new_y >= 0 && new_y < floors[current_floor].map->height) {
            char target = floors[current_floor].map->tiles[new_y][new_x];
            if (target == '.' || target == '#' || target == '+' || target == '^' || target == '6' || target == 'f' || target == 'g' || target == 'B' || target == 'f' || target == 'd'
               || target == 'w' || target == 'n' || target == 's' || target == 'H' || target == 'V' || target == 'N' || target == 'F' || target == 'U' || target == 'S' || target == 'D' || target == 'G' || target == 'p' || target == 'x' || target == 'm' ) {
                floors[current_floor].map->tiles[user.y][user.x] = previous_tile;
                previous_tile = target;
                user.x = new_x;
                user.y = new_y;
                floors[current_floor].map->tiles[user.y][user.x] = '@';

                mark_visited(floors[current_floor].map, user.x, user.y);

                for (int i = 0; i < floors[current_floor].map->room_count; ++i) {
                    if (user.x >= floors[current_floor].map->rooms[i].x &&
                        user.x < floors[current_floor].map->rooms[i].x + floors[current_floor].map->rooms[i].width &&
                        user.y >= floors[current_floor].map->rooms[i].y &&
                        user.y < floors[current_floor].map->rooms[i].y + floors[current_floor].map->rooms[i].height) {
                        reveal_room(floors[current_floor].map, i);
                        
                    
                        for (int j = 0; j < monster_count; ++j) {
                            Monster *monster = &monsters[j];
                            if (monster->x >= floors[current_floor].map->rooms[i].x &&
                                monster->x < floors[current_floor].map->rooms[i].x + floors[current_floor].map->rooms[i].width &&
                                monster->y >= floors[current_floor].map->rooms[i].y &&
                                monster->y < floors[current_floor].map->rooms[i].y + floors[current_floor].map->rooms[i].height) {
                                monster->active = true;
                            }
                        }
                        player_in_new_room = true;
                        break;
                    }
                }
                reveal_corridors(floors[current_floor].map, user.x, user.y);

                if (target == 'g') {
                    int gold_amount = rand() % 20 + 1;
                    gold_count += gold_amount;
                    score += gold_amount;
                    move(0, 0);
                    clrtoeol();
                    mvprintw(0, 0, "You picked up %d gold! Total gold: %d", gold_amount, gold_count);
                    refresh();
                    previous_tile = '.';
                } else if (target == 'B') {
                    int black_gold_amount = rand() % 5 + 1;
                    gold_count += black_gold_amount * 5;
                    score += black_gold_amount * 5;
                    move(0, 0);
                    clrtoeol();
                    mvprintw(0, 0, "You picked up %d black gold! Total gold: %d", black_gold_amount, gold_count);
                    refresh();
                    previous_tile = '.';
                } else if (target == '^') {
                    floors[current_floor].map->tiles[oy][ox] = '6';
                    user.hp -= 20;
                    move(0, 0);
                    clrtoeol();
                    mvprintw(0, 0, "You stepped on a trap! HP: %d", user.hp);
                    refresh();
                } else if (target == 'f') {
                    food_count++;
                    previous_tile = '.';
                } else if (target == 'm') {
                    x_food++;
                    previous_tile = '.';
                } else if (target == 'x') {
                    z_food++;
                    previous_tile = '.';
                } else if (target == 'p') {
                    y_food++;
                    previous_tile = '.';
                } else if (target == 'd') {
                    char c = getchar();
                    if (c == 'p') {
                        weapons[1].n++;
                        move(0, 0);
                        clrtoeol();
                        mvprintw(0, 0, "You picked up a Dagger! Total: %d", weapons[1].n);
                        refresh();

                        previous_tile = '.';
                    }
                } else if (target == 'w') {
                    char c = getchar();
                    if (c == 'p') {
                        weapons[2].n++;
                        move(0, 0);
                        clrtoeol();
                        mvprintw(0, 0, "You picked up a Magic Wand! Total: %d", weapons[2].n);
                        refresh();

                        previous_tile = '.';
                    }
                } else if (target == 'n') {
                    char c = getchar();
                    if (c == 'p') {
                        weapons[3].n++;
                        move(0, 0);
                        clrtoeol();
                        mvprintw(0, 0, "You picked up a Normal Arrow! Total: %d", weapons[3].n);
                        refresh();

                        previous_tile = '.';
                    }
                } else if (target == 's') {
                    char c = getchar();
                    if (c == 'p') {
                        weapons[4].n++;
                        move(0, 0);
                        clrtoeol();
                        mvprintw(0, 0, "You picked up a Sword! Total: %d", weapons[4].n);
                        refresh();

                        previous_tile = '.';
                    }
                } else if (target == 'H') {
                    char c = getchar();
                    if (c == 'p') {
                        spells[0].count++;
                        move(0, 0);
                        clrtoeol();
                        mvprintw(0, 0, "You picked up a Health Spell! Total: %d", spells[0].count);
                        refresh();

                        previous_tile = '.';
                    }
                } else if (target == 'V') {
                    char c = getchar();
                    if (c == 'p') {
                        spells[1].count++;
                        move(0, 0);
                        clrtoeol();
                        mvprintw(0, 0, "You picked up a Speed Spell! Total: %d", spells[1].count);
                        refresh();

                        previous_tile = '.';
                    }
                } else if (target == 'N') {
                    char c = getchar();
                    if (c == 'p') {
                        spells[2].count++;
                        move(0, 0);
                        clrtoeol();
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

        move_monsters(floors[current_floor].map); 
    }
}

void place_food(GameMap* map) {
    for (int i = 0; i < map->room_count; ++i) {
        Room room = map->rooms[i];
        int food_x = room.x + 1 + rand() % (room.width - 2);
        int food_y = room.y + 1 + rand() % (room.height - 2);
        if(map->tiles[food_y][food_x] != '.') 
         map->tiles[food_y][food_x] = 'f';
    }
        for (int i = 0; i < map->room_count; ++i) {
        Room room = map->rooms[i];
        int food_x = room.x + 1 + rand() % (room.width - 2);
        int food_y = room.y + 1 + rand() % (room.height - 2);
        if(map->tiles[food_y][food_x] != '.') 
         map->tiles[food_y][food_x] = 'x';
    }
        for (int i = 0; i < map->room_count; ++i) {
        Room room = map->rooms[i];
        int food_x = room.x + 1 + rand() % (room.width - 2);
        int food_y = room.y + 1 + rand() % (room.height - 2);
        if(map->tiles[food_y][food_x] != '.') 
         map->tiles[food_y][food_x] = 'm';
    }
        for (int i = 0; i < map->room_count; ++i) {
        Room room = map->rooms[i];
        int food_x = room.x + 1 + rand() % (room.width - 2);
        int food_y = room.y + 1 + rand() % (room.height - 2);
        if(map->tiles[food_y][food_x] != '.') 
         map->tiles[food_y][food_x] = 'p';
    }
}

void manage_food_menu() {
    clear();
    mvprintw(15, 73, "hunger:");
    for(int i = 0 ; i < (hunger_level / 10); i++) {
        mvaddstr(15,80 + i,"\u25A3");
    }
    mvprintw(17, 60, "Type");
    mvprintw(17, 76, "numbers saved");
    mvprintw(17, 100, "health increase");

    attron(COLOR_PAIR(22)); 
    mvprintw(19, 60, "1:regular food f"); 
    mvprintw(19, 82, "%d", food_count + y_food);
    mvprintw(19, 105, "50");
    attroff(COLOR_PAIR(22));

    attron(COLOR_PAIR(23)); 
    mvprintw(21, 60, "2.magical food \U000015F0"); 
    mvprintw(21, 82, "%d", x_food);
    mvprintw(21, 105, "50");
    attroff(COLOR_PAIR(23));

    attron(COLOR_PAIR(24));
    mvprintw(23, 60, "3.exquisite food \U00001598");  
    mvprintw(23, 82, "%d", z_food);
    mvprintw(23, 105, "50");
    attroff(COLOR_PAIR(24));

    refresh();
    while(1) {
        char n = getchar();
        if(n == '1') {
            if(y_food > 0)  {
                user.hp -= 50;
                mvprintw(25, 82, "                                 ");
                mvprintw(25, 82, "you ate expired food");
                y_food--;
                continue;
            }
            if(food_count <= 0) {
                mvprintw(25, 82, "                                 ");
                mvprintw(25, 82, "Not Enough Food");
                refresh();
                continue;
            }
            hunger_level -= 80;
            user.hp += 50;
            food_count--;
        }
        else if(n == '2') {
            if(x_food <= 0) {
                mvprintw(25, 82, "                                 ");
                mvprintw(25, 82, "Not Enough Food");
                refresh();
                continue;
            }
            hunger_level -= 80;
            x_food--;
            s = 1;
        } 
        else if(n == '3') {
            if(z_food <= 0) {
                mvprintw(25, 82, "                                 ");
                mvprintw(25, 82, "Not Enough Food");
                refresh();
                continue;
            }
            d = 1;
            hunger_level -= 80;
            z_food--;
        }
        else if(n == 'E') {
            clear();
            break;
        }
        else {
            continue;
        }
    }
}

void update_status() {

    WINDOW *status_win = newwin(7, 40, 0, 160);
    box(status_win, 0, 0);

    wattron(status_win, COLOR_PAIR(41));
    mvwprintw(status_win, 1, 13, "HP:   ");
    wattroff(status_win, COLOR_PAIR(41));
    wattron(status_win, COLOR_PAIR(26));

    if (user.hp >= 1000) {
        mvwprintw(status_win, 1, 19, "%d", user.hp);
    } else if (user.hp >= 100) {
        mvwprintw(status_win, 1, 19, " %d", user.hp);
    } else if (user.hp >= 10) {
        mvwprintw(status_win, 1, 19, "  %d", user.hp);
    } else {
        mvwprintw(status_win, 1, 19, "   %d", user.hp);
    }
    wattroff(status_win, COLOR_PAIR(26));

    wattron(status_win, COLOR_PAIR(90));
    mvwprintw(status_win, 2,13, "Food:  ");
    wattroff(status_win, COLOR_PAIR(90));
    wattron(status_win, COLOR_PAIR(24));
    if (food_count >= 1000) {
        mvwprintw(status_win, 2, 19, "%d", food_count);
    } else if (food_count >= 100) {
        mvwprintw(status_win, 2, 19, " %d", food_count);
    } else if (food_count >= 10) {
        mvwprintw(status_win, 2, 19, "  %d", food_count);
    } else {
        mvwprintw(status_win, 2, 19, "   %d", food_count);
    }
    wattroff(status_win, COLOR_PAIR(24));

    wattron(status_win, COLOR_PAIR(23));
    mvwprintw(status_win, 3, 13, "Hunger:");
    wattroff(status_win, COLOR_PAIR(23));
    wattron(status_win, COLOR_PAIR(22));
    if (hunger_level >= 1000) {
        mvwprintw(status_win, 3, 20, "%d", hunger_level);
    } else if (hunger_level >= 100) {
        mvwprintw(status_win, 3, 20, " %d", hunger_level);
    } else if (hunger_level >= 10) {
        mvwprintw(status_win, 3, 20, "  %d", hunger_level);
    } else {
        mvwprintw(status_win, 3, 20, "   %d", hunger_level);
    }
    wattroff(status_win, COLOR_PAIR(22));

    wattron(status_win, COLOR_PAIR(45));
    mvwprintw(status_win, 4, 13, "Gold:  ");
    wattroff(status_win, COLOR_PAIR(45));
    wattron(status_win, COLOR_PAIR(15));
    if (gold_count >= 1000) {
        mvwprintw(status_win, 4,  19, "%d", gold_count);
    } else if (gold_count >= 100) {
        mvwprintw(status_win, 4, 19, " %d", gold_count);
    } else if (gold_count >= 10) {
        mvwprintw(status_win, 4, 19, "  %d", gold_count);
    } else {
        mvwprintw(status_win, 4, 19, "   %d", gold_count);
    }
    wattroff(status_win, COLOR_PAIR(15));

    wattron(status_win, COLOR_PAIR(16));
    mvwprintw(status_win, 5, 13, "Score: ");
    wattroff(status_win, COLOR_PAIR(16));
    wattron(status_win, COLOR_PAIR(17));
    if (score >= 1000) {
        mvwprintw(status_win, 5, 19, "%d", score);
    } else if (score >= 100) {
        mvwprintw(status_win, 5, 19, " %d", score);
    } else if (score >= 10) {
        mvwprintw(status_win, 5, 19, "  %d", score);
    } else {
        mvwprintw(status_win, 5, 19, "   %d", score);
    }
    wattroff(status_win, COLOR_PAIR(17));

    wrefresh(status_win);
    delwin(status_win);
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
                map->tiles[black_gold_y][black_gold_x] = 'B'; 
                
        }
    }
}

void place_weapons(GameMap* map) {
    for (int i = 0; i < map->room_count; ++i) {
        Room room = map->rooms[i];
        int weapon_x = room.x + 1 + rand() % (room.width - 2);
        int weapon_y = room.y + 1 + rand() % (room.height - 2);
        int weapon_index;

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
    mvprintw(20, 85, "SpellBook (Press 'I' to exit)");
    for (int i = 0; i < MAX_SPELLS; ++i) {
        int color_pair;
        char unicode[5];

        if (strcmp(spells[i].name, "Health") == 0) {
            color_pair = 19; 
            strcpy(unicode, "\u2764");
        } else if (strcmp(spells[i].name, "Speed") == 0) {
            color_pair = 20; 
            strcpy(unicode, "\u23E9"); 
        } else if (strcmp(spells[i].name, "Damage") == 0) {
            color_pair = 21;
            strcpy(unicode, "\u26A1"); 
        } else {
            color_pair = 10;  
            strcpy(unicode, "");
        }

        attron(COLOR_PAIR(color_pair));
        mvprintw(22 + i, 85, "%s: %d %s", spells[i].name, spells[i].count, unicode);
        attroff(COLOR_PAIR(color_pair));
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
    mvprintw(20, 85, "Inventory (Press 'I' to exit)");
    for (int i = 0; i < MAX_WEAPONS; ++i) {
        int color_pair;
        char unicode[5];

        if (strcmp(weapons[i].name, "Mace") == 0) {
            color_pair = 12; 
            strcpy(unicode, "\u2692");  // ⚒
        } else if (strcmp(weapons[i].name, "Dagger") == 0) {
            color_pair = 15; 
            strcpy(unicode, "\U0001F5E1");  // 🗡
        } else if (strcmp(weapons[i].name, "Magic Wand") == 0) {
            color_pair = 16;
            strcpy(unicode, "\U000023B0");  // ⎰
        } else if (strcmp(weapons[i].name, "Normal Arrow") == 0) {
            color_pair = 17;  
            strcpy(unicode, "\U000027B3");  // ➳
        } else if (strcmp(weapons[i].name, "Sword") == 0) {
            color_pair = 18;  
            strcpy(unicode, "\U00002694");  // ⚔
        } else {
            color_pair = 10;
            strcpy(unicode, "");
        }

        attron(COLOR_PAIR(color_pair));
        mvprintw(22 + i, 85, "%s: %d %s", weapons[i].name, weapons[i].n, unicode);
        attroff(COLOR_PAIR(color_pair));
    }
    refresh();
    while (1) {
        char c = getch();
        if (c == 'I' || c == 'i') {
            break;
        }
    }
}

void init_monster(Monster *monster, char type, int x, int y) {
    monster->type = type;
    monster->x = x;
    monster->y = y;
    monster->follow_steps = 0;
    monster->exhausted = false;
    monster->active = false; 
    monster->previous_tile = '.'; 
    
    switch (type) {
        case 'D':
            monster->hp = 5;
            monster->damage = 2;
            monster->max_follow_steps = -1;
            break;
        case 'F':
            monster->hp = 10;
            monster->damage = 4;
            monster->max_follow_steps = -1; 
            break;
        case 'G':
            monster->hp = 15;
            monster->damage = 6;
            monster->max_follow_steps = -1; 
            break;
        case 'S':
            monster->hp = 20;
            monster->damage = 8;
            monster->max_follow_steps = -1; 
            break;
        case 'U':
            monster->hp = 30;
            monster->damage = 10;
            monster->max_follow_steps = -1; 
            break;
    }
}

void spawn_monster(GameMap* map, char type, int x, int y) {
    if (monster_count >= MAX_MONSTERS) return;

    for (int i = 0; i < monster_count; ++i) {
        if (monsters[i].x == x && monsters[i].y == y) {
            return;
        }
    }

    Monster *monster = &monsters[monster_count++];
    init_monster(monster, type, x, y);
    map->tiles[y][x] = type;
}

void initialize_visited() {
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            visited[y][x] = false;
        }
    }
}

void mark_visited(GameMap* map, int x, int y) {
    if (x >= 0 && x < map->width && y >= 0 && y < map->height) {
        visited[y][x] = true;
    }
}

void reveal_room(GameMap* map, int room_idx) {
    Room room = map->rooms[room_idx];
    for (int y = room.y; y < room.y + room.height; ++y) {
        for (int x = room.x; x < room.x + room.width; ++x) {
            mark_visited(map, x, y);
        }
    }
}

void reveal_corridors(GameMap* map, int x, int y) {
    for (int dy = -5; dy <= 5; ++dy) {
        for (int dx = -5; dx <= 5; ++dx) {
            if (y + dy >= 0 && y + dy < map->height && x + dx >= 0 && x + dx < map->width) {
                char tile = map->tiles[y + dy][x + dx];
                if (tile == '#' || tile == '+') {
                    mark_visited(map, x + dx, y + dy);
                }
            }
        }
    }
}

void move_monsters(GameMap* map) {
    for (int i = 0; i < monster_count; ++i) {
        Monster *monster = &monsters[i];
        if (monster->hp <= 0) continue;

        if (monster->active) {
            if (monster->type == 'S') {
                move_snake(map, monster);
            } else {
                move_monster_towards_player(map, monster);
            }

            if (monster->type != 'S') {
                if (!is_player_in_room(map, monster->x, monster->y)) {
                    monster->active = false;
                }
            }
        }
    }
}

void move_monster_towards_player(GameMap* map, Monster *monster) {
    int dx = 0;
    int dy = 0;

    if (user.x < monster->x) dx = -1;
    else if (user.x > monster->x) dx = 1;

    if (user.y < monster->y) dy = -1;
    else if (user.y > monster->y) dy = 1;

    int new_x = monster->x + dx;
    int new_y = monster->y + dy;

    char target_tile = map->tiles[new_y][new_x];
    if (target_tile == '.' || target_tile == '#' || target_tile == '+') {

        map->tiles[monster->y][monster->x] = monster->previous_tile;
        monster->previous_tile = target_tile;

        monster->x = new_x;
        monster->y = new_y;
        map->tiles[monster->y][monster->x] = monster->type;
    }
}

void move_snake(GameMap* map, Monster *snake) {
    int dx = 0;
    int dy = 0;

    if (user.x < snake->x) dx = -1;
    else if (user.x > snake->x) dx = 1;

    if (user.y < snake->y) dy = -1;
    else if (user.y > snake->y) dy = 1;

    int new_x = snake->x + dx;
    int new_y = snake->y + dy;

   
    char target_tile = map->tiles[new_y][new_x];
    if (target_tile == '.' || target_tile == '#' || target_tile == '+' || target_tile == '-') {
     
        map->tiles[snake->y][snake->x] = snake->previous_tile;
        snake->previous_tile = map->tiles[new_y][new_x]; 

        snake->x = new_x;
        snake->y = new_y;

        if (target_tile != 'D' && target_tile != 'F' && target_tile != 'G' && target_tile != 'U') {
            map->tiles[new_y][new_x] = snake->type;
        }
    } else if (target_tile == 'D' || target_tile == 'F' || target_tile == 'G' || target_tile == 'U') {

        map->tiles[snake->y][snake->x] = snake->previous_tile;
        snake->previous_tile = map->tiles[new_y][new_x]; 
        snake->x = new_x;
        snake->y = new_y;
    }
}

bool is_player_in_room(GameMap* map, int x, int y) {
    for (int i = 0; i < map->room_count; ++i) {
        Room room = map->rooms[i];
        if (x >= room.x && x < room.x + room.width && y >= room.y && y < room.y + room.height) {
            if (user.x >= room.x && user.x < room.x + room.width && user.y >= room.y && user.y < room.y + room.height) {
                return true;
            }
        }
    }
    return false;
}

// void make_scoreboard() {
//     if(karbar == 1) {
//         FILE* f = fopen("scoreboard1", "w");
//         fprintf(f, "%s\n", player.name);
//         fprintf(f, "%d\n", player.score);
//         fprintf(f, "%d\n", player.gold);
//         fprintf(f, "%d\n", n_login);
//         fprintf(f, "%d\n", player.exp);
//         fclose(f);
//     }
//     else if(karbar == 2) {
//         FILE* f = fopen("scoreboard2", "w");
//         fprintf(f, "%s\n", player.name);
//         fprintf(f, "%d\n", player.score);
//         fprintf(f, "%d\n", player.gold);
//         fprintf(f, "%d\n", n_login);
//         fprintf(f, "%d\n", player.exp);
//         fclose(f);
//     }
//     else if(karbar == 3) {
//         FILE* f = fopen("scoreboard3", "w");
//         fprintf(f, "%s\n", player.name);
//         fprintf(f, "%d\n", player.score);
//         fprintf(f, "%d\n", player.gold);
//         fprintf(f, "%d\n", n_login);
//         fprintf(f, "%d\n", player.exp);
//         fclose(f);
//     }
//     else if(karbar == 4) {
//         FILE* f = fopen("scoreboard4", "w");
//         fprintf(f, "%s\n", player.name);
//         fprintf(f, "%d\n", player.score);
//         fprintf(f, "%d\n", player.gold);
//         fprintf(f, "%d\n", n_login);
//         fprintf(f, "%d\n", player.exp);
//         fclose(f);
//     }
// }
// void scoreboard() {
//     FILE* f1 = fopen("scoreboard1", "r");
//     FILE* f2 = fopen("scoreboard2", "r");
//     FILE* f3 = fopen("scoreboard3", "r");
//     FILE* f4 = fopen("scoreboard4", "r");
//     at p[4];
//     fscanf(f1, "%s", p[0].name);
//     fscanf(f1, "%d", &p[0].score);
//     fscanf(f1, "%d", &p[0].gold);
//     fscanf(f1, "%d", &p[0].n_login);
//     fscanf(f1, "%d", &p[0].exp);
//     fclose(f1);

//     fscanf(f2, "%s", p[1].name);
//     fscanf(f2, "%d", &p[1].score);
//     fscanf(f2, "%d", &p[1].gold);
//     fscanf(f2, "%d", &p[1].n_login);
//     fscanf(f2, "%d", &p[1].exp);
//     fclose(f2);

//     fscanf(f3, "%s", p[2].name);
//     fscanf(f3, "%d", &p[2].score);
//     fscanf(f3, "%d", &p[2].gold);
//     fscanf(f3, "%d", &p[2].n_login);
//     fscanf(f3, "%d", &p[2].exp);
//     fclose(f3);

//     fscanf(f4, "%s", p[3].name);
//     fscanf(f4, "%d", &p[3].score);
//     fscanf(f4, "%d", &p[3].gold);
//     fscanf(f4, "%d", &p[3].n_login);
//     fscanf(f4, "%d", &p[3].exp);
//     fclose(f4);

//     init_pair(1, COLOR_RED, COLOR_BLACK);
//     init_pair(2, COLOR_GREEN, COLOR_BLACK);
//     init_pair(4, COLOR_BLUE, COLOR_BLACK);

//     while(1) {
//         attron(COLOR_PAIR(9));
//         mvprintw(0, 0, "        name                           score                gold                 n                 exp");
//         mvprintw(0, 184, "0:back");
//         refresh();
//         attroff(COLOR_PAIR(9));

//         if(p[0].score <= p[1].score && p[1].score <= p[2].score) {
//             if(strcmp(player.name, "mahdi0x06") == 0) {
//                 attron(COLOR_PAIR(1));
//                 mvprintw(2, 0, "1.\U0001F947 %s(goat)                   %d                   %d                   %d                 %d", p[2].name, p[2].score, p[2].gold, p[2].n_login, p[2].exp);
//                 attroff(COLOR_PAIR(1));
//                 attron(COLOR_PAIR(2));
//                 mvprintw(4, 0, "2.\U0001F948 %s(legend)                 %d                   %d                   %d                 %d", p[1].name, p[1].score, p[1].gold, p[1].n_login, p[1].exp);
//                 attroff(COLOR_PAIR(2));
//                 attron(COLOR_PAIR(4));
//                 mvprintw(6, 0, "\U000027A13.\U0001F949 %s(champ)                  %d                   %d                   %d                 %d", p[0].name, p[0].score, p[0].gold, p[0].n_login, p[0].exp);
//                 attroff(COLOR_PAIR(4));
//             }

//             else if(strcmp(player.name, "mahdi0110") == 0) {
//                 attron(COLOR_PAIR(1));

//                 mvprintw(2, 0, "\U000027A11.\U0001F947 %s(goat)                   %d                   %d                   %d                 %d", p[2].name, p[2].score, p[2].gold, p[2].n_login, p[2].exp);
//                 attron(COLOR_PAIR(1));
//                 attron(COLOR_PAIR(2));

//                 mvprintw(4, 0, "2.\U0001F948 %s(legend)                 %d                   %d                   %d                 %d", p[1].name, p[1].score, p[1].gold, p[1].n_login, p[1].exp);
//                 attron(COLOR_PAIR(2));
//                 attron(COLOR_PAIR(4));

//                 mvprintw(6, 0, "3.\U0001F949 %s(champ)                  %d                   %d                   %d                 %d", p[0].name, p[0].score, p[0].gold, p[0].n_login, p[0].exp);
//                 attron(COLOR_PAIR(4));

//             }
//             else if(strcmp(player.name, "TAmabani") == 0) {
//                 attron(COLOR_PAIR(1));
                
//                 mvprintw(2, 0, "1.\U0001F947 %s(goat)                   %d                   %d                   %d                 %d", p[2].name, p[2].score, p[2].gold, p[2].n_login, p[2].exp);
//                 attron(COLOR_PAIR(1));
//                 attron(COLOR_PAIR(2));
                
//                 mvprintw(4, 0, "\U000027A12.\U0001F948 %s(legend)                 %d                   %d                   %d                 %d", p[1].name, p[1].score, p[1].gold, p[1].n_login, p[1].exp);
//                 attron(COLOR_PAIR(2));
//                 attron(COLOR_PAIR(4));
                
//                 mvprintw(6, 0, "3.\U0001F949 %s(champ)                  %d                   %d                   %d                 %d", p[0].name, p[0].score, p[0].gold, p[0].n_login, p[0].exp);
//                 attron(COLOR_PAIR(4));
            
//             }
//             else {
//                 attron(COLOR_PAIR(1));
            
//                 mvprintw(2, 0, "1.\U0001F947 %s(goat)                   %d                   %d                   %d                 %d", p[2].name, p[2].score, p[2].gold, p[2].n_login, p[2].exp);
//                 attroff(COLOR_PAIR(1));
//                 attron(COLOR_PAIR(2));
            
//                 mvprintw(4, 0, "2.\U0001F948 %s(legend)                  %d                   %d                   %d                 %d", p[1].name, p[1].score, p[1].gold, p[1].n_login, p[1].exp);
//                 attroff(COLOR_PAIR(2));
//                 attron(COLOR_PAIR(4));
            
//                 mvprintw(6, 0, "3.\U0001F949 %s(champ)                    %d                   %d                   %d                 %d", p[0].name, p[0].score, p[0].gold, p[0].n_login, p[0].exp);   
//                 attroff(COLOR_PAIR(4));
//             }
//             refresh();
//         }
//         else if(p[0].score <= p[2].score && p[2].score <= p[1].score) {
//             if(strcmp(player.name, "mahdi0x06") == 0) {
//                 attron(COLOR_PAIR(1));
                
//                 mvprintw(2, 0, "1.\U0001F947 %s(goat)                   %d                   %d                   %d                 %d", p[1].name, p[1].score, p[1].gold, p[1].n_login, p[1].exp);
//                 attroff(COLOR_PAIR(1));
//                 attron(COLOR_PAIR(2));
                
//                 mvprintw(4, 0, "2.\U0001F948 %s(legend)                 %d                   %d                   %d                 %d", p[2].name, p[2].score, p[2].gold, p[2].n_login, p[2].exp);
//                 attroff(COLOR_PAIR(2));
//                 attron(COLOR_PAIR(4));
                
//                 mvprintw(6, 0, "\U000027A13.\U0001F949 %s(champ)                  %d                   %d                   %d                 %d", p[0].name, p[0].score, p[0].gold, p[0].n_login, p[0].exp); 
//                 attroff(COLOR_PAIR(4));
            
//             }
//             else if(strcmp(player.name, "mahdi0110") == 0) {
//                 attron(COLOR_PAIR(1));
                
//                 mvprintw(2, 0, "\U000027A11.\U0001F947 %s(goat)                   %d                   %d                   %d                 %d", p[1].name, p[1].score, p[1].gold, p[1].n_login, p[1].exp);
//                 attroff(COLOR_PAIR(1));
//                 attron(COLOR_PAIR(2));
                
//                 mvprintw(4, 0, "2.\U0001F948 %s(legend)                 %d                   %d                   %d                 %d", p[2].name, p[2].score, p[2].gold, p[2].n_login, p[2].exp);
//                 attroff(COLOR_PAIR(2));
//                 attron(COLOR_PAIR(4));
                
//                 mvprintw(6, 0, "3.\U0001F949 %s(champ)                  %d                   %d                   %d                 %d", p[0].name, p[0].score, p[0].gold, p[0].n_login, p[0].exp); 
//                 attroff(COLOR_PAIR(4));
            
//             }
//             else if(strcmp(player.name, "TAmabani") == 0) {
//                 attron(COLOR_PAIR(1));
                
//                 mvprintw(2, 0, "1.\U0001F947 %s(goat)                   %d                   %d                   %d                 %d", p[1].name, p[1].score, p[1].gold, p[1].n_login, p[1].exp);
//                 attroff(COLOR_PAIR(1));
//                 attron(COLOR_PAIR(2));
                
//                 mvprintw(4, 0, "\U000027A12.\U0001F948 %s(legend)                 %d                   %d                   %d                 %d", p[2].name, p[2].score, p[2].gold, p[2].n_login, p[2].exp);
//                 attroff(COLOR_PAIR(2));
//                 attron(COLOR_PAIR(4));
                
//                 mvprintw(6, 0, "3.\U0001F949 %s(champ)                  %d                   %d                   %d                 %d", p[0].name, p[0].score, p[0].gold, p[0].n_login, p[0].exp); 
//                 attroff(COLOR_PAIR(4));
            
//             }
//             else {
//                 attron(COLOR_PAIR(1));
            
//                 mvprintw(2, 0, "1.\U0001F947 %s(goat)                   %d                   %d                   %d                 %d", p[1].name, p[1].score, p[1].gold, p[1].n_login, p[1].exp);
//                 attroff(COLOR_PAIR(1));
//                 attron(COLOR_PAIR(2));
            
//                 mvprintw(4, 0, "2.\U0001F948 %s(legend)                  %d                   %d                   %d                 %d", p[2].name, p[2].score, p[2].gold, p[2].n_login, p[2].exp);
//                 attroff(COLOR_PAIR(2));
//                 attron(COLOR_PAIR(4));
            
//                 mvprintw(6, 0, "3.\U0001F949 %s(champ)                    %d                   %d                   %d                 %d", p[0].name, p[0].score, p[0].gold, p[0].n_login, p[0].exp);   
//                 attroff(COLOR_PAIR(4));
//             }
//            refresh(); 
//         }
//         else if(p[1].score <= p[0].score && p[0].score <= p[2].score) {
//             if(strcmp(player.name, "mahdi0x06") == 0) {
//                 attron(COLOR_PAIR(1));
                
//                 mvprintw(2, 0, "1.\U0001F947 %s(goat)                   %d                   %d                   %d                 %d", p[2].name, p[2].score, p[2].gold, p[2].n_login, p[2].exp);
//                 attroff(COLOR_PAIR(1));
//                 attron(COLOR_PAIR(2));
                
//                 mvprintw(4, 0, "\U000027A12.\U0001F948 %s(legend)                 %d                   %d                   %d                 %d", p[0].name, p[0].score, p[0].gold, p[0].n_login, p[0].exp);
//                 attroff(COLOR_PAIR(2));
//                 attron(COLOR_PAIR(4));
                
//                 mvprintw(6, 0, "3.\U0001F949 %s(champ)                  %d                   %d                   %d                 %d", p[1].name, p[1].score, p[1].gold, p[1].n_login, p[1].exp); 
//                 attroff(COLOR_PAIR(4));
            
//             }
//             else if(strcmp(player.name, "mahdi0110") == 0) {
//                 attron(COLOR_PAIR(1));
            
//                 mvprintw(2, 0, "1.\U0001F947 %s(goat)                   %d                   %d                   %d                 %d", p[2].name, p[2].score, p[2].gold, p[2].n_login, p[2].exp);
//                 attroff(COLOR_PAIR(1));
//                 attron(COLOR_PAIR(2));
            
//                 mvprintw(4, 0, "2.\U0001F948 %s(legend)                 %d                   %d                   %d                 %d", p[0].name, p[0].score, p[0].gold, p[0].n_login, p[0].exp);
//                 attroff(COLOR_PAIR(2));
//                 attron(COLOR_PAIR(4));
            
//                 mvprintw(6, 0, "\U000027A13.\U0001F949 %s(champ)                  %d                   %d                   %d                 %d", p[1].name, p[1].score, p[1].gold, p[1].n_login, p[1].exp); 
//                 attroff(COLOR_PAIR(4));
            
//             }
//             else if(strcmp(player.name, "TAmabani") == 0) {
//                 attron(COLOR_PAIR(1));
            
//                 mvprintw(2, 0, "\U000027A11.\U0001F947 %s(goat)                   %d                   %d                   %d                 %d", p[2].name, p[2].score, p[2].gold, p[2].n_login, p[2].exp);
//                 attroff(COLOR_PAIR(1));
//                 attron(COLOR_PAIR(2));
            
//                 mvprintw(4, 0, "2.\U0001F948 %s(legend)                 %d                   %d                   %d                 %d", p[0].name, p[0].score, p[0].gold, p[0].n_login, p[0].exp);
//                 attroff(COLOR_PAIR(2));
//                 attron(COLOR_PAIR(4));
            
//                 mvprintw(6, 0, "3.\U0001F949 %s(champ)                  %d                   %d                   %d                 %d", p[1].name, p[1].score, p[1].gold, p[1].n_login, p[1].exp); 
//                 attroff(COLOR_PAIR(4));
            
//             }
//             else {
//                 attron(COLOR_PAIR(1));
            
//                 mvprintw(2, 0, "1.\U0001F947 %s(goat)                   %d                   %d                   %d                 %d", p[2].name, p[2].score, p[2].gold, p[2].n_login, p[2].exp);
//                 attroff(COLOR_PAIR(1));
//                 attron(COLOR_PAIR(2));
            
//                 mvprintw(4, 0, "2.\U0001F948 %s(legend)                  %d                   %d                   %d                 %d", p[0].name, p[0].score, p[0].gold, p[0].n_login, p[0].exp);
//                 attroff(COLOR_PAIR(2));
//                 attron(COLOR_PAIR(4));
            
//                 mvprintw(6, 0, "3.\U0001F949 %s(champ)                    %d                   %d                   %d                 %d", p[1].name, p[1].score, p[1].gold, p[1].n_login, p[1].exp);   
//                 attroff(COLOR_PAIR(4));
//             }
//             refresh(); 
//         }
//         else if(p[1].score <= p[2].score && p[2].score <= p[0].score) {
//             if(strcmp(player.name, "mahdi0x06") == 0) {
//                 attron(COLOR_PAIR(1));
            
//                 mvprintw(2, 0, "\U000027A11.\U0001F947 %s(goat)                   %d                   %d                   %d                 %d", p[0].name, p[0].score, p[0].gold, p[0].n_login, p[0].exp);
//                 attroff(COLOR_PAIR(1));
//                 attron(COLOR_PAIR(2));
            
//                 mvprintw(4, 0, "2.\U0001F948 %s(legend)                 %d                   %d                   %d                 %d", p[2].name, p[2].score, p[2].gold, p[2].n_login, p[2].exp);
//                 attroff(COLOR_PAIR(2));
//                 attron(COLOR_PAIR(4));
            
//                 mvprintw(6, 0, "3.\U0001F949 %s(champ)                  %d                   %d                   %d                 %d", p[1].name, p[1].score, p[1].gold, p[1].n_login, p[1].exp);  
//                 attroff(COLOR_PAIR(4));
            
//             }
//             else if(strcmp(player.name, "mahdi0110") == 0) {
//                 attron(COLOR_PAIR(1));
            
//                 mvprintw(2, 0, "1.\U0001F947 %s(goat)                   %d                   %d                   %d                 %d", p[0].name, p[0].score, p[0].gold, p[0].n_login, p[0].exp);
//                 attroff(COLOR_PAIR(1));
//                 attron(COLOR_PAIR(2));
            
//                 mvprintw(4, 0, "2.\U0001F948 %s(legend)                 %d                   %d                   %d                 %d", p[2].name, p[2].score, p[2].gold, p[2].n_login, p[2].exp);
//                 attroff(COLOR_PAIR(2));
//                 attron(COLOR_PAIR(4));
            
//                 mvprintw(6, 0, "\U000027A13.\U0001F949 %s(champ)                  %d                   %d                   %d                 %d", p[1].name, p[1].score, p[1].gold, p[1].n_login, p[1].exp);  
//                 attroff(COLOR_PAIR(4));
            
//             }
//             else if(strcmp(player.name, "TAmabani") == 0) {
//                 attron(COLOR_PAIR(1));
            
//                 mvprintw(2, 0, "1.\U0001F947 %s(goat)                   %d                   %d                   %d                 %d", p[0].name, p[0].score, p[0].gold, p[0].n_login, p[0].exp);
//                 attroff(COLOR_PAIR(1));
//                 attron(COLOR_PAIR(2));
            
//                 mvprintw(4, 0, "\U000027A12.\U0001F948 %s(legend)                 %d                   %d                   %d                 %d", p[2].name, p[2].score, p[2].gold, p[2].n_login, p[2].exp);
//                 attroff(COLOR_PAIR(2));
//                 attron(COLOR_PAIR(4));
            
//                 mvprintw(6, 0, "3.\U0001F949 %s(champ)                  %d                   %d                   %d                 %d", p[1].name, p[1].score, p[1].gold, p[1].n_login, p[1].exp);   
//                 attroff(COLOR_PAIR(4));
            
//             }
//             else {
//                 attron(COLOR_PAIR(1));
            
//                 mvprintw(2, 0, "1.\U0001F947 %s(goat)                   %d                   %d                   %d                 %d", p[0].name, p[0].score, p[0].gold, p[0].n_login, p[0].exp);
//                 attroff(COLOR_PAIR(1));
//                 attron(COLOR_PAIR(2));
            
//                 mvprintw(4, 0, "2.\U0001F948 %s(legend)                  %d                   %d                   %d                 %d", p[2].name, p[2].score, p[2].gold, p[2].n_login, p[2].exp);
//                 attroff(COLOR_PAIR(2));
//                 attron(COLOR_PAIR(4));
            
//                 mvprintw(6, 0, "3.\U0001F949 %s(champ)                    %d                   %d                   %d                 %d", p[1].name, p[1].score, p[1].gold, p[1].n_login, p[1].exp);   
//                 attroff(COLOR_PAIR(4));
//             }
//             refresh();
//         }
//         else if(p[2].score <= p[1].score && p[1].score <= p[0].score) {
//             if(strcmp(player.name, "mahdi0x06") == 0) {
//                 attron(COLOR_PAIR(1));
            
//                 mvprintw(2, 0, "\U000027A11.\U0001F947 %s(goat)                  %d                   %d                   %d                 %d", p[0].name, p[0].score, p[0].gold, p[0].n_login, p[0].exp);
//                 attroff(COLOR_PAIR(1));
//                 attron(COLOR_PAIR(2));
            
//                 mvprintw(4, 0, "2.\U0001F948 %s(legend)                  %d                   %d                   %d                 %d", p[1].name, p[1].score, p[1].gold, p[1].n_login, p[1].exp);
//                 attroff(COLOR_PAIR(2));
//                 attron(COLOR_PAIR(4));
            
//                 mvprintw(6, 0, "3.\U0001F949 %s(champ)                    %d                   %d                   %d                 %d", p[2].name, p[2].score, p[2].gold, p[2].n_login, p[2].exp);
//                 attroff(COLOR_PAIR(4));
            
//             }
//             else if(strcmp(player.name, "mahdi0110") == 0) {
//                 attron(COLOR_PAIR(1));
            
//                 mvprintw(2, 0, "1.\U0001F947 %s(goat)                   %d                   %d                   %d                 %d", p[0].name, p[0].score, p[0].gold, p[0].n_login, p[0].exp);
//                 attroff(COLOR_PAIR(1));
//                 attron(COLOR_PAIR(2));
            
//                 mvprintw(4, 0, "\U000027A12.\U0001F948 %s(legend)                  %d                   %d                   %d                 %d", p[1].name, p[1].score, p[1].gold, p[1].n_login, p[1].exp);
//                 attroff(COLOR_PAIR(2));
//                 attron(COLOR_PAIR(4));
            
//                 mvprintw(6, 0, "3.\U0001F949 %s(champ)                    %d                   %d                   %d                 %d", p[2].name, p[2].score, p[2].gold, p[2].n_login, p[2].exp);    
//                 attroff(COLOR_PAIR(4));
            
//             }
//             else if(strcmp(player.name, "TAmabani") == 0) {
//                 attron(COLOR_PAIR(1));
            
//                 mvprintw(2, 0, "1.\U0001F947 %s(goat)                   %d                   %d                   %d                 %d", p[0].name, p[0].score, p[0].gold, p[0].n_login, p[0].exp);
//                 attroff(COLOR_PAIR(1));
//                 attron(COLOR_PAIR(2));
            
//                 mvprintw(4, 0, "2.\U0001F948 %s(legend)                  %d                   %d                   %d                 %d", p[1].name, p[1].score, p[1].gold, p[1].n_login, p[1].exp);
//                 attroff(COLOR_PAIR(2));
//                 attron(COLOR_PAIR(4));
            
//                 mvprintw(6, 0, "\U000027A13.\U0001F949 %s(champ)                    %d                   %d                   %d                 %d", p[2].name, p[2].score, p[2].gold, p[2].n_login, p[2].exp);   
//                 attroff(COLOR_PAIR(4));
            
//             }
//             else {
//                 attron(COLOR_PAIR(1));
            
//                 mvprintw(2, 0, "1.\U0001F947 %s(goat)                   %d                   %d                   %d                 %d", p[0].name, p[0].score, p[0].gold, p[0].n_login, p[0].exp);
//                 attroff(COLOR_PAIR(1));
//                 attron(COLOR_PAIR(2));
            
//                 mvprintw(4, 0, "2.\U0001F948 %s(legend)                  %d                   %d                   %d                 %d", p[1].name, p[1].score, p[1].gold, p[1].n_login, p[1].exp);
//                 attroff(COLOR_PAIR(2));
//                 attron(COLOR_PAIR(4));
            
//                 mvprintw(6, 0, "3.\U0001F949 %s(champ)                    %d                   %d                   %d                 %d", p[2].name, p[2].score, p[2].gold, p[2].n_login, p[2].exp);   
//                 attroff(COLOR_PAIR(4));
//             }
//             refresh();
//         }
//         else if(p[2].score <= p[0].score && p[0].score <= p[1].score) {
//             if(strcmp(player.name, "mahdi0x06") == 0) {
//                 attron(COLOR_PAIR(1));
                
//                 mvprintw(2, 0, "1.\U0001F947 %s(goat)                   %d                   %d                   %d                 %d", p[1].name, p[1].score, p[1].gold, p[1].n_login, p[1].exp);
//                 attroff(COLOR_PAIR(1));
//                 attron(COLOR_PAIR(2));
            
//                 mvprintw(4, 0, "\U000027A12.\U0001F948 %s(legend)                 %d                   %d                   %d                 %d", p[0].name, p[0].score, p[0].gold, p[0].n_login, p[0].exp);
//                 attroff(COLOR_PAIR(2));
//                 attron(COLOR_PAIR(4));
            
//                 mvprintw(6, 0, "3.\U0001F949 %s(champ)                  %d                   %d                   %d                 %d", p[2].name, p[2].score, p[2].gold, p[2].n_login, p[2].exp);
//                 attroff(COLOR_PAIR(4));
            
//             }
//             else if(strcmp(player.name, "mahdi0110") == 0) {
//                 attron(COLOR_PAIR(1));
            
//                 mvprintw(2, 0, "\U000027A11.\U0001F947 %s(goat)                   %d                   %d                   %d                 %d", p[1].name, p[1].score, p[1].gold, p[1].n_login, p[1].exp);
//                 attroff(COLOR_PAIR(1));
//                 attron(COLOR_PAIR(2));
            
//                 mvprintw(4, 0, "2.\U0001F948 %s(legend)                 %d                   %d                   %d                 %d", p[0].name, p[0].score, p[0].gold, p[0].n_login, p[0].exp);
//                 attroff(COLOR_PAIR(2));
//                 attron(COLOR_PAIR(4));
            
//                 mvprintw(6, 0, "3.\U0001F949 %s(champ)                  %d                   %d                   %d                 %d", p[2].name, p[2].score, p[2].gold, p[2].n_login, p[2].exp);
//                 attroff(COLOR_PAIR(4));
            
//             }
//             else if(strcmp(player.name, "TAmabani") == 0) {
//                 attron(COLOR_PAIR(1));
            
//                 mvprintw(2, 0, "1.\U0001F947 %s(goat)                   %d                   %d                   %d                 %d", p[1].name, p[1].score, p[1].gold, p[1].n_login, p[1].exp);
//                 attroff(COLOR_PAIR(1));
//                 attron(COLOR_PAIR(2));
            
//                 mvprintw(4, 0, "2.\U0001F948 %s(legend)                 %d                   %d                   %d                 %d", p[0].name, p[0].score, p[0].gold, p[0].n_login, p[0].exp);
//                 attroff(COLOR_PAIR(2));
//                 attron(COLOR_PAIR(4));
            
//                 mvprintw(6, 0, "\U000027A13.\U0001F949 %s(champ)                  %d                   %d                   %d                 %d", p[2].name, p[2].score, p[2].gold, p[2].n_login, p[2].exp); 
//                 attroff(COLOR_PAIR(4));
            
            
//             }
//             else {
//                 attron(COLOR_PAIR(1));
            
//                 mvprintw(2, 0, "1.\U0001F947 %s(goat)                   %d                   %d                   %d                 %d", p[1].name, p[1].score, p[1].gold, p[1].n_login, p[1].exp);
//                 attroff(COLOR_PAIR(1));
//                 attron(COLOR_PAIR(2));
            
//                 mvprintw(4, 0, "2.\U0001F948 %s(legend)                  %d                   %d                   %d                 %d", p[0].name, p[0].score, p[0].gold, p[0].n_login, p[0].exp);
//                 attroff(COLOR_PAIR(2));
//                 attron(COLOR_PAIR(4));
            
//                 mvprintw(6, 0, "3.\U0001F949 %s(champ)                    %d                   %d                   %d                 %d", p[2].name, p[2].score, p[2].gold, p[2].n_login, p[2].exp);   
//                 attroff(COLOR_PAIR(4));
//             }
//             refresh();
//         }
//         attron(COLOR_PAIR(9));
//         if(karbar == 4) {
//             mvprintw(8, 0, "\U000027A14.%s                                 %d                   %d                   %d                 %d", p[3].name, p[3].score, p[3].gold, p[3].n_login, p[3].exp); 

//         }
//         else {
//             mvprintw(8, 0, "4.%s                                 %d                   %d                   %d                 %d", p[3].name, p[3].score, p[3].gold, p[3].n_login, p[3].exp); 

//         }
//         refresh();
//         attroff(COLOR_PAIR(9));
//         char c = getchar();
//         if(c == '0') {
//             break;
//         }
//     }
//     menu();
// }
