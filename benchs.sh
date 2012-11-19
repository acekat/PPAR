#!/bin/sh


#for i in 1536 24576 196608 786432 1572864 3145728 12582912
for i in 1572864
do
	for f in `seq 1 $1`
	do
		make -B bench SORT=$2 N=$i P=$3 PNODE=$4 TH=$5 HF=$6 
	done
done