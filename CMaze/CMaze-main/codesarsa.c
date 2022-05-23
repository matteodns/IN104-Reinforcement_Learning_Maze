#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mazeEnv.h"
#include "functions.h"


int action_max_Q(float** Q, int s){     //retourne l'action max

	int action_max=0;
	float Q_max = Q[s][0];

	for (int i=0; i<number_actions; i++){ 
		if (Q[s][i]>Q_max) {
			Q_max=Q[s][i];
			action_max=i;
		}
	}

	if (Q_max==0){
		return 4;
	}

	return action_max;
}

int action_max_Q_rand(float** Q, int s){          //retourne l'action max, et si elles sont toutes équivalentes, une totalement au hasard

	int a = rand()%4;
	int action_max=a;
	float Q_max = Q[s][a];

	for (int i=0; i<number_actions; i++){ 
		if (Q[s][i]>Q_max) {
			Q_max=Q[s][i];
			action_max=i;
		}
	}

	return action_max;
}



int e_greedy(float eps, float** Q, int s) {     //renvoie une action en fct de Q
	
	action direction;

	if(rand()%101 < eps*100) {        // au hasard

		direction=rand()%number_actions;

	}

	direction=action_max_Q_rand(Q,s);

	return direction;   //sinon on choisit l'action qui maximise Q

}


void Q_render(float** Q){

	for (int i=0; i<rows; i++){
		for (int j=0; j<cols; j++){
			if (action_max_Q(Q,i*cols+j)==0){
				printf("H ");
			} else if (action_max_Q(Q,i*cols+j)==1){
				printf("B ");
			} else if (action_max_Q(Q,i*cols+j)==2){
				printf("G ");
			} else if (action_max_Q(Q,i*cols+j)==3){
				printf("D ");
			} else {
				printf("  ");
			}

		}
		printf("\n");
	}

}


void Q_learning(float** Q, int n_ep_max, float alpha, float gamma, float eps) {

	envOutput episode;
	action direction;
	action direction_n;
	int s;
	int k;
	int r;
	int new_s;
	int wall1;
	int wall2;

	/*long start_time;
	long wait_time;
	long end_time;*/

	for (int n_ep=1; n_ep<=n_ep_max; n_ep++){   //pour chaque episode (<nb_ep_max)

		printf("Episode n°%d\n", n_ep);


		//Initialisation de l'épisode

		state_row=start_row;
		state_col=start_col;
		s = state_row*cols+state_col;
		episode.reward=0;
		episode.done=0;


		k=1;

		wall1=1;

		while(wall1==1){                       //Cette boucle permet de ne pas choisir des actions qui foncent dans un mur (plus radical que reward=-1)

			int i = state_row;
			int j = state_col;

			direction = e_greedy(eps, Q, s);

		    if (direction==up){
		       i = max(0,i-1);
		    }else if (direction==down){
		       i = min(rows-1,i +1);
		    }else if (direction==right){
		       j = min(cols-1,j +1);
		    }else if (direction==left){
		       j = max(0,j -1);
		    }

			if (visited[i][j]!=wall){
				wall1=0;
			}

		}	

		while (episode.done==0) {    //Tant que s n'est pas terminal

			printf("Episode n°%d, étape n°%d\n",n_ep,k);

			printf("Direction : %d\n",direction);

			episode = maze_step(direction);

			r=episode.reward;	
			new_s=episode.new_row*cols+episode.new_col;

			wall2=1;

			/*start_time=clock();
			wait_time=1000;
			end_time=start_time+wait_time;*/

			while(wall2==1){                       //Cette boucle permet de ne pas choisir des actions qui foncent dans un mur (plus radical que reward=-1)

				int i = state_row;
				int j = state_col;

				direction_n = e_greedy(eps, Q, new_s);

			    if (direction_n==up){
			       i = max(0,i-1);
			    }else if (direction_n==down){
			       i = min(rows-1,i+1);
			    }else if (direction_n==right){
			       j = min(cols-1,j+1);
			    }else if (direction_n==left){
			       j = max(0,j-1);
			    }

				if (visited[i][j]!=wall /*|| visited[i][j]==goal || clock()>end_time*/){
					wall2=0;
				}

				//printf("%d",direction_n);

			}

			Q[s][direction] = Q[s][direction] + alpha * (r + gamma*Q[new_s][direction_n] - Q[s][direction]);
			
			s=new_s;
			direction=direction_n;
			
			Q_render(Q);

			printf("ligne:%d\ncolonne:%d\nrecompense:%d\nfini:%d\n\n",episode.new_row,episode.new_col,episode.reward,episode.done);

			k++;

		}

		printf("Fin de l'épisode n°%d\n", n_ep);
		Q_render(Q);

	}
}

