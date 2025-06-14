#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define SpidermanmonkeyHealth 200
#define GigantopithecusHealth 400
#define SpidermanmonkeyDamage 2
#define GigantopithecusDamage 5

typedef struct roomnode{
    struct room* room;
    struct roomnode* next;
} roomnode;

struct room {
    int roomid;
    int doors;
    int monsterchoice;
    int searched;
    _Bool visited;
    _Bool cleared;
    _Bool specialitem;
    roomnode* connections;
};

typedef struct{
    int amountofrooms;
    struct room* rooms;
} dungeon;

int monsterattack = 0;
int playerattack = 0;
int number = 0;
int gameover = 0;

struct player {
    char name[50];  
    int health;
    int damage;
    int currentroomid;
    };

struct monsters{
    char *name;
    int health;
    int damage;
};

struct player player;
struct monsters monster1;
struct monsters monster2;

typedef void (*MonsterGenerator)(struct room*);
typedef void (*BattleHandler)(struct monsters*, struct room*);
typedef void (*ItemHandler)(struct room*);
void generatemonsters(struct room *r);
void battle(struct monsters *m, struct room *currentroom);

MonsterGenerator generate_monsters;
BattleHandler battle_handler;

void handle_health_potion(struct room *r) {
    int heal = 20 + rand() % 31;
    player.health += heal;
    printf("\nYou found a health potion! +%d health!\n", heal);
    printf("You now have %d health\n", player.health);
}

void handle_sword(struct room *r) {
    int original_damage = player.damage;
    player.damage *= 2;
    printf("\nYou found a banana sword! ");
    printf("Damage increased from %d to %d!\n", original_damage, player.damage);
}

void handle_lego(struct room *r) {
    int damage = 20 + rand() % 31;
    player.health -= damage;
    printf("\nOUCH! You stepped on a Lego brick! -%d health!\n", damage);
    printf("You now have %d health\n", player.health);
}

ItemHandler item_handlers[3] = {
    &handle_health_potion,
    &handle_sword,
    &handle_lego
};

void generatemonsters(struct room *r){
    if ((rand() % 4) == 0){ 
        if ((rand() % 3) == 0){
            r -> monsterchoice = 2;
        }
        else{
            r -> monsterchoice = 1;
        }
    }
    else{
    r -> monsterchoice = 0;
    }
}


void connectrooms(struct room* room1, struct room* room2){
    roomnode* current = room1 -> connections;
    while (current != NULL) {
        if (current -> room == room2) return;
        current = current -> next;
    }

    roomnode* node1 = malloc(sizeof(roomnode));
    node1 -> room = room2;
    node1 -> next = room1 -> connections;
    room1-> connections = node1;
    room1 -> doors++;

    roomnode* node2 = malloc(sizeof(roomnode));
    node2 -> room = room1;
    node2 -> next = room2 -> connections;
    room2-> connections = node2;
    room2 -> doors++;
}

int room_availability(dungeon* dungeon, int current_id){
    int roomstocheck[dungeon -> amountofrooms];
    int counter = 0;

    for(int i = 0; i < current_id; i++){
        if(dungeon -> rooms[i].doors < 4){
            roomstocheck[counter++] = i;
        }
    }
    if(counter > 0){
    return roomstocheck[rand() % counter];
    }
    else{
    return -1;
    }
}


dungeon* dungeoncreate(int amountofrooms){
    dungeon* dungeon = malloc(sizeof(*dungeon));
    dungeon->amountofrooms = amountofrooms;
    dungeon->rooms = malloc(amountofrooms * sizeof(struct room));

    for (int i = 0; i < amountofrooms; i++) {
        dungeon->rooms[i].roomid = i;
        dungeon->rooms[i].doors = 0;
        dungeon->rooms[i].connections = NULL;
        dungeon->rooms[i].visited = 0;
        dungeon->rooms[i].cleared = 0;
        dungeon->rooms[i].searched = 0;
        dungeon->rooms[i].specialitem = 0;
        (*generate_monsters)(&dungeon->rooms[i]);
    }

        int endroom = rand() % amountofrooms;
        dungeon->rooms[endroom].specialitem = 1;

    for(int i = 1; i < amountofrooms; i++){
        int target = room_availability(dungeon, i);
        if (target == -1){
            free(dungeon -> rooms);
            free(dungeon);
            return NULL;
        }
    connectrooms(&dungeon -> rooms[i], &dungeon -> rooms[target]);

    int extraconnections = rand() % 4;
    for (int j = 0; j < extraconnections; j++){
    int newtarget = room_availability(dungeon, i);
    if (newtarget != -1 && dungeon -> rooms[i].doors < 4){
        connectrooms(&dungeon -> rooms[i], &dungeon -> rooms[newtarget]);
        }
    }
}
return dungeon;
}

