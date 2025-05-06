#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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
int main(){
//struct player waarden geven
struct player player;
//Naam van de player vragen aan de gebruiker.
printf("Give me the player name: \n");
fgets(player.name,sizeof(player.name),stdin);
player.name[strcspn(player.name, "\n")] = '\0'; //Dit is om de newline die de fgets functie genereert weg te halen als die er is.
printf("Welcome %s, have fun in this amazing game!\n",player.name);
//Health van player instellen
player.health = 100;
player.currentroomid = 1;
printf("You start with %d Health \n",player.health);
printf("You have spawned in room number %d, this is where your adventure will start\n",player.currentroomid);
//structs voor kamers instellen:
}