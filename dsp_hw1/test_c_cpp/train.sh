#!/bin/bash


iter=10
for i in $(seq 1 5)
do
	./train $iter ../model_init.txt ../seq_model_0$i.txt model_0$i.txt &
done