void freedungeons(dungeon* dungeon){
    for (int i = 0; i < dungeon -> amountofrooms; i++){
        roomnode* current = dungeon -> rooms[i].connections;
        while (current != NULL) {
            roomnode* temp = current; 
            current = current -> next;
            free(temp);
        }
    }
    free(dungeon -> rooms);
    free(dungeon);
}

void ToBinary(){
int binarynumber[10];
int i = 0;
    while (number > 0){
        binarynumber[i] = number % 2;
        number = number / 2;
        i++;
    }
    for(int j = i - 1; j >= 0; j--){
        if (binarynumber[j] == 0){
            monsterattack = 1;
            playerattack = 0;
        }
        else if(binarynumber[j] == 1){
            monsterattack = 0;
            playerattack = 1;
        }
    }
}

void items(struct room *currentroom) {
    if(currentroom->searched == 0){
        currentroom->searched = 1;
        
        if(currentroom->specialitem) {
            printf("\nYou found the GOLDEN BANANA!\n");
            printf("You win!! You are the MonkeyGod now, they will do the rest of the work for you!\n");
            gameover = 1;
            remove("save.dat");
            return;
        }
        if(rand() % 4 == 0) { 
            int item_type = rand() % 3;
            (*item_handlers[item_type])(currentroom);
        }
    }
}

void handle_player_attack(struct monsters *m, struct room *r) {
    m->health -= player.damage;
    printf("HIT!, Your shot landed! %s now has %d health.\n",m->name,m->health);
}

void handle_monster_attack(struct monsters *m, struct room *r) {
    player.health -= (m->damage);
    printf("The %s attacked you! Focus Up %s! You have %d health.\n",m->name,player.name,player.health);
}

void battle(struct monsters *m, struct room *currentroom){
    printf("Watch out!! Whats that? Oh no its a %s\n",m -> name);
    while(m -> health > 0 && player.health > 0){
    number = rand() % 17;
    ToBinary();
    if(monsterattack == 1){
        handle_monster_attack(m, currentroom);
    }
    if(playerattack == 1){
        handle_player_attack(m, currentroom);
        }
    }
    if(m -> health <= 0){
        printf("ARGHHH, %s has been killed!\n",m -> name);
        currentroom -> cleared = 1;
    }
    if(player.health <= 0){
        printf("%s Died\n",player.name);
        gameover = 1;
    }
    if(currentroom -> cleared == 1){
        printf("You were able to eat its organs and got 100 extra health!\n");
        player.health += 100;
        printf("You now have %d health points.\n",player.health);
    }
}
    
void save_game(dungeon *d, struct player *p) {
    FILE *f = fopen("save.dat", "wb");
    if (!f){
    return;
    }
    fwrite(p, sizeof(struct player), 1, f);
    fwrite(&d->amountofrooms, sizeof(int), 1, f);
    
    for (int i = 0; i < d->amountofrooms; i++) {
        struct room *r = &d->rooms[i];
        fwrite(r, sizeof(struct room), 1, f);
        
        int connection_count = 0;
        roomnode *node = r->connections;
        while (node) {
            connection_count++;
            node = node->next;
        }
        fwrite(&connection_count, sizeof(int), 1, f);
        
        node = r->connections;
        while (node) {
            int room_id = node->room->roomid;
            fwrite(&room_id, sizeof(int), 1, f);
            node = node->next;
        }
    }
    fclose(f);
}

