#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>
#include <stdint.h>
#include <locale.h>
#include <ctype.h>
#include <bitset>
#include "Ngram.h"

Vocab Voc;
//#define false

inline uint16_t makeBig5(uint8_t a, uint8_t b){
	return (((uint16_t)a)<<8 | ((uint16_t)b));	
}

inline uint16_t makeBig5(const char *c){
	return makeBig5(c[0],c[1]);
}




//viterbi
map<uint16_t, double> prob_map[2];

vector<map<uint16_t,uint16_t>> backtrack;

static void viterbi_first(vector<uint16_t> &words,Ngram &lm){
	prob_map[0].clear();
	prob_map[1].clear();
	backtrack.clear();
	for(int i=0;i<words.size();i++){
		prob_map[0][words[i]] = 0.0;
		/*
		uint8_t str[3] = {0};
		str[0] = words[i]>>8;
		str[1] = words[i];
		cout<<(const char*)str<<' ';
		*/
	}

}

VocabIndex find_index(uint16_t word){
	uint8_t str[3] = {0};
	str[0] = word>>8;
	str[1] = word;
	return Voc.getIndex((const char *)str);
}

double get_prob(VocabIndex w1,VocabIndex w2,Ngram &lm){
	VocabIndex context[] = {w1,Vocab_None};
	return lm.wordProb(w2,context);
}


static void viterbi(vector<uint16_t> &words,Ngram &lm){
	prob_map[1].clear();
	backtrack.push_back(map<uint16_t,uint16_t>());
	
	for(int i=0;i<words.size();i++){
		uint16_t word = words[i];
		VocabIndex word_id = find_index(word);
		if (word_id == Vocab_None)
			word_id = Voc.getIndex(Vocab_Unknown);

		double max_prob = -1.0/0.0;
		uint16_t max_prob_word = 0;
		//cout<<prob_map[0].size()<<'\n';
		for(map<uint16_t,double>::iterator it=prob_map[0].begin();it!= prob_map[0].end();it++){
			uint16_t prev_word = it->first;
			double prev_prob = it->second;
			VocabIndex prev_word_id = find_index(prev_word);
			LogP p = get_prob(prev_word_id,word_id,lm);
		//	cout<<p<<'\n';
			if(p == LogP_Zero)
				p = -100;
			double pro = prev_prob + p;
			if(pro > max_prob){
				max_prob = pro;
				max_prob_word = prev_word;
			}
			
		}
		if(max_prob_word != 0){ 
			prob_map[1][word] = max_prob;
			backtrack.back()[word] = max_prob_word;			
		}
		else{
			fprintf(stderr,"abort\n");
			abort();
		}

	}
	swap(prob_map[0],prob_map[1]);
}

static void viterbi_result(Ngram &lm){
	double max_prob = -1.0/0.0;
	uint16_t word;
	for(map<uint16_t,double>::iterator it=prob_map[0].begin();it!=prob_map[0].end();it++){
		uint16_t tmp_word = it->first;
		double pro = it->second;

		//Vocab_SentEnd
		VocabIndex end_id = Voc.getIndex(Vocab_SentEnd);
		VocabIndex word_id = find_index(tmp_word);
		if(word_id == Vocab_None)
			word_id = Voc.getIndex(Vocab_Unknown);
		LogP p = get_prob(word_id,end_id,lm);
		if(pro+p > max_prob){
			word = tmp_word;
			max_prob = pro;
		}
	}
	//cout<<"backtrack size: "<<backtrack.size()<<endl;
	vector<uint16_t> result;
	result.push_back(word);
	while(!backtrack.empty()){
		word = backtrack.back()[word];
		backtrack.pop_back();
		result.push_back(word);
	}
	reverse(result.begin(),result.end());
	for(int i=0;i<result.size();i++){
		uint8_t str[3] = {0};
		str[0] = result[i]>>8;
		str[1] = result[i];
		cout<<(const char*)str<<' ';
	}
	return;

}






// ./mydisambig    ZhuYin-Big5.map   Xgram.lm   order_num   input_file
int main(int argc,char *argv[]){
	//printf("%d",argc);
	fprintf(stderr,"%s\n", setlocale(LC_ALL, "zh_TW.BIG5"));
	if(argc < 5){
		fprintf(stderr,"Usage:./mydisambig    ZhuYin-Big5.map   Xgram.lm   order_num    input_file\n");
		exit(1);
	}
	const char *map_file_name = argv[1];
	const char *lm_file = argv[2];
	int ngram_order = atoi(argv[3]);
	const char *input_file(argv[4]);
	//read language model
	Ngram lm( Voc, ngram_order );
	File lmFile( lm_file, "r");
	lm.read( lmFile);
	lmFile.close();
//build the map table	
	map <uint16_t, vector<uint16_t>> table;
	
/*	
	FILE *fp = fopen(map_file,"r");
	if(!fp){
		fprintf(stderr,"open map file fail!\n");
		exit(1);
	}
	fprintf(stderr,"open map file success!\n");
	
	char line[1024];
	while( (fgets(line,1024,fp)) != NULL ){
		char *ptr = line;
		vector<uint16_t> vec;
		uint16_t key = makeBig5(ptr[0],ptr[1]);
		for(const char *p = ptr+3;*p;){
			uint16_t val = makeBig5(p[0],p[1]);
			vec.push_back(val);
			p+=2;
			while ((*p) == ' '||(*p) == '\n')
				p++;

		}
		table[key] = vec;
	}
	fclose(fp);
*/
	ifstream map_file(map_file_name);
  	for (std::string line; std::getline(map_file, line);) {
    		const char* ptr = line.c_str();

    		uint16_t key = makeBig5(ptr[0], ptr[1]);
    		vector<uint16_t> value;
    		for (const char* p = ptr+3; *p; ) {
      			uint16_t v = makeBig5(p[0], p[1]);
      			value.push_back(v);
      			p += 2;
      			while (isspace(*p)) ++p;
    		}
    		table[key] = value;
  	}
  	map_file.close();

#ifdef false
	{
		    uint16_t key = makeBig5("«G");
		        vector<uint16_t> value = table[key];
			for (int i = 0; i < value.size(); ++i) {
				        uint8_t v[3] = {0};
					v[0] = value[i] >> 8;
					v[1] = value[i] ;
					std::cout << (const char*)v << '\n';
		        }
	}

#endif
//start viterbi

	ifstream input_fp(input_file);
		
	for(string line;getline(input_fp,line);){
		const char *ptr = line.c_str();
		int is_first_word = 1;
		
		while(isspace(*ptr))
			ptr++;
		for(const char *p = ptr;*p;){
			uint16_t key = makeBig5(p);
			vector<uint16_t> words = table[key];
			//cout<<"words "<<words.size()<<endl;
			if(is_first_word){
				viterbi_first(words,lm);
				is_first_word = 0;
			}
			else{
				viterbi(words,lm);
			}
			p += 2;
			while( isspace(*p))
				p++;
		}
	
		cout<<"<s> ";
		viterbi_result(lm);
		cout<<"</s>"<<endl;
	}
	return 0;
}

