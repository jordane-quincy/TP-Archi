/*
    TP mémoire cache - Jordane QUINCY et Jean-Baptiste DURIEZ
    Etat : Travail en cours
    Code : 'D' + 'Q' = 68 + 81 = 149, Gestion Ecriture = 149%2 = 1 = WB, Remplacement des blocs = 149%4 = 1 = LRU
*/

#include <stdlib.h>
#include <stdio.h>


typedef struct s {
    int valid;
    double tag;
    //On a besoin d'un compteur pour gérer le LRU
    int compteur;
    int M;
}bloc;

typedef bloc **Cache;

Cache initialiser (int cs, int asso) {
    Cache C;
    int nbrElement = cs / asso;
    int i, j;
    C = malloc(nbrElement * sizeof (bloc));
    for (i = 0; i < nbrElement; i++) {
        C[i] = malloc(asso * sizeof(bloc));
    }
    for (i = 0; i < nbrElement; i++) {
        for (j = 0; j < asso; j++) {
            C[i][j].valid = 0;
        }
    }
    return C;
};

int getNbCyclePerdu (int bs, int nbDefautLecture, int nbDefautEcriture, int nbLigneSupprDuCache)
{
	//Pénalité d’un défaut = (12 + bs/8) cycles
	//Cycles perdu par la mémoire =
	// nombre de cycles dues à la pénalité des défauts * (nombre défauts lecture + nombre défauts écriture + nombre de lignes supprimées du cache)
    return (12 + bs/8) * (nbDefautLecture + nbDefautEcriture + nbLigneSupprDuCache);
};

void main(int argc, char *argv[]) {
    /*int cs;
    int bs;
    int asso;
    char trace[50];
    printf("Donnez dans un premier temps les données que nous allons utiliser\n");
    printf("Quelle est la taille du cache ? \n");
    scanf("%d", &cs);
    printf("Quelle est la taille d'un bloc ? \n");
    scanf("%d", &bs);
    printf("Quel est le degré d'associativité du cache ? \n");
    scanf("%d", &asso);
    printf("Dans quel fichier se trouve la trace à tester ? \n");
    scanf("%s", trace);
    int test = ((int)'D' + (int)'Q')%4;

    printf("%d", test);*/
    //Tester si on a le bon nombre d'argument
    if (argc != 5) {
        printf("Le nombre d'arguments est invalide, il doit etre egal à 5 !!\n");
    }
    else {
        int cs = atoi(argv[1]);
        int bs = atoi(argv[2]);
        int asso = atoi(argv[3]);
        char* trace = argv[4];
        Cache C = initialiser(cs, asso);
        printf("Les donnees sont : %d %d %d %s", cs, bs, asso, trace);
        printf("%d", C[2047][0].valid);

    }
}
