#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <algorithm>

using namespace std;

enum game_status
{
    WIN,
    LOSE,
    PROCESS
};

enum move_direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    STAY
};

enum person_type
{
    ENEMY,
    PLAYER,
    NOBODY
};

struct character
{
    person_type type;
    string name = "unknown";
    int health = 0;
    int armor = 0;
    int damage = 0;
    int coordinate_x = 0;
    int coordinate_y = 0;
    bool is_alive = true;
};

struct map
{
    person_type type = NOBODY;
    vector<int> enemies;
};

//take damage func
void take_damage(map (&map)[20][20], character &attack, character &defender)
{
    cout << defender.name << " took damage: " << attack.damage << endl;
    defender.armor -= attack.damage;
    if (defender.armor < 0)
    {
        defender.health += defender.armor;
        defender.armor = 0;

        if (defender.health <= 0)
        {
            defender.is_alive = false;
            cout << defender.name << " is dead" << endl;
        }
    }
}

move_direction move_converted (string &move_str)
{
    if (move_str == "u")
        return UP;
    else if (move_str == "d")
        return DOWN;
    else if (move_str == "l")
        return LEFT;
    else if (move_str == "r")
        return RIGHT;
    else
        return STAY;
}

void monster_info (vector<character> &enemies)
{
    for (int i = 0; i < enemies.size(); ++i)
    {
        character &enemy = enemies[i];
        cout << enemy.name << "; HP - " << enemy.health
             << "; Armor - " << enemy.armor
             << "; position - " << enemy.coordinate_x << ", "
             << enemy.coordinate_y << endl;
        if (enemy.is_alive == true) cout << "Alive";
        else if (enemy.is_alive == false) cout << "Dead";
        cout << endl;
    }
}

void show_map (map (&map)[20][20], vector<character> &enemies, character &player)
{
    monster_info(enemies);

    //enemies mark
    for (int i = 0; i < enemies.size(); ++i)
    {
        character &enemy = enemies[i];
        if (enemy.is_alive == true)
        {
            map[enemy.coordinate_x][enemy.coordinate_y].type = enemy.type;
            map[enemy.coordinate_x][enemy.coordinate_y].enemies.push_back(i); //add monster`s massive in
        }
    }

    //player mark
    map[player.coordinate_x][player.coordinate_y].type = player.type;

    for (int i = 0; i < 20; ++i)
    {
        for (int j = 0; j < 20; ++j)
        {
            if (map[i][j].type == ENEMY)
            {
                cout << "E";
            }
            else if (map[i][j].type == PLAYER)
            {
                cout << "P";
            }
            else
            {
                cout << ".";
            }
        }
        cout << endl;
    }
}

void create_enemy (map (&map)[20][20], vector<character> &enemies)
{
    for (int i = 0; i < enemies.size(); ++i)
    {
        character &enemy = enemies[i];
        enemy.type = ENEMY;
        enemy.name = ("Enemy #" + to_string(i + 1));
        enemy.health = (rand() % 100) + 50;
        enemy.armor = (rand() % 50);
        enemy.damage = (rand() % 15) + 15;
        enemy.coordinate_x = ((rand() % 19) + 1);
        enemy.coordinate_y = ((rand() % 19) + 1);
        map[enemy.coordinate_x][enemy.coordinate_y].type = ENEMY;
    }
}

void create_player (map (&map)[20][20], character &player)
{
    cout << "Enter player name: ";
    cin >> player.name;
    cout << "Enter player health: ";
    cin >> player.health;
    cout << "Enter player armor: ";
    cin >> player.armor;
    cout << "Enter player damage: ";
    cin >> player.damage;

    player.type = PLAYER;
    player.coordinate_x = 0;
    player.coordinate_y = 0;
}

void save_character (ofstream &file, character &person)
{
    int len_name = person.name.length();
    file.write((char*) &len_name, sizeof(len_name));
    file.write(person.name.c_str(), len_name);
    file.write((char*) &person.health, sizeof(person.health));
    file.write((char*) &person.armor, sizeof(person.armor));
    file.write((char*) &person.damage, sizeof(person.damage));
    file.write((char*) &person.coordinate_x, sizeof(person.coordinate_x));
    file.write((char*) &person.coordinate_y, sizeof(person.coordinate_y));
    file.write((char*) &person.is_alive, sizeof(person.is_alive));
}

void save_map (map (&map)[20][20], vector<character> (&enemies), character &player)
{
    ofstream file("save.bin", ios::binary);

    save_character(file, player);

    for (int i = 0; i < 5; ++i)
    {
        save_character(file, enemies[i]);
    }
    file.close();
}

