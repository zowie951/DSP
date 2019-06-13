#include <stdio.h>
#include "Ngram.h"

int main(int argc, char *argv[])
{

    int ngram_order = 3;
    Vocab voc;
    Ngram lm( voc, ngram_order );

    {
        const char lm_filename[] = "./corpus.lm";
        File lmFile( lm_filename, "r" );
        lm.read(lmFile);
        lmFile.close();
    }

    VocabIndex wid = voc.getIndex("ʨ");
    if(wid == Vocab_None) {
        printf("No word with wid = %d\n", wid);
        printf("where Vocab_None is %d\n", Vocab_None);
    }

    wid = voc.getIndex("�w��");
    VocabIndex context[] = {voc.getIndex("�}") , voc.getIndex("�r"), Vocab_None};
    printf("log Prob(�w��|�r-�}) = %f\n", lm.wordProb(wid, context));
}
