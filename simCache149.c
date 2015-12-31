/*
    TP mémoire cache - Jordane QUINCY et Jean-Baptiste DURIEZ
    Etat : Travail en cours
    Code : 'D' + 'Q' = 68 + 81 = 149, Gestion Ecriture = 149%2 = 1 = WB, Remplacement des blocs = 149%4 = 1 = LRU
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
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
    long nbrFailReading;
    long nbrFailWriting;
    long nbrSuppCache;
    long nbrCopyInMemoryAfterSuppCache;
    long nbrHitReading;
    long nbrHitWriting;
    long nbrOfReading;
    long nbrOfWriting;
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
    C.nbrOfReading = 0;
    C.nbrOfWriting = 0;
    int nbrElement = cs / (asso*bs);
    int i, j;
    C.Cache = malloc(nbrElement * sizeof (bloc));
    for (i = 0; i < nbrElement; i++) {
        C.Cache[i] = malloc(asso * sizeof(bloc));
    }
    for (i = 0; i < nbrElement; i++) {
        for (j = 0; j < asso; j++) {
            C.Cache[i][j].valid = 0;
            C.Cache[i][j].tag = (double)-1;
        }
    }
    return C;
};

long getNbCyclePerdu (int bs, long nbDefautLecture, long nbDefautEcriture, long nbLigneSupprDuCache)
{
	//Pénalité d’un défaut = (12 + bs/8) cycles
	//Cycles perdu par la mémoire =
	// nombre de cycles dues à la pénalité des défauts * (nombre défauts lecture + nombre défauts écriture + nombre de lignes supprimées du cache)
    return (12 + bs/8) * (nbDefautLecture + nbDefautEcriture + nbLigneSupprDuCache);
};

// Reading Gestion
void addressTreatment (int index, double tag, ModelCache *C, int isWrite) {
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
    if (isWrite) {
        C->nbrOfWriting++;
    }
    else {
        C->nbrOfReading++;
    }
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
        C->Cache[index][indexToReplace].valid = 1;
        C->Cache[index][indexToReplace].tag = tag;
        C->Cache[index][indexToReplace].M = 0;
        C->Cache[index][indexToReplace].compteur = 0;
        if (isWrite) {
            C->nbrFailWriting++;
            C->Cache[index][indexToReplace].M = 1;
        }
        else {
            C->nbrFailReading++;
        }
    }
    else {
        if (tagFounded) { //It is a hit !
            C->Cache[index][indexTagFounded].compteur++;
            if (isWrite) {
                C->nbrHitWriting++;
                C->Cache[index][indexTagFounded].M = 1;
            }
            else {
                C->nbrHitReading++;
            }
        }
        else if (indexToReplace >= 0) { //There is at least one valid bit sets to 0
            C->Cache[index][indexToReplace].valid = 1;
            C->Cache[index][indexToReplace].tag = tag;
            C->Cache[index][indexToReplace].M = 0;
            C->Cache[index][indexToReplace].compteur = 0;
            if (isWrite) {
                C->Cache[index][indexToReplace].M = 1;
                C->nbrFailWriting++;
            }
            else {
                C->nbrFailReading++;
            }
        }
        else { //All valid bits are 1 and it is a fail so use LRU and replace one bloc
            C->nbrSuppCache++;
            if (C->Cache[index][indexToReplaceLRU].M) {
                C->nbrCopyInMemoryAfterSuppCache++;
            }
            C->Cache[index][indexToReplaceLRU].valid = 1;
            C->Cache[index][indexToReplaceLRU].tag = tag;
            C->Cache[index][indexToReplaceLRU].M = 0;
            C->Cache[index][indexToReplaceLRU].compteur = 0;
            if (isWrite) {
                C->Cache[index][indexToReplaceLRU].M = 1;
                C->nbrFailWriting++;
            }
            else {
                C->nbrFailReading++;
            }
        }
    }
};



// Address analysis
void addressAnalysis (char car ,long address, ModelCache *C) {
    int isWrite = 0;
    long addressBase10 = address;
    long numBloc = addressBase10 / C->bs;
    int nbrEntree = C->cs / (C->bs * C->asso);
    int index = numBloc % nbrEntree;
    double tag = (double)numBloc / nbrEntree;
    // Check if it is a reading or a writing
    if (car == 'W')  {
        isWrite = 1;
    }
    addressTreatment(index, tag, C, isWrite);
};

double calculTempsExec(ModelCache *C){
    printf("%d\n", C->asso);
    printf("log : %lf\n", log2(C->asso));
    double nbrDeHit = (C->nbrHitReading + C->nbrHitWriting) * 40/100;
    double nbrDeMiss = (C->nbrFailReading + C->nbrFailWriting) * 40/100;
    printf("le calcul : %lf * (10 + log2(%d)) + %lf * 20 * (10 + log2(%d))\n", nbrDeHit, C->asso, nbrDeMiss, C->asso);
    return (nbrDeHit * (10 + log2(C->asso))) + (nbrDeMiss * 20 * (10 + log2(C->asso)));
}

void main(int argc, char *argv[]) {
    //Ceci fait tout crasher
    int i = 0;
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
        long adre;
        ModelCache C = initializeCache(cs, asso, bs);
        printf("Les donnees sont :\nTaille de la memoire cache : %d octets\nTaille d'un bloc : %d octets\nDegre d'associativite : %d\nNom du fichier analyse : %s\n", cs, bs, asso, trace);
        FILE* tr = fopen(trace, "r");
        while(!feof(tr)) {
            fscanf(tr, "%c%X\n", &car, &adre);
            addressAnalysis(car, adre, &C);
        }
        fclose(tr);
        printf("\nResultats apres l'analyse du fichier d'addresses :\n");
        printf("Nombre de lectures : %ld\nNbr d'ecritures %ld", C.nbrOfReading, C.nbrOfWriting);
        printf("\nnbr Fail : %ld\n", C.nbrFailReading+C.nbrFailWriting);
        printf("nbr succes : %ld\n", C.nbrHitReading + C.nbrHitWriting);
        printf("Compy in memory : %ld \n", C.nbrCopyInMemoryAfterSuppCache);
        printf("Nbr de supp : %ld \n", C.nbrSuppCache);
        printf("Valeur cycle perdu %ld\n", getNbCyclePerdu(bs, C.nbrFailReading, C.nbrFailWriting, C.nbrCopyInMemoryAfterSuppCache));
        //nbr d'instruction SW et LW
        double nbrInstruction = (C.nbrFailReading + C.nbrFailWriting + C.nbrHitReading + C.nbrHitWriting) *40/100;
        double tempsExecInfini = nbrInstruction * 10;
        printf("temps exec taille infinie : %lf ns\n", tempsExecInfini);
        printf("tmps exec : %lf\n", calculTempsExec(&C));


    }
}
