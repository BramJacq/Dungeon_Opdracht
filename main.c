#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define GanjaTronHealth 200
#define CBD_RexHealth 400
#define GanjaTronDamage 2
#define CBD_RexDamage 5

typedef struct roomnode{
    struct room* room;
    struct roomnode* next;
} roomnode;

//structs voor kamers:
struct room {
    int roomid;
    int doors;
    int monsterchoice; //0 = No monster in room, 1 = monster1 is in the room, 2 = monster2 is in the room
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
int generatenewnumber = 0;
int gameover = 0;
int chest = 0;

//structs voor spelers:
struct player {
    char name[50];
    int health;
    int damage;
    int currentroomid;
    };

//struct voor monsters:
struct monsters{
    char *name;
    int health;
    int damage;
};

struct player player;
struct monsters monster1;
struct monsters monster2;

//Ik heb beslist om met random getallen ervoor te zorgen
//dat er 25% kans is dat er een monster in de kamer zit
//dan is er een random getal dat 33% kans geeft dat het monster2 omdat deze heel sterk is.
//er is 66% kans dat het het simpele monster1 is.
void generatemonsters(struct room *r){
    if ((rand() % 4) == 0){ //25% kans dat er een monster in de kamer aanwezig is.
        if ((rand() % 3) == 0){
            r -> monsterchoice = 2; //33% kans dat het monster2 is.
        }
        else{
            r -> monsterchoice = 1; //66% kans dat het monster1 is.
        }
    }
    else{
    r -> monsterchoice = 0; //Geen monster in de kamer (75% kans).
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

    // Initialize rooms hier geven we de kamer een id en zorgen we dat we met 0 deuren per kamer beginnen.
    for (int i = 0; i < amountofrooms; i++) {
        dungeon->rooms[i].roomid = i;
        dungeon->rooms[i].doors = 0;
        dungeon->rooms[i].connections = NULL;
        dungeon->rooms[i].visited = 0;
        dungeon->rooms[i].cleared = 0;
        dungeon->rooms[i].searched = 0;
        dungeon->rooms[i].specialitem = 0;
        // Generate monsters for this room
        generatemonsters(&dungeon->rooms[i]);
    }
    //specialitem in random kamer laten voorkomen.
        int endroom = rand() % amountofrooms;
        dungeon->rooms[endroom].specialitem = 1;

    // zorgen dat we beginnen bij kamer 1 want kamer 0 is de ingang, en zoekt een willekeurige kamer om mee te verbinden
    for(int i = 1; i < amountofrooms; i++){
        int target = room_availability(dungeon, i);
        if (target == -1){
            free(dungeon -> rooms);
            free(dungeon);
            return NULL;
        }
    connectrooms(&dungeon -> rooms[i], &dungeon -> rooms[target]);

    // Add random additional connections (0-3)
    int extraconnections = rand() % 4; //Kiest of er 0 tot 3 extra verbindingen gegenereerd gaan worden.
    for (int j = 0; j < extraconnections; j++){
    int newtarget = room_availability(dungeon, i);
    if (newtarget != -1 && dungeon -> rooms[i].doors < 4){ //controleren dat er nog plaats is voor een extra connectie want 4 deuren is het maximum dat mag.
        connectrooms(&dungeon -> rooms[i], &dungeon -> rooms[newtarget]);
        }
    }
}
return dungeon;
}

void freedungeons(dungeon* dungeon){
    for (int i = 0; i < dungeon -> amountofrooms; i++){//hier geven we de knooppunten vrij
        roomnode* current = dungeon -> rooms[i].connections;
        while (current != NULL) {  //array van de kamers vrij
            roomnode* temp = current; 
            current = current -> next;
            free(temp);
        }
    }
    free(dungeon -> rooms); //hier geven we de blueprint vrij
    free(dungeon);
}
//Zorgen dat we een getal om kunnen zetten in een binair getal
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
            return;
        }
        if(rand() % 4 == 0) { // 25% kans op item
            int item_type = rand() % 3;
            
            switch(item_type) {
                case 0: { // Health potion geeft je 20 tot 50 meer health
                    int heal = 20 + rand() % 31;
                    player.health += heal;
                    printf("\nYou found a health potion! +%d health!\n", heal);
                    printf("You now have %d health\n", player.health);
                    break;
                }
                case 1: { // Zwaard
                    int original_damage = player.damage; //verhoogt playerdamage *2
                    player.damage *= 2;
                    printf("\nYou found a banana sword! ");
                    printf("Damage increased from %d to %d!\n", original_damage, player.damage);
                    break;
                }
                case 2: { // Lego
                    int damage = 20 + rand() % 31;
                    player.health -= damage;
                    printf("\nOUCH! You stepped on a Lego brick! -%d health!\n", damage);
                    printf("You now have %d health\n", player.health);
                    }
                break;
            }
        }
    }
}