void load_character (ifstream &file, character &person)
{
    int len_name;

    file.read((char*) &len_name, sizeof(len_name));
    person.name.resize(len_name);
    file.read((char*) person.name.c_str(), len_name);
    file.read((char*) &person.health, sizeof(person.health));
    file.read((char*) &person.armor, sizeof(person.armor));
    file.read((char*) &person.damage, sizeof(person.damage));
    file.read((char*) &person.coordinate_x, sizeof(person.coordinate_x));
    file.read((char*) &person.coordinate_y, sizeof(person.coordinate_y));
    file.read((char*) &person.is_alive, sizeof(person.is_alive));
}

void load_map (map (&map)[20][20], vector<character>(&enemies), character &player)
{
    ifstream file("save.bin", ios::binary);

    load_character(file, player);

    for (int i = 0; i < 5; ++i)
    {
        load_character(file, enemies[i]);
    }
    file.close();

    for (int i = 0; i < 20; ++i)
    {
        for (int j = 0; j < 20; ++j)
        {
            map[i][j].enemies.clear();
        }
    }

    show_map(map, enemies, player);
}

void player_move (map (&map)[20][20], character &player, move_direction &direction)
{
    if (direction == UP && player.coordinate_x > 0)
    {
        map[player.coordinate_x][player.coordinate_y].type = NOBODY;
        player.coordinate_x -= 1;
    }
    else if (direction == DOWN && player.coordinate_x < 19)
    {
        map[player.coordinate_x][player.coordinate_y].type = NOBODY;
        player.coordinate_x += 1;
    }
    else if (direction == LEFT && player.coordinate_y > 0)
    {
        map[player.coordinate_x][player.coordinate_y].type = NOBODY;
        player.coordinate_y -= 1;
    }
    else if (direction == RIGHT && player.coordinate_y < 19)
    {
        map[player.coordinate_x][player.coordinate_y].type = NOBODY;
        player.coordinate_y += 1;
    }
}

void enemy_move (map (&map)[20][20], vector<character> &enemies, character &player)
{
    move_direction direction = static_cast<move_direction>(rand() & 4);
    string user_move;

    for (int i = 0; i < enemies.size(); ++i)
    {
        character &enemy = enemies[i];

        if (enemy.is_alive == true)
        {
            map[enemy.coordinate_x][enemy.coordinate_y].enemies.clear();

            player_move(map, enemy, direction);

            if ((enemy.coordinate_x == player.coordinate_x) &&
                (enemy.coordinate_y == player.coordinate_y))
            {
                take_damage(map, player, enemy);
            }
        }
    }
}

//load, save or play
void player_menu(map (&map)[20][20], vector<character> &enemies, character &player)
{
    string user_command;
    move_direction direction;

    cout << "Player menu (input command): ";
    cin >> user_command;

    if (user_command == "load")
    {
        load_map(map, enemies, player);
        return;
    }
    else if (user_command == "save")
    {
        save_map(map, enemies, player);
        return;
    }

    direction = move_converted(user_command);

    player_move(map, player, direction);

    //fight
    if (map[player.coordinate_x][player.coordinate_y].enemies.size() > 0)
    {
        for (int i = 0; i < map[player.coordinate_x][player.coordinate_y].enemies.size(); ++i)
        {
            take_damage(map, player, enemies[map[player.coordinate_x][player.coordinate_y].enemies[i]]);
        }
    }
}

void check_status (game_status &game_status, character &player, vector<character> &enemies)
{
    game_status = WIN;
    for (int i = 0; i < enemies.size(); ++i)
    {
        character &enemy = enemies[i];

        if (enemy.is_alive == true)
        {
            game_status = PROCESS;
        }
    }

    if (player.health <= 0)
    {
        game_status = LOSE;
    }
}

int main()
{
    srand(time(nullptr));
    string input_command = " ";
    game_status status = PROCESS;
    map map[20][20];
    vector<character> enemies(5);

    character player;

    cout << "Welcome! Do you want start new game or load previous one? (new/load): ";
    cin >> input_command;

    while (input_command != "new" && input_command != "load")
    {
        cout << "Wrong input! Try again: ";
        cin >> input_command;
    }

    if (input_command == "new")
    {
        create_enemy(map, enemies);

        create_player(map, player);

        show_map(map, enemies, player);
    }
    else if (input_command == "load")
    {
        load_map(map, enemies, player);
    }

    while (true)
    {
        player_menu(map, enemies, player);

        enemy_move(map, enemies, player);

        if (player.is_alive == false)
        {
            cout << player.name << " is dead" << endl;
            return 0;
        }

        show_map(map, enemies, player);

        check_status(status, player, enemies);
        if (status == WIN)
        {
            cout << "Player win" << endl;
            return 0;
        }
        else if (status == LOSE)
        {
            cout << "Game over!" << endl;
            return 0;
        }
    }
}