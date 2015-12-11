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
    int nbrFailReading;
    int nbrFailWriting;
    int nbrSuppCache;
    int nbrCopyInMemoryAfterSuppCache;
}ModelCache;


ModelCache initializeCache (int cs, int asso, int bs) {
    ModelCache C;
    C.cs = cs;
    C.asso = asso;
    C.bs = bs;
    C.nbrFailReading = 0;
    C.nbrCopyInMemoryAfterSuppCache = 0;
    C.nbrFailWriting = 0;
    C.nbrSuppCache = 0;
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

int getNbCyclePerdu (int bs, int nbDefautLecture, int nbDefautEcriture, int nbLigneSupprDuCache)
{
	//Pénalité d’un défaut = (12 + bs/8) cycles
	//Cycles perdu par la mémoire =
	// nombre de cycles dues à la pénalité des défauts * (nombre défauts lecture + nombre défauts écriture + nombre de lignes supprimées du cache)
    return (12 + bs/8) * (nbDefautLecture + nbDefautEcriture + nbLigneSupprDuCache);
};

// Reading Gestion
void readData (int index, int tag, ModelCache *C) {
    int i;
    int valueOfValidIsZero = 1;
    int indexOfAssoToReplace = 0;
    //Loop for to determine in which case we are (+ save util data for the LRU)
    for (i = 0; i < C->asso; i++) {
        if (C->Cache[index][i].valid != 0) {
            valueOfValidIsZero = 0;
            //Find the index of the LRU
            if (C->Cache[index][indexOfAssoToReplace].compteur > C->Cache[index][i].compteur) {
                indexOfAssoToReplace = i;
            }
        }
        //Gérer cas tous 0, tous 1 et quelques 1 libres
    }
    if (valueOfValidIsZero) {
        C->nbrFailReading++;
        C->Cache[index][0].valid = 1;
        C->Cache[index][0].tag = tag;
        C->Cache[index][0].M = 0;
        C->Cache[index][0].compteur = 0;
    }
};

// Writing gestion
void writeData (int index, int tag, ModelCache *C) {

};

// Address analysis
void addressAnalysis (char car ,char *address, ModelCache *C) {
    int isWrite = 0;
    int addressBase10 = (int)strtol(address, NULL, 16);
    int numBloc = addressBase10 / C->bs;
    int nbrEntree = C->cs / (C->bs * C->asso);
    int index = numBloc % nbrEntree;
    int tag = numBloc / nbrEntree;

    // Check if it is a reading or a writing
    if (car == "W")  {
        isWrite = 1;
    }
    // If it is a writing
    if (isWrite) {
        writeData(index, tag, C);
    }
    else { //It is a reading
        readData(index, tag, C);
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
            addressAnalysis(car, adre, &C);
        }

    }
}
