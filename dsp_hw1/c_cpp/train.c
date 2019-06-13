#include <math.h>

#define LEN 50
#define MAX_STATE 6
#define MAX_OBSERV 6

#include "hmm.h"


void Copy_hmm(HMM *des,HMM *source){
	strcpy(des->model_name,source->model_name);
	des->state_num = source->state_num;
	des->observ_num = source->observ_num;
	for(int i=0;i<MAX_STATE;i++){
		des->initial[i] = source->initial[i];
	}
	for(int i=0;i<MAX_STATE;i++)
		for(int j=0;j<MAX_STATE;j++)
			des->transition[i][j] = source->transition[i][j];
	for(int i=0;i<MAX_OBSERV;i++)
		for(int j=0;j<MAX_STATE;j++)
			des->observation[i][j] = source->observation[i][j];
}
int main(int argc,char *argv[]){
	HMM hmm,next_hmm;
	//loadHMM( &hmm,"../model_init.txt");
	loadHMM( &hmm, argv[2]);

	//int iter = 1;
	int iter = atoi(argv[1]);
	
	char *filename = argv[3];
	//char *filename = "../seq_model_01.txt";
	
	char *output = argv[4];

	next_hmm.model_name = (char *)malloc(strlen(filename)+1);

	FILE *fp = open_or_die(filename,"r");
	char token[51] ={};
	
	while(iter--){
		strcpy(next_hmm.model_name,hmm.model_name);
		next_hmm.state_num = 6;
		next_hmm.observ_num = 6;
		memset(next_hmm.observation,0,sizeof(next_hmm.observation));
		memset(next_hmm.initial,0,sizeof(next_hmm.initial));
		memset(next_hmm.transition,0,sizeof(next_hmm.transition));

		double total_gamma[MAX_STATE] = {},total_gamma_without_T[MAX_STATE] ={};

		int cnt = 0;
		fseek(fp,0,SEEK_SET);
		while( fscanf(fp,"%s",token) >0 ){
			double alpha[LEN][MAX_STATE] = {};
			double beta[LEN][MAX_STATE] = {};
			for(int i=0;i< hmm.state_num;i++){
				int obser = token[0]-'A';
				alpha[0][i] = hmm.initial[i]* hmm.observation[obser][i];
			}
			int len = strlen(token);
			for(int t=1;t< len;t++){
				int obser = token[t] - 'A';
				for(int i=0;i<hmm.state_num;i++){
					for(int j=0;j<hmm.state_num;j++){
						alpha[t][i] += alpha[t-1][j]*hmm.transition[j][i]*hmm.observation[obser][i];
					}	
				}
			}
			//for(int t=0;t<LEN;t++,printf("\n"))
			//	for(int i=0;i<MAX_STATE;i++)
			//		printf("%lf ",alpha[t][i]);
			//exit(0);

			for(int i=0;i<MAX_STATE;i++)
				beta[LEN-1][i] = 1.0;
			for(int t=LEN-2;t>=0;t--){
				int obser = token[t+1] - 'A';
				for(int i=0;i<MAX_STATE;i++)
					for(int j=0;j<MAX_STATE;j++)
						beta[t][i] += hmm.transition[i][j]*hmm.observation[obser][j]*beta[t+1][j];
			}
			double gamma[LEN][MAX_STATE] = {};
			for(int t=0;t<LEN;t++){
				int obser = token[t] - 'A';
				double total = 0.0;
				for(int i=0;i<MAX_STATE;i++)
					total += alpha[t][i]*beta[t][i];
				for(int i=0;i<MAX_STATE;i++){
					gamma[t][i] = alpha[t][i]*beta[t][i] / total;
					total_gamma[i] += gamma[t][i];
					next_hmm.observation[obser][i] += gamma[t][i];
					if(t<LEN-1){
						total_gamma_without_T[i] += gamma[t][i];
					}
					if(t==0)
						next_hmm.initial[i] += gamma[t][i];
				}
			}

			//for(int t=0;t<LEN;t++,printf("\n"))
			//	for(int i=0;i<MAX_STATE;i++)
			//		printf("%lf ",gamma[t][i]);
			//exit(0);

			double epsilon[LEN-1][MAX_STATE][MAX_STATE] = {};
			for(int t=0;t<LEN-1;t++){
				double total = 0.0;
				int obser = token[t+1] - 'A';
				for(int i=0;i<MAX_STATE;i++)
					for(int j=0;j<MAX_STATE;j++)
						total += alpha[t][i] * hmm.transition[i][j]*hmm.observation[obser][j]*beta[t+1][j];
				for(int i=0;i<MAX_STATE;i++)
					for(int j=0;j<MAX_STATE;j++){
						epsilon[t][i][j] = alpha[t][i]*hmm.transition[i][j]*hmm.observation[obser][j]*beta[t+1][j] / total;
						next_hmm.transition[i][j] += epsilon[t][i][j];
					}
			}



			cnt++;
		}
		//calculate next pi
		for(int i=0;i<MAX_STATE;i++)
			next_hmm.initial[i] /= cnt;
		//calculate next a
		for(int i=0;i<MAX_STATE;i++){
			double sum = 0;
			for(int j=0;j<MAX_STATE;j++){
				next_hmm.transition[i][j] /= total_gamma_without_T[i];
				//printf("%lf %lf   ",next_hmm.transition[i][j],total_gamma_without_T[i]);
				sum += next_hmm.transition[i][j];
			}
			//printf("%lf\n",sum);
		}
		//calculate next obser
		for(int i=0;i<MAX_OBSERV;i++)
			for(int j=0;j<MAX_STATE;j++)
				next_hmm.observation[i][j] /= total_gamma[j];


		Copy_hmm(&hmm,&next_hmm);
	}
	FILE *output_fp = open_or_die(output,"w");	
	dumpHMM(output_fp,&hmm);

	return 0;


}
