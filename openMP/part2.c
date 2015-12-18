#include <stdio.h>
#include <omp.h>
#define NUMSTEPS 10
#define NUM_THREADS 2

int main (int argc, const char * argv[])
{
    static long num_steps = 10000000000;
    int i, nthreads;
    double pi;
    double step;
    double x;
    double sum = 0.0;
    step =(1.0/ (double) num_steps);
    /******************** SERIE *********/
    //x = 0.5 * step;
    omp_set_num_threads(4);

    double wtime;

    wtime = omp_get_wtime ();

    #pragma omp parallel for reduction (+:sum)	private (x)
    for (i=0; i<= num_steps; i++)
    {
        if (i == 0) printf("\n Nombre de proc=%d, Nombre de threads=%d\n", omp_get_num_procs(), omp_get_num_threads());
        x = (double) ((double)i + 0.5) *  step;
        sum += 4.0/(1.0+x*x);
        //printf("\n Thread=%d, i=%d, x=%f, sum=%f", omp_get_thread_num(), i, x, sum);
    }
    pi = step * sum;
    wtime=omp_get_wtime()-wtime;
    printf ( " Elapsed wall clock time = %f with %ld steps\n",
             wtime, num_steps );



    printf(" \n Calcul Serie Pi = %f\n", pi);

}
