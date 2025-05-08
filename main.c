#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

int monsterattack = 0;
int playerattack = 0;
int number = 0;
int generatenewnumber = 0;
int gameover = 0;
//structs voor spelers:
struct player {
    char name[50];
    int health;
    int damage;
    int currentroomid;
    };
//structs voor kamers:
struct room {
    int roomid;
    int doors;
    int monsters;
    int searched;
    _Bool Visited;
};
//struct voor monsters:
struct monsters{
    char *name;
    int health;
    int damage;
};

//Zorgen dat we een getal om kunnen zetten in een binair getal
void ToBinary(){
//printf("%d\n",number); uncomment to see if this works.
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

int main(){
srand(time(NULL));
//struct player waarden geven
struct player player;
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
//structs voor kamers instellen:
struct room room;

//Monster 1
struct monsters monster1;
monster1.name = "GanjaTron";
monster1.health = 200;
monster1.damage = 2;
//Monster 2
struct monsters monster2;
monster2.name = "CBD-Rex";  
monster2.health = 400;
monster2.damage = 90;

//Getal voor monster battle:
while(monster1.health > 0 && player.health > 0){
number = rand() % 17;
ToBinary(number);
if(monsterattack == 1){
    player.health = (player.health - monster1.damage);
    printf("The monster attacked you\n");
}
if(playerattack == 1){
    monster1.health = (monster1.health - player.damage);
    printf("HIT!, Your shot landed!\n");
}
}
if(monster1.health <= 0){
    printf("ARGHHH, %s has been killed!\n",monster1.name);
}
if(player.health <= 0){
    printf("%s Died\n",player.name);
    gameover = 1;
}
if(gameover == 1){
    printf("Game Over!");
    return 0;
}
}