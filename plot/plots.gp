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
set xrange [1:2]
set ytics
set grid y
set auto x

set term png
set output 'v1-v2.png'

plot \
newhistogram "MPI pur 1536", \
'../bench/v1-v2' using 4:xtic(2) t col, \
newhistogram "MPI pur 1572864", \
'' u 8:xtic(6) t col, \
newhistogram "Hybride 1536", \
'' u 12:xtic(10) t col, \
newhistogram "Hybride 1572864", \
'' u 16:xtic(14) t col


# set title "ALL 196608"
# set ylabel "Nombre"
# set grid
# set log x
# set auto y

# set terminal png
# set output "all.png"

# plot '../bench/all-196608' using ($1/$4):2 with lines, \
# 	'' u ($1/$5):2 with lines, \
# 	'' u ($1/$6):2 with lines, \
# 	'' u ($1/$7):2 with lines, \
# 	'' u ($1/$8):2 with lines 