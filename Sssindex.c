#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <values.h>
#include <math.h>
#include <omp.h>

//para el calculo de tiempos
#include <sys/resource.h>
#include <time.h>
#include <sys/time.h>

#define alpha 0.4 		
#define VISITADO 1
#define ERROR -1
#define THREAD 120
//#define Tam 2

void copiavalor(double *a, double *b, int DIM);
__attribute__((target(mic))) double distancia(double *p1, double *p2, int DIM);
double leedato(double *dato, FILE *file, int DIM);
__attribute__((target(mic))) double distancia_max(double *DB, int DIM, int cant_Vectores);
void matrixToVector(double **matrix, int num_cols, int num_rows, double *vector);
__attribute__((target(mic))) double realloc_(int *pivo_Id, int tam);

int main(int argc, char const *argv[])
{

	if (argc != 4) {
		printf("Error :: Ejecutar como : a.out archivo_BD Num_elem DIM\n");
		return 0;
	}
	int DIM;
	int cant_Vectores;
	int*pivo_Id,centro;
	cant_Vectores = atoi(argv[2]);
	DIM = atoi(argv[3])+2;
	char str_f[256];
	FILE *archivo_BD;
	double *Distancias, **DB,dato[DIM-2], *DB_vector,dist_temp[THREAD];
	srand(time(NULL));
	int num_threads, thread_num;
	//Indice de loops
	int i=0,j=0,k=0;

	for (i = 0; i < THREAD; ++i)
		dist_temp[i]=0;

	int ind_pivote = DIM-1;
	int ind_estado = DIM-2;
	int cant_pivotes=1;


	sprintf(str_f, "%s", argv[1]);
	printf("\nAbriendo %s... ", argv[1]);
	fflush(stdout);
	archivo_BD = fopen(str_f, "r");
	printf("OK\n");
	fflush(stdout);

	DB = (double **) malloc(sizeof (double *)*cant_Vectores);
	for (i = 0; i < cant_Vectores; i++)
		DB[i] = (double *) malloc(sizeof (double)*DIM);

	for (i = 0; i < cant_Vectores; ++i){
		for (j = DIM-2; j < DIM; ++j){
			DB[i][j]==-1;
		}
	}

	printf("\nCargando DB... ");
	fflush(stdout);
	for (i = 0; i < cant_Vectores; i++) {
		if (leedato(dato, archivo_BD,DIM) == ERROR || feof(archivo_BD)) {
			printf("\n\nERROR :: N_DB mal establecido\n\n");
			fflush(stdout);
			fclose(archivo_BD);
			break;
		}
		copiavalor(DB[i], dato,DIM);
	}
	fclose(archivo_BD);
	printf("OK\n");
	fflush(stdout);



	centro=rand()%cant_Vectores;
	centro = 3;
  	//Habilitar primer vector
	DB[centro][ind_pivote]=1;
	DB[centro][ind_estado]=VISITADO;
	pivo_Id[0] = centro;

	DB_vector = (double *)_mm_malloc(sizeof(double)*DIM*cant_Vectores, 64);
	pivo_Id = (int *)_mm_malloc(sizeof(int)*cant_pivotes, 64);
	matrixToVector(DB, DIM, cant_Vectores, DB_vector);

	#pragma offload target(mic:0) in(centro) in(ind_pivote) in(dist_temp) in(pivo_Id:length(cant_pivotes)) in(cant_Vectores) in(DIM) in(ind_estado) in(DB_vector:length(DIM*cant_Vectores))
	{
    	#pragma omp parallel private(i, j, thread_num) shared(DB_vector, DIM, num_threads)
		{

			#pragma omp master
			{
				num_threads = omp_get_num_threads();
				printf("run with %d threads\n", num_threads);
				printf("cant_Vectores: %d\n",cant_Vectores );
				fflush(0);
			}
         	#pragma omp barrier

			thread_num = omp_get_thread_num();

			for(i=thread_num*DIM; i<cant_Vectores*DIM; i+=num_threads*DIM){
				for(j=i+DIM; j<cant_Vectores*DIM; j+=DIM){
					double temp = distancia(&(DB_vector[i]), &(DB_vector[j]), DIM);
					if(temp > dist_temp[thread_num])
						dist_temp[thread_num] = temp;
					printf("Distancias: %lf\n",dist_temp[thread_num]);
					fflush(0);
				}
			}
			#pragma omp barrier
			#pragma omp master
			{
				double distancia_M=0;
				int i=0;
				for (i = 0; i < THREAD; ++i){
					if (dist_temp[i]>distancia_M)
						distancia_M=dist_temp[i];
				}
				distancia_M*=alpha;
				printf("Distancia M: %lf\n",distancia_M);
				fflush(0);
			}







		}
	}

	


	return 0;
}

__attribute__((target(mic))) double distancia(double *p1, double *p2, int DIM){
    int i=0;
    double suma=0.0;
   __assume_aligned(p1, 64);
   __assume_aligned(p2, 64);   
    #pragma vector aligned
    #pragma ivdep
    #pragma simd
    for (i=0; i < DIM-2; i++){
    	printf("");
        suma += (p1[i]-p2[i])*(p1[i]-p2[i]);
    }
    return sqrt(suma);
}


double leedato(double *dato, FILE *file,int DIM) {
	int i = 0;

	for (i = 0; i < DIM-2; i++)
		if (fscanf(file, "%lf", &(dato[i])) < 1)
			return ERROR;
		return 1;
}

void copiavalor(double *a, double *b,int DIM) {
	int i;
	for (i = 0; i < DIM-2; i++)
		a[i] = b[i];
	return;
}

__attribute__((target(mic))) double distancia_max(double *DB, int DIM, int cant_Vectores) {
	int i,j = 0;
	int thread_num = omp_get_thread_num();
	int num_threads = omp_get_num_threads();
	double aux[num_threads],Dist_Eucl = 0;


	for (i=thread_num; i<cant_Vectores; i+=num_threads){
		for (j = i+1; j < cant_Vectores; ++j){
			Dist_Eucl=distancia(&(DB[i*DIM]), &(DB[j*DIM]), DIM);
			Dist_Eucl=0;
		}
	}
	return Dist_Eucl;
}

void matrixToVector(double **matrix, int num_cols, int num_rows, double *vector){
    int i,j;
    for(i=0; i<num_rows; i++)
        for(j=0; j<num_cols; j++)
            vector[(i*num_cols)+j] = matrix[i][j];
}

__attribute__((target(mic))) double realloc_(int *pivo_Id, int tam) {
	int i;
	int *aux;
	aux = (int *)_mm_malloc(sizeof(int)*tam, 64);

	for (i = 0; i < tam-1; ++i)
		aux[i]=pivo_Id[i];
	free(pivo_Id);

	return *aux;
}