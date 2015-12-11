/*
    TP mémoire cache - Jordane QUINCY et Jean-Baptiste DURIEZ
    Etat : Travail en cours
    Code : 'D' + 'Q' = 68 + 81 = 149, Gestion Ecriture = 149%2 = 1 = WB, Remplacement des blocs = 149%4 = 1 = LRU
*/

#include <stdlib.h>
#include <stdio.h>


typedef struct s {
    int valid;
    int tag;
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
    int nbrHitReading;
    int nbrHitWriting;
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
    C.nbrHitReading = 0;
    C.nbrHitWriting = 0;
    int nbrElement = cs / (asso*bs);
    int i, j;
    C.Cache = malloc(nbrElement * sizeof (bloc));
    for (i = 0; i < nbrElement; i++) {
        C.Cache[i] = malloc(asso * sizeof(bloc));
    }
    for (i = 0; i < nbrElement; i++) {
        for (j = 0; j < asso; j++) {
            C.Cache[i][j].valid = 0;
            C.Cache[i][j].tag = -1;
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
void addressTreatment (int index, int tag, ModelCache *C, int isWrite) {
    int i;
    //allZero vaut 1 si tout les bits valid à chaque bloc de l'index vaut 0, allZero vaut 0 sinon
    int allZero = 1;
    //indexToReplaceLRU va prendre la valeur de l'index(j) à remplacer si tous les bits valid valent 1 et qu'aucun tag ne correspond (il n'est donc pas toujours utilisé)
    int indexToReplaceLRU = 0;
    //indexToReplace va prendre la valeur de l'index(j) à remplacer si les tags ne correspond pas et si il y a au moins un bit valid à 0 (dans ce cas on utilise pas le LRU)
    int indexToReplace = -1;
    //tagFounded vaut 0 si on ne trouve pas le tag déjà présent, 1 sinon
    int tagFounded = 0;
    //indexTagFounded prend la valeur de l'index(j) si un bloc a le même tag que celui à lire (afin d'incrémenter le compteur pour le LRU)
    int indexTagFounded = -1;
    //Loop for to determine in which case we are (+ save util data for the LRU)
    for (i = 0; i < C->asso; i++) {
        if (C->Cache[index][i].valid == 0) {
                indexToReplace = i;
        }
        else {
            //There is one valid bit set to 1
            allZero = 0;
            //Find the index of the LRU
            if (C->Cache[index][indexToReplaceLRU].compteur > C->Cache[index][i].compteur) {
                indexToReplaceLRU = i;
            }
            //Check if the tag is the same
            if (C->Cache[index][i].tag == tag) {
                tagFounded = 1;
                indexTagFounded = i;
            }
        }
    }
    //Manage the cache memory
    //Case when all valid bit are 0
    if (allZero) {
        C->nbrFailReading++;
        C->Cache[index][indexToReplace].valid = 1;
        C->Cache[index][indexToReplace].tag = tag;
        C->Cache[index][indexToReplace].M = 0;
        C->Cache[index][indexToReplace].compteur = 0;
    }
    else {
        if (tagFounded) { //It is a hit !
            if (isWrite) {
                C->nbrHitWriting++;
            }
            else {
                C->nbrHitReading++;
            }
            C->Cache[index][indexTagFounded].compteur++;
            if (isWrite) {
                C->Cache[index][indexTagFounded].M = 1;
            }
        }
        else if (indexToReplace >= 0) { //There is at least one valid bit sets to 0
            if (isWrite) {
                C->Cache[index][indexTagFounded].M = 1;
                C->nbrFailWriting++;
            }
            else {
                C->nbrFailReading++;
            }
            C->Cache[index][indexToReplace].valid = 1;
            C->Cache[index][indexToReplace].tag = tag;
            C->Cache[index][indexToReplace].M = 0;
            C->Cache[index][indexToReplace].compteur = 0;

        }
        else { //All valid bits are 1 and it is a fail so use LRU and replace one bloc
            C->nbrSuppCache++;
            if (isWrite) {
                C->Cache[index][indexTagFounded].M = 1;
                C->nbrFailWriting++;
            }
            else {
                C->nbrFailReading++;
            }
            if (C->Cache[index][indexToReplaceLRU].M) {
                C->nbrCopyInMemoryAfterSuppCache++;
            }
            C->Cache[index][indexToReplaceLRU].valid = 1;
            C->Cache[index][indexToReplaceLRU].tag = tag;
            C->Cache[index][indexToReplaceLRU].M = 0;
            C->Cache[index][indexToReplaceLRU].compteur = 0;

        }
    }
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
    addressTreatment(index, tag, C, isWrite);
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
        printf("nbr Fail : %d", C.nbrFailReading+C.nbrFailWriting);

    }
}
