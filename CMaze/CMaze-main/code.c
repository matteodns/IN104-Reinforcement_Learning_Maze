#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mazeEnv.h"


int action_max_Q(float** Q, int s){     //retourne l'action

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

int action_max_Q_rand(float** Q, int s){     //retourne l'action

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

/*
On remarque qu'avec cette méthode, un fort taux de hasard epsilon et un fort taux d'apprentissage alpha est nécessaire pour que la solution soit trouvée dans un temps correct, sinon les directions tournent en rond. Cela est aussi du à l'environnement, où une seul case permet d'obtenir une unique récompense, ce qui n'est pas optimale pour cette méthode.
*/


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
				printf("0 ");
			}

		}
		printf("\n");
	}

}


void Q_learning(float** Q, int n_ep_max, float alpha, float gamma, float eps) {

	int start_s = start_row*cols+start_row;

	envOutput episode;
	action direction;

	for (int n_ep=1; n_ep<=n_ep_max; n_ep++){   //pour chaque episode (<nb_ep_max)

		printf("Episode n°%d\n", n_ep);


		//Initialisation de l'épisode

		int s = start_s;
		state_row=start_row;
		state_col=start_col;
		episode.reward=0;
		episode.done=0;


		int k=1;

		while (episode.done==0) {    //Tant que s n'est pas terminal

			printf("Episode n°%d, étape n°%d\n",n_ep,k);
			
			direction = e_greedy(eps, Q, s);

			printf("Direction : %d\n",direction);

			episode = maze_step(direction);


			signed int r=episode.reward;	
			int new_s=episode.new_row*cols+episode.new_col;

			//Calcul de max_a_Q

			float max_a_Q = Q[new_s][0];		
			for (int i=0; i<number_actions; i++){ 
				if (Q[new_s][i]>max_a_Q) {
					max_a_Q=Q[new_s][i];
				}
			}

			Q[s][direction] = Q[s][direction] + alpha * (r + gamma*max_a_Q - Q[s][direction]);
			
			s=new_s;
			
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

/*
Problème, la matrice est initialisée à 0 partout, ce qui a pour conséquence de fixer au début, des mouvement vers le haut uniquement (si on ne prends pas un assez grand taux de hasard, on se retrouve bloqué à la première ligne). Pour remedier à ca, on peut essayer d'initialiser la matrice Q aléatoirement.
*/


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

		visited[current_row][current_col]=crumb;

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
   maze_make("maze.txt");
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

   
   Q_learning(Q, n_ep_max, alpha, gamma, eps);

   //Q_render(Q);
   

   printf("Nombre d'épisodes:%d\nTaux d'apprentissage alpha:%.2f\nTaux d'impatience gamma:%.2f\nTaux de hasard epsilon:%.2f\n",n_ep_max, alpha, gamma, eps);


/*   
   while (visited[goal_row][goal_col]!=crumb){

   	Q_learning(Q, n_ep_max, alpha, gamma, eps);
   	printf("Q-learning OK \n");
   	Q_render(Q);

   	init_visited();

   	printf("initialisation visited OK\n");

   	visit(Q);
   	add_crumbs();		//fait apparaitre les points visités
   	maze_render();



   }
*/   



   for (int i=0; i<size; i++){
   	free(Q[i]);
   }
   free (Q);

   return 0;
}

//alpha =0.1
//eps=0.1