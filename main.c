#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

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
    dungeon* dungeon = malloc(sizeof(dungeon));
    dungeon -> amountofrooms = amountofrooms;
    dungeon -> rooms = malloc(amountofrooms * sizeof(struct room));

    for (int i = 0; i < amountofrooms; i++){
        dungeon -> rooms[i].roomid = i;
        dungeon -> rooms[i].doors = 0;
        dungeon -> rooms[i].connections = NULL;
        dungeon -> rooms[i].monsterchoice = rand() % 2;
    }
    
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

void battle(){
    //Getal voor monster battle:
    while(monster1.health > 0 && player.health > 0){
    number = rand() % 17;
    ToBinary(number);
    if(monsterattack == 1){
        player.health = (player.health - monster1.damage);
        printf("The monster attacked you! Focus Up %s! You have %d health.\n",player.name,player.health);
    }
    if(playerattack == 1){
        monster1.health = (monster1.health - player.damage);
        printf("HIT!, Your shot landed! %s now has %d health.\n",monster1.name,monster1.health);
    }
    }
    //Kijken of het monster al dood is
    if(monster1.health <= 0){
        printf("ARGHHH, %s has been killed!\n",monster1.name);
    }
    //Kijken of player al dood is
    if(player.health <= 0){
        printf("%s Died\n",player.name);
        gameover = 1;
    }
    if(gameover == 1){
        printf("Game Over!");
        return;
    }
    }
    
    
    //chests 
    //er moet nog een stuk komen waar we random genereren in welke kamer er een chest staat
    /*void treasurechests(){
    int specialitem == 0;
        if (chest == 1){
        
        }
        if (specialitem == 1){
            printf("You found the Golden Banana, Ur the MonkeyGod now, they will do everything that needs to be done for you\n");
            printf("You Won! Thankyou for playing %s\n",player.name);
            return 0;
        }
    }
    */
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
player.currentroomid = 1;

printf("You start with %d Health \n",player.health);
printf("You have spawned in room number %d, this is where your adventure will start\n",player.currentroomid);

//dungeon generatie
int amountofrooms;
printf("How many rooms do you want to have in the game? (2-10): ");
scanf("%d", &amountofrooms);
if(amountofrooms < 2 || amountofrooms > 10){
    printf("Because u are not smart enough to follow a simple instruction, i will choose the amount of rooms for you\n");
    amountofrooms = 5;
}
dungeon* dungeon = dungeoncreate(amountofrooms);
player.currentroomid = 0; //Start in de eerste kamer

//Monster 1
monster1.name = "GanjaTron";
monster1.health = 200;
monster1.damage = 2;
//Monster 2
monster2.name = "CBD-Rex";  
monster2.health = 400;
monster2.damage = 90;

while(!gameover){
    struct room* currentroom = &dungeon -> rooms[player.currentroomid];
    printf("\nYou are in room %d\n", currentroom -> roomid);
    printf("Connections to rooms: ");
    roomnode* node = currentroom -> connections;
    while(node != NULL){
        printf("%d ", node -> room -> roomid);
        node = node -> next;
    }
    
    if(currentroom -> monsterchoice){
        printf("\nA wild %s appears!\n", monster1.name);
        battle();
        if(gameover) break;
        currentroom -> monsterchoice = 0;
    }
    
    // Kamernavigatie
    int newroom;
    printf("\nEnter room number to move to (-1 to quit): ");
    scanf("%d", &newroom);
    if(newroom == -1) break;
    
    _Bool valid = 0;
    node = currentroom -> connections;
    while(node != NULL){
        if(node -> room -> roomid == newroom){
            valid = 1;
            break;
        }
        node = node -> next;
    }
    
    if(valid){
    player.currentroomid = newroom;
    } 
    else{
    printf("Invalid room selection!\n");
        }
    }
freedungeons(dungeon);
return 0;
}

/*//deze voorwaarden is om te testen of het werkt MOET NOG VERANDEREN!!
int fight = 1;
if (fight == 1){
   battle();*/

