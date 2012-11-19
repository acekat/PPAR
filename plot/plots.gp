#
# 'newhistogram' keyword to plot
#
set title "Immigration from different regions\n(give each histogram a separate title)"
set key under nobox
set style histogram clustered gap 1 title offset 2,0.25
set style fill solid noborder
set boxwidth 0.95
unset xtics
set xtics nomirror rotate by -45 scale 0 font ",8"
set xlabel "(note: histogram titles have specified offset relative to X-axis label)" offset 0,-2
set yrange [0:30]
set ytics
set grid y
set auto x

set term png
set output 'v1-v2.png'

plot \
newhistogram "MPI pur", \
'../bench/v1-v2' using 4:xtic(2) t col, '' u 8 t col, \
newhistogram "Hybride", \
'' u 11:xtic(10) t col, '' u 15 t col
#