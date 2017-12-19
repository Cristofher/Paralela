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
	double dist_temp=0,*dist_pivo;

	scanf("%d",&cant_Vectores);
  	srand(time(NULL)); /* Toma un número a partir de la fecha y hora actual. Varía en cada centésima de segundo. La semilla se toma del reloj del sistema. */
	Pivote *Pivotes;
	int cant_pivotes=0;

	//Indice de loops
	int i=0,j=0;
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
	printf("%lf\n",distancia_maxima );
	distancia_maxima=distancia_maxima*alpha;
	
	Centro=rand()%cant_Vectores;
	Centro = 3; //BORRAR
	
  //Habilitar primer vector
	Pivotes[Centro].pivote=1;
	cant_pivotes++;
	pivo_Id[0] = Centro;
	dist_pivo[0] = 0;
	printf("dist_pivo[0]= %lf\n",dist_pivo[0]);


	for (i=0;i<cant_Vectores;i++){
		dist_pivo=(double*)malloc(sizeof(double)*cant_pivotes);
		dist_pivo[0]=1;
		printf("%lf\n",dist_pivo[0] );

		for (j = 0; j < cant_pivotes; ++j){
			//dist_pivo[j] = dist_Euclidiana(Pivotes, pivo_Id[j], i);
			printf("%lf\n",dist_Euclidiana(Pivotes, pivo_Id[j], i));
			printf("\n");
			printf("%lf %lf\n",Pivotes[i].Coordenada[0],Pivotes[i].Coordenada[1] );
			printf("%lf %lf\n",Pivotes[pivo_Id[j]].Coordenada[0],Pivotes[pivo_Id[j]].Coordenada[1] );
			printf("dist_Euclidiana %lf\n",dist_pivo[j]);
			printf("distancia_maxima * alpha %lf\n",distancia_maxima);
			if (Pivotes[i].pivote!=1 && Pivotes[i].Estado != VISITADO && dist_pivo[j] <= distancia_maxima){
				Pivotes[i].Estado = VISITADO;
				printf("marco VISITADO\n");
			}else{
				printf("marco pivote %d\n",i);
				Pivotes[i].pivote = 1;
				pivo_Id = (int *) realloc(pivo_Id, (sizeof(int)+sizeof(pivo_Id)));
				cant_pivotes++;
				pivo_Id[cant_pivotes-1] = i;
				dist_pivo[cant_pivotes-1] = 0;
				break;
			}
		}
	}

	printf("Salio\n");



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