#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_CONNECTIONS 4
#define MAX_NAME_LENGTH 32

typedef enum { EMPTY, MONSTER, ITEM, TREASURE } RoomContent;
typedef enum { GOBLIN, ORC } MonsterType;
typedef enum { HEALTH_POTION, DAMAGE_BOOST } ItemType;

typedef struct Monster {
    MonsterType type;
    int hp;
    int damage;
} Monster;

typedef struct Item {
    ItemType type;
    int value;
} Item;

typedef struct Room {
    int id;
    RoomContent content;
    int visited;
    union {
        Monster* monster;
        Item* item;
    };
    struct Room** connections; // adjacency list
    int connectionCount;
} Room;

typedef struct Player {
    int hp;
    int damage;
    Room* currentRoom;
} Player;

// Functie prototypes
Room* createDungeon(int numberOfRooms);
Room* createRoom(int id);
void connectRooms(Room* a, Room* b);
void freeDungeon(Room* rooms, int numberOfRooms);
void exploreRoom(Player* player);
void fight(Player* player, Monster* monster);
void saveGame(Player* player, Room* rooms, int numberOfRooms, const char* filename);
void loadGame(Player* player, Room** rooms, int* numberOfRooms, const char* filename);

void printRoomInfo(Room* room);
int randomInRange(int min, int max);

// ---- MAIN ----
int main(int argc, char* argv[]) {
    srand(time(NULL));

    if (argc < 3) {
        printf("Usage: %s [new/load] [number_of_rooms/filename]\n", argv[0]);
        return 1;
    }

    Player player;
    Room* rooms = NULL;
    int numberOfRooms = 0;

    if (strcmp(argv[1], "new") == 0) {
        numberOfRooms = atoi(argv[2]);
        rooms = createDungeon(numberOfRooms);
        player.hp = 20;
        player.damage = 5;
        player.currentRoom = &rooms[0];
    } else if (strcmp(argv[1], "load") == 0) {
        loadGame(&player, &rooms, &numberOfRooms, argv[2]);
    } else {
        printf("Unknown option %s\n", argv[1]);
        return 1;
    }

    int playing = 1;
    while (playing && player.hp > 0) {
        exploreRoom(&player);

        if (player.currentRoom->content == TREASURE) {
            printf("Gefeliciteerd! Je hebt de schat gevonden!\n");
            break;
        }

        printf("Welke deur kies je? ");
        int choice;
        scanf("%d", &choice);

        if (choice >= 0 && choice < player.currentRoom->connectionCount) {
            player.currentRoom = player.currentRoom->connections[choice];
        } else {
            printf("Ongeldige keuze.\n");
        }
    }

    char saveChoice;
    printf("Wil je het spel opslaan? (y/n): ");
    scanf(" %c", &saveChoice);

    if (saveChoice == 'y' || saveChoice == 'Y') {
        saveGame(&player, rooms, numberOfRooms, "savegame.dat");
    }

    freeDungeon(rooms, numberOfRooms);
    return 0;
}

// ---- FUNCTIES ----
Room* createDungeon(int numberOfRooms) {
    Room* rooms = malloc(sizeof(Room) * numberOfRooms);

    for (int i = 0; i < numberOfRooms; i++) {
        rooms[i] = *createRoom(i);
    }

    // Random verbindingen leggen
    for (int i = 0; i < numberOfRooms; i++) {
        int connections = randomInRange(1, MAX_CONNECTIONS);
        rooms[i].connections = malloc(sizeof(Room*) * connections);
        rooms[i].connectionCount = 0;

        for (int j = 0; j < connections; j++) {
            int target = randomInRange(0, numberOfRooms - 1);
            connectRooms(&rooms[i], &rooms[target]);
        }
    }

    // Schat plaatsen
    int treasureRoom = randomInRange(1, numberOfRooms - 1);
    rooms[treasureRoom].content = TREASURE;

    return rooms;
}