void battle(struct monsters *m, struct room *currentroom){
    printf("Watch out!! Whats that? Oh no its a %s\n",m -> name);
    //Getal voor monster battle:
    while(m -> health > 0 && player.health > 0){
    number = rand() % 17;
    ToBinary();
    if(monsterattack == 1){
        player.health -= (m -> damage);
        printf("The %s attacked you! Focus Up %s! You have %d health.\n",m -> name,player.name,player.health);
    }
    if(playerattack == 1){
        m -> health -= player.damage;
        printf("HIT!, Your shot landed! %s now has %d health.\n",m -> name,m -> health);
        }
    }
    //Kijken of het monster al dood is
    if(m -> health <= 0){
        printf("ARGHHH, %s has been killed!\n",m -> name);
        currentroom -> cleared = 1; //Kamer is veilig.
    }
    //Kijken of player al dood is
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
    
int main(){
srand(time(NULL));

//Naam van de player vragen aan de gebruiker.
printf("Give me the player name: \n");
fgets(player.name,sizeof(player.name),stdin);
player.name[strcspn(player.name, "\n")] = '\0'; //Dit is om de newline die de fgets functie genereert weg te halen als die er is.
printf("Welcome %s, have fun in this amazing game!\n",player.name);

//Health van player instellen
player.health = 100;
player.damage = 5;

//dungeon generatie
int amountofrooms;
printf("How many rooms do you want to have in the game? (2-25): ");
scanf("%d", &amountofrooms);
if(amountofrooms < 2 || amountofrooms > 25){
    printf("Because u are not smart enough to follow a simple instruction, i will choose the amount of rooms for you\n");
    amountofrooms = 10;
}
dungeon* dungeon = dungeoncreate(amountofrooms);
player.currentroomid = 0; //Start in de eerste kamer

//Monster 1
monster1.name = "GanjaTron";
monster1.health = GanjaTronHealth;
monster1.damage = GanjaTronDamage;
//Monster 2
monster2.name = "CBD-Rex";  
monster2.health = CBD_RexHealth;
monster2.damage = CBD_RexDamage;

while(!gameover){
    struct room* currentroom = &dungeon -> rooms[player.currentroomid];
    currentroom->visited = 1;

    printf("\nYou are in room %d\n", currentroom -> roomid);
    //Zorgen dat we alleen de verbinding van de kamer weergeven als hij veilig is
    if(currentroom -> cleared){
        printf("Connections: ");
        roomnode* node = currentroom -> connections;
        while(node != NULL){
            printf("%d ", node -> room -> roomid);
            node = node -> next;
        }
    }
    //Items beheren
    items(currentroom);
    if(gameover == 1){
        return 0;
        freedungeons(dungeon);
        break;
    }
    //gevecht starten als dat nodig is:
    if(!currentroom -> cleared && currentroom -> monsterchoice){
        switch(currentroom -> monsterchoice){
            case 1: 
                monster1.health = GanjaTronHealth;
                battle(&monster1, currentroom);
                break;
            case 2: 
                monster2.health = CBD_RexHealth;
                battle(&monster2, currentroom);
                break;
        }
        if(gameover == 1){
            break;
            return 0;
        }
    }

    //laat beschikbare kamers zien voor navigatie
    printf("\nAvailable rooms: ");
    roomnode* node = currentroom->connections;
    while(node != NULL){
        printf("%d ", node->room->roomid);
        node = node->next;
    }

    int newroom;
    printf("\nEnter room number to move to (-1 to quit): ");
    scanf("%d", &newroom);
    getchar();

    if(newroom == -1){
    break;
    }

    _Bool valid = 0;
    node = currentroom -> connections;
    while(node && !valid){
        if(node -> room -> roomid == newroom){
            valid = 1;
            player.currentroomid = newroom;
        }
        node = node -> next;
    }  
    
    if(!valid){
        printf("Invalid room selection!\n");
    } 
}
if(gameover == 1){
    freedungeons(dungeon);
    return 0;
}
freedungeons(dungeon);
return 0;
}