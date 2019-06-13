#include <math.h>

#define LEN 50
#define MAX_STATE 6
#define MAX_OBSERV 6

#include "hmm.h"



int main(int argc,char *argv[]){
	HMM hmm_model[6];
	int cnt = load_models(argv[1],hmm_model,5);
	if(cnt < 5){
		fprintf(stderr,"load_models error\n");
		exit(0);
	}

	FILE *test_fp = open_or_die(argv[2],"r");
	FILE *output_fp = open_or_die(argv[3],"w");
	char token[51] = {};
	while( fscanf(test_fp,"%s",token) >0){
		int best_model = 0;
		double best_p = 0.0;
		for(int model_index=0 ; model_index<cnt ; model_index++){
			double sigma[LEN][MAX_STATE] = {};
			for(int i=0;i<MAX_STATE;i++){
				int obser = token[0] - 'A';
				sigma[0][i] = hmm_model[model_index].initial[i] * hmm_model[model_index].observation[obser][i];			
			}
			for(int t=1;t<LEN;t++){
				int obser = token[t] - 'A';
				for(int j=0;j<MAX_STATE;j++){
					for(int i=0;i<MAX_STATE;i++){
						double tmp = sigma[t-1][i] * hmm_model[model_index].transition[i][j]*hmm_model[model_index].observation[obser][j];
						if(tmp > sigma[t][j])
							sigma[t][j] = tmp;
					}
				}
			}
			for(int i=0;i<MAX_STATE;i++)
				if(best_p < sigma[LEN-1][i]){
					best_p = sigma[LEN-1][i];
					best_model = model_index;
				}

		}

		fprintf(output_fp,"model_0%d.txt %e\n",best_model+1,best_p);
	}


	return 0;
}
