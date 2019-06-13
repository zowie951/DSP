#include <stdio.h>
#include <string.h>


int main(){
	FILE *fp = fopen("../testing_answer.txt","r");
	FILE *fp1 = fopen("./result1.txt","r");
	char ans[100],res[100],tmp[100];
	int right = 0,cnt = 0;
	while( fscanf(fp,"%s",ans) >0 && fscanf(fp1,"%s%s",res,tmp)>0){
		if(strcmp(ans,res) == 0)
			right++;
		cnt++;
	}
	printf("right:%d total:%d\nacc: %lf\n",right,cnt,right*1.0/cnt);
	return 0;
}
