#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <values.h>
#include <math.h>
#include <float.h>

//para el calculo de tiempos
#include <sys/resource.h>
#include <time.h>
#include <sys/time.h>

#define alpha 0.4 		
#define VISITADO 1
#define ERROR -1
//#define Tam 2

int DIM;
int cant_Vectores;

void copiavalor(double *a, double *b);
double distancia(double *p1, double *p2);
double leedato(double *dato, FILE *file);
double distancia_max(double **DB);

int main(int argc, char const *argv[])
{

	if (argc != 4) {
		printf("Error :: Ejecutar como : a.out archivo_BD Num_elem DIM\n");
		return 0;
	}

	int*pivo_Id,centro;
	cant_Vectores = atoi(argv[2]);
	DIM = atoi(argv[3])+2;
	char str_f[256];
	FILE *archivo_BD;
	double dist_temp=0, distancia_maxima;
	double **Distancias, **DB,dato[DIM-2];
	srand(time(NULL));
	int cant_pivotes=0;

	int ind_pivote = DIM-1;
	int ind_estado = DIM-2;

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
		if (leedato(dato, archivo_BD) == ERROR || feof(archivo_BD)) {
			printf("\n\nERROR :: N_DB mal establecido\n\n");
			fflush(stdout);
			fclose(archivo_BD);
			break;
		}
		copiavalor(DB[i], dato);
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

	distancia_maxima=distancia_max(DB)*alpha;

	centro=rand()%cant_Vectores;
	centro = 3;
  	//Habilitar primer vector
	DB[centro][ind_pivote]=1;
	DB[centro][ind_estado]=VISITADO;
	cant_pivotes++;
	pivo_Id[0] = centro;

	printf("DISTANCIA MAX: %lf\n",distancia_maxima );

	for (i=0;i<cant_Vectores;i++){
		for (j = 0; j < cant_pivotes; ++j){
			dist_temp = distancia(DB[i], DB[pivo_Id[j]]);
			printf("dist_temp: %lf i: %d j: %d\n",dist_temp,i,j );
			if (DB[i][ind_pivote]!=1 && DB[i][ind_estado] != VISITADO && dist_temp <= distancia_maxima){
				DB[i][ind_estado] = VISITADO;
				break;
			}
		}

		if(DB[i][ind_estado] != VISITADO){
			DB[i][ind_pivote] = 1;
			pivo_Id = (int *) realloc(pivo_Id, (sizeof(int)+sizeof(pivo_Id)));
			cant_pivotes++;
			pivo_Id[cant_pivotes-1] = i;
		}
	}

	printf("cant_pivotes : %d\n",cant_pivotes);
	for (i = 0; i < cant_pivotes; ++i){
		printf("Pivote %d: %d\n",i+1, pivo_Id[i] );
	}
	fflush(stdout);

	Distancias = (double **) malloc(sizeof (double *)*cant_pivotes);
    for (i = 0; i < cant_pivotes; i++)
        Distancias[i] = (double *) malloc(sizeof (double)*cant_Vectores);

    for (i = 0; i < cant_pivotes; ++i){
    	for (j = 0; j < cant_Vectores; ++j){
    		Distancias[i][j]=distancia(DB[pivo_Id[i]],DB[j]);
    	}
    }

    for (i = 0; i < cant_pivotes; ++i){
    	for (j = 0; j < cant_Vectores; ++j){
    		printf("%lf ",Distancias[i][j]);
    	}
    	printf("\n");
    }

	return 0;
}

double distancia(double *p1, double *p2) {
	int i = 0;
	double suma = 0;

	for (i = 0; i < DIM-2; i++)
		suma += ((p1[i] - p2[i])*(p1[i] - p2[i]));
	return sqrt(suma);
}

double leedato(double *dato, FILE *file) {
	int i = 0;

	for (i = 0; i < DIM-2; i++)
		if (fscanf(file, "%lf", &(dato[i])) < 1)
			return ERROR;
		return 1;
}

void copiavalor(double *a, double *b) {
	int i;
	for (i = 0; i < DIM-2; i++)
		a[i] = b[i];
	return;
}

double distancia_max(double **DB) {
	int i,j = 0;
	double aux,Dist_Eucl = 0;
	for (i = 0; i < cant_Vectores; ++i){
		for (j = i+1; j < cant_Vectores; ++j){
			Dist_Eucl=distancia(DB[i],DB[j]);
			if(Dist_Eucl>=aux){
				aux=Dist_Eucl;
			}
			Dist_Eucl=0;
		}
	}
	return aux;
}