float** alloc_Q_1(int size){
	float** Q=calloc(size,sizeof(int*));
	for (int i=0; i<size; i++){
		Q[i]=calloc(number_actions,sizeof(int));
		if(Q[i]==NULL){
			printf("Erreur d'allocation de Q[i]");
			exit (-1);
		}
	}
	if(Q==NULL){
		printf("Erreur d'allocation de Q");
		exit (-1);
	}

	return Q;
}


float** alloc_Q_2(int size){
	float** Q=malloc(size*sizeof(int*));
	for (int i=0; i<size; i++){
		Q[i]=malloc(number_actions*sizeof(int));
		if(Q[i]==NULL){
			printf("Erreur d'allocation de Q[i]");
			exit (-1);
		}
		for (int j=0; j<number_actions; j++){
			Q[i][j]=rand()%number_actions;
		}
	}
	if(Q==NULL){
		printf("Erreur d'allocation de Q");
		exit (-1);
	}

	return Q;
}

void visit(float** Q){

	init_visited();

	int current_row=start_row;
	int current_col=start_col;

	while (visited[current_row][current_col]!=wall || visited[current_row][current_col]!=goal){

		if (action_max_Q(Q,current_row*cols+current_col)==0){
			current_row-=1;
		} else if (action_max_Q(Q,current_row*cols+current_col)==1){
			current_row+=1;
		} else if (action_max_Q(Q,current_row*cols+current_col)==2){
			current_col-=1;
		} else {
			current_col+=1;
		}
		
		if (visited[current_row][current_col]==crumb){
			break;
		}

		if (visited[current_row][current_col]!=goal && visited[current_row][current_col]!=wall){

			visited[current_row][current_col]=crumb;

		}

	}

}

void add_crumbs(){
     for (int i=0; i<rows; i++){
          for (int j=0; j<cols; j++){
              if (visited[i][j] ==crumb){
                  maze[i][j] ='.';
              }
          }
     }
     //maze[start_row][start_col]= 's';
 }



int main(){
   maze_make("maze2.txt");
   init_visited();

   srand(time(NULL));

   printf("%d, %d \n", rows, cols);
   printf("number of actions :  %d \n", number_actions); 
   maze_render();

   int size=rows*cols;

   float** Q=alloc_Q_1(size);  //création de Q (1=vide 2=aléatoire)

   Q_render(Q);


   // Définition des constantes

   int n_ep_max;
   printf("Nombre d'épisodes:");
   scanf("%d",&n_ep_max);

   float alpha;
   printf("Taux d'apprentissage alpha:");
   scanf("%f",&alpha);

   float gamma;
   printf("Taux d'impatience gamma:");
   scanf("%f",&gamma);

   float eps;
   printf("Taux de hasard epsilon:");
   scanf("%f",&eps);

   
   Q_learning(Q, n_ep_max, alpha, gamma, eps);    //création de notre matrice Q

   printf("Nombre d'épisodes:%d\nTaux d'apprentissage alpha:%.2f\nTaux d'impatience gamma:%.2f\nTaux de hasard epsilon:%.2f\n",n_ep_max, alpha, gamma, eps);  

   visit(Q);                              //faire le labyrinthe, selon les resultats de la matrice Q

   add_crumbs();

   maze_render();                         //On affiche le labyrinthe avec le parcours effectué, et on voit si ca a marché

   for (int i=0; i<size; i++){
   	free(Q[i]);
   }
   free (Q);

   return 0;
}

//alpha =0.1
//eps=0.1