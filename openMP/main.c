#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#define N 1000

typedef float **Matrice;

Matrice initialiser(int n) {
    Matrice M;
    int i, j;
    M = malloc(N * sizeof (float));
    for (i = 0; i < n; i++) {
        M[i] = malloc (N* sizeof(float));
        for (j = 0; j < n; j++) {
            M[i][j] = 2;
        }
    }
    return M;
};

int main()
{
	Matrice A,B,C;
	double t_debut,t_fin,t_exec;
	A = initialiser(N);
	B = initialiser(N);
	C = initialiser(N);
	int nbTh = 2;
	int tailleMatrice = N;
    int i, t, j, k;
	//ajouter un for ici pour faire varier entre 1 et 4 cores
    t_debut = omp_get_wtime();
    for(t = 1; t <= nbTh; t++){
        omp_set_num_threads(t);
        t_debut = omp_get_wtime();
        #pragma omp parallel for private(j, k)
        for(i = 0; i < tailleMatrice; i++){
            for(j = 0; j < tailleMatrice; j++){
                C[i][j] = 0;
                for(k = 0; k < tailleMatrice; k++){
                    C[i][j] = C[i][j] + (A[i][k] * B[k][j]);
                }
            }
        }
        t_fin = omp_get_wtime();
        t_exec = t_fin - t_debut;
        printf("%d\t%lf\n", t, t_exec);
    }
    return 0;
};
