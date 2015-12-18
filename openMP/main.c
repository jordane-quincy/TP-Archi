#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#define N 10



typedef float **Matrice;

Matrice initialiser(int n) {
    Matrice M;
    int i, j;
    M = malloc(N * sizeof(float));
    for (i = 0; i < n; i++) {
        M[i] = malloc (N* sizeof (float));
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
	int nbTh = 4;
    int t, i, j, k;
    t = 2;
	//ajouter un for ici pour faire varier entre 1 et 4 cores
        omp_set_dynamic(0);
        omp_set_num_threads(2);
        t_debut = omp_get_wtime();
        printf("nbr de thread : %d\n", omp_get_num_threads());
        #pragma omp parallel for
        for(i=0; i<N; i++){
                printf("test\n");
            for(j=0; j<N; j++){
                C[i][j] = 0;
                for(k=0; k<N; k++){
                    C[i][j] += A[i][k] * B[k][j];
                    t_fin = omp_get_wtime();
                    t_exec = t_fin - t_debut;
                    //printf("%d %lf\n", t,t_exec);
                }
            }
        }
    printf("\n");
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            printf("%.2f    ", C[i][j]);
        }
        printf("\n");
    }
    return 0;
};