Room* createRoom(int id) {
    Room* room = malloc(sizeof(Room));
    room->id = id;
    room->content = (rand() % 3 == 0) ? MONSTER : ((rand() % 2 == 0) ? ITEM : EMPTY);
    room->visited = 0;
    room->connections = NULL;
    room->connectionCount = 0;

    if (room->content == MONSTER) {
        room->monster = malloc(sizeof(Monster));
        room->monster->type = rand() % 2;
        room->monster->hp = (room->monster->type == GOBLIN) ? 8 : 12;
        room->monster->damage = (room->monster->type == GOBLIN) ? 3 : 5;
    } else if (room->content == ITEM) {
        room->item = malloc(sizeof(Item));
        room->item->type = rand() % 2;
        room->item->value = (room->item->type == HEALTH_POTION) ? 5 : 2;
    }

    return room;
}

void connectRooms(Room* a, Room* b) {
    if (a->connectionCount < MAX_CONNECTIONS && b->connectionCount < MAX_CONNECTIONS) {
        a->connections[a->connectionCount++] = b;
        b->connections[b->connectionCount++] = a;
    }
}

void exploreRoom(Player* player) {
    Room* room = player->currentRoom;

    printf("\nJe bent in kamer %d\n", room->id);

    if (!room->visited) {
        switch (room->content) {
            case MONSTER:
                printf("Er is een monster hier!\n");
                fight(player, room->monster);
                free(room->monster);
                room->content = EMPTY;
                break;
            case ITEM:
                printf("Je hebt een item gevonden!\n");
                if (room->item->type == HEALTH_POTION) {
                    player->hp += room->item->value;
                    printf("Je HP is verhoogd met %d! (Totaal: %d)\n", room->item->value, player->hp);
                } else if (room->item->type == DAMAGE_BOOST) {
                    player->damage += room->item->value;
                    printf("Je Damage is verhoogd met %d! (Totaal: %d)\n", room->item->value, player->damage);
                }
                free(room->item);
                room->content = EMPTY;
                break;
            case TREASURE:
                printf("Je ziet de SCHAT!\n");
                break;
            case EMPTY:
                printf("De kamer is leeg.\n");
                break;
        }
        room->visited = 1;
    } else {
        printf("Je bent hier al geweest. De kamer is leeg.\n");
    }

    printf("Deuren naar: ");
    for (int i = 0; i < room->connectionCount; i++) {
        printf("%d ", room->connections[i]->id);
    }
    printf("\n");
}

void fight(Player* player, Monster* monster) {
    while (player->hp > 0 && monster->hp > 0) {
        int sequence = rand() % 16; // 0 - 15 => 4 bits
        printf("Aanval volgorde: ");
        for (int i = 3; i >= 0; i--) {
            int bit = (sequence >> i) & 1;
            printf("%d", bit);

            if (bit == 0) {
                player->hp -= monster->damage;
                printf(" (Monster valt aan!) ");
            } else {
                monster->hp -= player->damage;
                printf(" (Speler valt aan!) ");
            }

            if (player->hp <= 0 || monster->hp <= 0) break;
        }
        printf("\nSpeler HP: %d, Monster HP: %d\n", player->hp, monster->hp);
    }

    if (player->hp <= 0) {
        printf("Je bent verslagen!\n");
    } else {
        printf("Je hebt het monster verslagen!\n");
    }
}

void freeDungeon(Room* rooms, int numberOfRooms) {
    for (int i = 0; i < numberOfRooms; i++) {
        free(rooms[i].connections);
    }
    free(rooms);
}

void saveGame(Player* player, Room* rooms, int numberOfRooms, const char* filename) {
    FILE* f = fopen(filename, "wb");
    if (!f) {
        perror("Fout bij opslaan");
        return;
    }
    fwrite(&numberOfRooms, sizeof(int), 1, f);
    fwrite(player, sizeof(Player), 1, f);
    fwrite(rooms, sizeof(Room), numberOfRooms, f);
    fclose(f);
}

void loadGame(Player* player, Room** rooms, int* numberOfRooms, const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) {
        perror("Fout bij laden");
        exit(1);
    }
    fread(numberOfRooms, sizeof(int), 1, f);
    *rooms = malloc(sizeof(Room) * (*numberOfRooms));
    fread(player, sizeof(Player), 1, f);
    fread(*rooms, sizeof(Room), *numberOfRooms, f);
    fclose(f);

    player->currentRoom = &(*rooms)[player->currentRoom->id];
}

int randomInRange(int min, int max) {
    return min + rand() % (max - min + 1);
}
