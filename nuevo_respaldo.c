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
//#define Tam 2

void copiavalor(double *a, double *b, int DIM);
__attribute__((target(mic))) double distancia(double *p1, double *p2, int DIM);
double leedato(double *dato, FILE *file, int DIM);
__attribute__((target(mic))) double distancia_max(double *DB, int DIM, int cant_Vectores);
void matrixToVector(double **matrix, int num_cols, int num_rows, double *vector);

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
	double *Distancias, **DB,dato[DIM-2], *DB_vector;
	srand(time(NULL));
	int num_threads, thread_num;

	int ind_pivote = DIM-1;
	int ind_estado = DIM-2;
	int cant_pivotes=1;

	//Indice de loops
	int i=0,j=0,k=0;
	pivo_Id=(int*)malloc(sizeof(int));


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
		printf("\n");	
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

	printf("%d\n",cant_Vectores);
	printf("%d\n",DIM );

	for (i = 0; i < cant_Vectores; ++i){
		for (j = 0; j < DIM-2; ++j){
			printf("%lf ", DB[i][j] );
		}
		printf("\n");	
	}


	centro=rand()%cant_Vectores;
	centro = 3;
  	//Habilitar primer vector
	DB[centro][ind_pivote]=1;
	DB[centro][ind_estado]=VISITADO;
	pivo_Id[0] = centro;

	DB_vector = (double *)_mm_malloc(sizeof(double)*DIM*cant_Vectores, 64);
	matrixToVector(DB, DIM, cant_Vectores, DB_vector);

	#pragma offload target(mic:0) in(centro) in(ind_pivote) inout(pivo_Id:length(cant_pivotes)) in(cant_Vectores) in(DIM) in(ind_estado) in(DB_vector:length(DIM*cant_Vectores))
	{
    	#pragma omp parallel private(i, j, thread_num) shared(DB_vector, DIM, num_threads)
		{

			#pragma omp master
			{
				num_threads = omp_get_num_threads();
				printf("run with %d threads\n", num_threads);
			}
         	#pragma omp barrier
			
			thread_num = omp_get_thread_num();
			double dist_temp,distancia_maxima;

			distancia_maxima=distancia_max(DB_vector,DIM,cant_Vectores)*alpha;
			printf("DISTANCIA MAX: %lf\n",distancia_maxima );

			for (i=0;i<cant_Vectores;i++){
				for(j=thread_num; j<cant_pivotes; j+=num_threads){
					dist_temp = distancia(&DB_vector[i], &DB_vector[pivo_Id[j]],DIM);
					printf("dist_temp: %lf i: %d j: %d\n",dist_temp,i,j );
					if (DB_vector[(i*DIM) + ind_pivote]!=1 && DB_vector[(i*DIM)+ind_estado] != VISITADO && dist_temp <= distancia_maxima){
						DB_vector[(i*DIM)+ind_estado] = VISITADO;
						break;
					}
				}

				if(DB_vector[(i*DIM )+ind_estado] != VISITADO){
					DB_vector[(i*DIM)+ind_pivote] = 1;
					//pivo_Id = (int *) realloc(pivo_Id, (sizeof(int)+sizeof(pivo_Id)));
					cant_pivotes++;
					//pivo_Id[cant_pivotes-1] = i;
				}
			}
			printf("cant_pivotes : %d\n",cant_pivotes);

			
			//for (i = 0; i < cant_pivotes; ++i){
			//	printf("Pivote %d: %d\n",i+1, pivo_Id[i] );
			//}
			//fflush(stdout);
		}
	}

	//Distancias = (double *) malloc(sizeof (double)*(cant_pivotes*cant_Vectores));
 

    //for (i = 0; i < cant_pivotes; ++i){
    // 	for (j = 0; j < cant_Vectores; ++j){
    //		Distancias[j+(i*cant_Vectores)]=distancia(DB[pivo_Id[i]],DB[j]);
    //	}
    // }

    //for (i = 0; i < cant_pivotes; ++i){
    //	for (j = 0; j < cant_Vectores; ++j){
    //		printf("%lf ",Distancias[j+(i*cant_Vectores)]);
    //	}
    // 	printf("\n");
    //}

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
	double aux,Dist_Eucl = 0;
	for (i = 0; i < cant_Vectores; ++i){
		for (j = i+1; j < cant_Vectores; ++j){
			Dist_Eucl=distancia(&(DB[i]), &(DB[j*DIM]), DIM);
			if(Dist_Eucl>=aux){
				aux=Dist_Eucl;
			}
			Dist_Eucl=0;
		}
	}
	return aux;
}

void matrixToVector(double **matrix, int num_cols, int num_rows, double *vector)
{
    int i,j;
    for(i=0; i<num_rows; i++)
        for(j=0; j<num_cols; j++)
            vector[(i*num_cols)+j] = matrix[i][j];
}