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

typedef struct {
    int bs;
    int cs;
    int asso;
    bloc **Cache;
}ModelCache;


ModelCache initializeCache (int cs, int asso, int bs) {
    ModelCache C;
    C.cs = cs;
    C.asso = asso;
    C.bs = bs;
    int nbrElement = cs / (asso*bs);
    int i, j;
    C.Cache = malloc(nbrElement * sizeof (bloc));
    for (i = 0; i < nbrElement; i++) {
        C.Cache[i] = malloc(asso * sizeof(bloc));
    }
    for (i = 0; i < nbrElement; i++) {
        for (j = 0; j < asso; j++) {
            C.Cache[i][j].valid = 0;
        }
    }
    return C;
};

// Reading Gestion
void readData () {

};

// Writing gestion
void writeData () {

};

// Address analysis
void addressAnalysis (char car ,char *address, ModelCache C) {
    int isWrite = 0;
    int addressBase10 = (int)strtol(address, NULL, 16);
    int numBloc = addressBase10 / C.bs;
    int nbrEntree = C.cs / (C.bs * C.asso);
    int index = numBloc % nbrEntree;
    int tag = numBloc / nbrEntree;

    // Check if it is a reading or a writing
    if (car == "W")  {
        isWrite = 1;
    }
    // If it is a writing
    if (isWrite) {
        writeData();
    }
    else { //It is a reading
        readData();
    }
};

void main(int argc, char *argv[]) {
    //Test the number of arguments
    if (argc != 5) {
        printf("Le nombre d'arguments est invalide, il doit etre egal à 5 !!\n");
    }
    else {
        int cs = atoi(argv[1]);
        int bs = atoi(argv[2]);
        int asso = atoi(argv[3]);
        char* trace = argv[4];
        char car;
        char adre[8];
        ModelCache C = initializeCache(cs, asso, bs);
        printf("Les donnees sont : %d %d %d %s\n", cs, bs, asso, trace);
        FILE* tr = fopen(trace, "r");
        while(!feof(tr)) {
            fscanf (tr, "%c%s\n", &car, adre);
            addressAnalysis(car, adre, C);
        }

    }
}
