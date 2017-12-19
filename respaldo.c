#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#define alpha 0.4 				//alpha
#define Tam 2
#define VISITADO 1

typedef struct
{
	double Coordenada[Tam];	//Contiene las coodrdenadas del vetor 
	int Id_Vector;         	// Es el identificador del vector
	int Estado;          		 
	int pivote;  			 				 
}Pivote;

double max_M(Pivote *Pivotes, int cant_Vectores);	
double dist_Euclidiana(Pivote *Pivotes, int pivote, int indice);

int main(int argc, char const *argv[])
{
	int cant_Vectores, Centro,*pivo_Id;
	double dist_temp=0, **Distancias;
	scanf("%d",&cant_Vectores);
  	srand(time(NULL)); /* Toma un número a partir de la fecha y hora actual. Varía en cada centésima de segundo. La semilla se toma del reloj del sistema. */
	Pivote *Pivotes;
	int cant_pivotes=0;

	//Indice de loops
	int i=0,j=0,k=0;
	pivo_Id=(int*)malloc(sizeof(int));

	for (i = 0; i < cant_Vectores; ++i){
		for (j = 0; j < Tam; ++j){
			scanf("%lf",&Pivotes[i].Coordenada[j]);
		}
		Pivotes[i].Id_Vector=i;
		Pivotes[i].Estado=-1;
		Pivotes[i].pivote=0;
	}
	
	float distancia_maxima = max_M(Pivotes,cant_Vectores);
	distancia_maxima=distancia_maxima*alpha;
	
	Centro=rand()%cant_Vectores;
	
  //Habilitar primer vector
	Pivotes[Centro].pivote=1;
	Pivotes[Centro].Estado=VISITADO;
	cant_pivotes++;
	pivo_Id[0] = Centro;



	for (i=0;i<cant_Vectores;i++){

		for (j = 0; j < cant_pivotes; ++j){
			dist_temp = dist_Euclidiana(Pivotes, pivo_Id[j], i);
			if (Pivotes[i].pivote!=1 && Pivotes[i].Estado != VISITADO && dist_temp <= distancia_maxima){
				Pivotes[i].Estado = VISITADO;
				break;
			}
		}

		if(Pivotes[i].Estado != VISITADO){
			Pivotes[i].pivote = 1;
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



	return 0;
}

double max_M(Pivote *Pivotes, int cant_Vectores){
	double Dist_Eucl=0,aux=0;
	int i,j,k;
	for (i = 0; i < cant_Vectores; ++i){
		for (j = i+1; j < cant_Vectores; ++j){
			for (k = 0; k < Tam; ++k){
				Dist_Eucl = Dist_Eucl + (pow(Pivotes[i].Coordenada[k]-Pivotes[j].Coordenada[k],2));
			}
			if(sqrt(Dist_Eucl)>=aux){
				aux=sqrt(Dist_Eucl);
			}
			Dist_Eucl=0;
		}
	}

	return aux;
}

double dist_Euclidiana(Pivote *Pivotes, int pivote, int indice){
	double Dist_Eucl=0;
	int k;
	for (k = 0; k < Tam; ++k){
		Dist_Eucl = Dist_Eucl + (pow(Pivotes[indice].Coordenada[k]-Pivotes[pivote].Coordenada[k],2));
	}
	return Dist_Eucl;
}