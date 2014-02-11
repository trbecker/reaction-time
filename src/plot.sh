#!/bin/bash

# Extract data from database
sqlite3 ${HOME}/.local/reaction_time/data.sqlite3 "SELECT strftime('%s', time), reaction_time FROM data;" > data.dat
sqlite3 ${HOME}/.local/reaction_time/data.sqlite3 "SELECT strftime('%s', time), avg(reaction_time) FROM data GROUP BY time;" > avg.dat

cat << EOF | gnuplot
set term png
set output "plot-$(date +%Y-%m-%d).png"
set datafile separator "|"

set xlabel "Sample"
set xdata time
set timefmt "%s"
set format x "%F %H:%m"
set xtics rotate

set ylabel "Reaction time (ms)"
plot "data.dat" u 1:2 pt 7 ps .5  lc 3 t "Raw", "avg.dat" u 1:2 w lines ls 10 lc 1 t "Average"
EOF

rm data.dat
