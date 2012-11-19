#! /bin/bash
if [ -f $1 ]
then
	rm $1
fi

if [ -f gnuplot.gp ]
then
	rm gnuplot.gp
fi

echo "set term postscript enhanced color" >> gnuplot.gp
echo "set output '"$1"'" >> gnuplot.gp
#echo "set yrange [$5:$6]" >> gnuplot.gp
#echo "set xrange [0:$4]" >> gnuplot.gp
echo "set ylabel 'nombre delements';" >> gnuplot.gp
echo "set xlabel 'temps en seconde';" >> gnuplot.gp

#echo "plot '$2' using (\$3/\$1):(\$4*\$5) smooth unique title 'Pram MPI Pure ' with linespoints;" >> gnuplot.gp
echo "plot '$2' using (\$3/\$1):(\$4*\$5) smooth unique title 'Pram MPI Pure ' with linespoints, '$3' using (\$3/\$1):(\$4*\$5) smooth unique title 'Pram Hybrid MPI OpenMP' with linespoint;" >> gnuplot.gp
#echo "plot '$2' using 3:2 smooth unique title 'qsort glibc ' with linespoints, '$3' using 3:2 smooth unique title 'myqsort hybride' with linespoint,'$4' using 3:2 smooth unique title 'myqsort en OpenMP' with linespoint;" >> gnuplot.gp
#echo "plot '$2' using 3:2 smooth unique title 'qsort glibc ' with linespoints, '$3' using 3:2 smooth unique title 'myqsort hybride' with linespoints,'$4' using 3:2 smooth unique title 'myqsort en OpenMP' with linespoints,'$5' using 3:2 smooth unique title 'pram Hybride' with linespoints,'$6' using 3:2 smooth unique title 'pram MPI Pur' with linespoints;" >> gnuplot.gp
gnuplot gnuplot.gp
#rm gnuplot.gp
