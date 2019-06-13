for i in {1..10};
do 
	./disambig -text ../testdata/$i.txt -map ../ZhuYin-Big5.map -lm ../bigram.lm -order 2 > ../result/$i.txt
done