int load_game(dungeon **d, struct player *p) {
    FILE *f = fopen("save.dat", "rb");
    if (!f) return 0;

    fread(p, sizeof(struct player), 1, f);
    
    int room_count;
    fread(&room_count, sizeof(int), 1, f);
    
    *d = malloc(sizeof(dungeon));
    (*d)->amountofrooms = room_count;
    (*d)->rooms = malloc(room_count * sizeof(struct room));
    
    for (int i = 0; i < room_count; i++) {
        struct room *r = &(*d)->rooms[i];
        fread(r, sizeof(struct room), 1, f);
        r->connections = NULL;
        
        int connection_count;
        fread(&connection_count, sizeof(int), 1, f);
        
        for (int j = 0; j < connection_count; j++) {
            int target_id;
            fread(&target_id, sizeof(int), 1, f);
            
            roomnode *new_node = malloc(sizeof(roomnode));
            new_node->room = &(*d)->rooms[target_id];
            new_node->next = r->connections;
            r->connections = new_node;
        }
    }
    fclose(f);
    return 1;
}

    int main(){
        srand(time(NULL));
        dungeon* dungeon = NULL;

        FILE *test = fopen("save.dat", "rb");
        if (test) {
            fclose(test);
            printf("Saved game found. Load? (1=Yes, 0=No): ");
            int choice;
            scanf("%d", &choice);
            while(getchar() != '\n');

            if (choice == 1) {
                if (!load_game(&dungeon, &player)) {
                    printf("Failed to load save. Starting new game.\n");
                }
            }
        }
        generate_monsters = &generatemonsters;
        battle_handler = &battle;
        if (!dungeon) {        
            printf("Give me the player name: ");
            fgets(player.name,sizeof(player.name),stdin);
            player.name[strcspn(player.name, "\n")] = '\0';
            printf("Welcome %s, have fun in this amazing game!\n",player.name);

            player.health = 100;
            player.damage = 5;

            int amountofrooms;
            printf("How many rooms do you want to have in the game? (2-25): ");
            scanf("%d", &amountofrooms);
            while(getchar() != '\n');
            
            if(amountofrooms < 2 || amountofrooms > 25){
                printf("Because u are not smart enough to follow a simple instruction, i will choose the amount of rooms for you\n");
                amountofrooms = 10;
            }
            dungeon = dungeoncreate(amountofrooms);
            player.currentroomid = 0;
        }

        monster1.name = "Spidermanmonkey";
        monster1.health = SpidermanmonkeyHealth;
        monster1.damage = SpidermanmonkeyDamage;

        monster2.name = "Gigantopithecus";  
        monster2.health = GigantopithecusHealth;
        monster2.damage = GigantopithecusDamage;

        while(!gameover){
            struct room* currentroom = &dungeon -> rooms[player.currentroomid];
            currentroom->visited = 1;

            printf("You are in room %d\n", currentroom -> roomid);
            if(currentroom -> cleared){
                printf("Connections: ");
                roomnode* node = currentroom -> connections;
                while(node != NULL){
                    printf("%d ", node -> room -> roomid);
                    node = node -> next;
                }
            }

            items(currentroom);
            if(gameover == 1){
                freedungeons(dungeon);
                return 0;
            }

            if(!currentroom -> cleared && currentroom -> monsterchoice){
                switch(currentroom -> monsterchoice){
                    case 1: 
                        monster1.health = SpidermanmonkeyHealth;
                        (*battle_handler)(&monster1, currentroom);
                        break;
                    case 2: 
                        monster2.health = GigantopithecusHealth;
                        (*battle_handler)(&monster2, currentroom);
                        break;
                }
                if(gameover == 1){
                    break;
                }
            }

            printf("\nAvailable rooms: ");
            roomnode* node = currentroom->connections;
            while(node != NULL){
                printf("%d ", node->room->roomid);
                node = node->next;
            }

            char input[10];
            printf("\nEnter room number to move to (-1 to quit, 'save' to save): ");
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = '\0';

            if(strcmp(input, "save") == 0) {
                save_game(dungeon, &player);
                printf("Game saved!\n");
                continue;
            }

            int newroom = atoi(input);
            if(newroom == -1) break;

            _Bool valid = 0;
            node = currentroom -> connections;
            while(node && !valid){
                if(node -> room -> roomid == newroom){
                    printf("%s is leaving room %d and going to room %d\n",player.name,player.currentroomid,newroom);
                    valid = 1;
                    player.currentroomid = newroom;
                }
                node = node -> next;
            }  
            
            if(!valid){
                printf("Invalid room selection!\n");
            } 
        }
        
        freedungeons(dungeon);
        return 0;
    }