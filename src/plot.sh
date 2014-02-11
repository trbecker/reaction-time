#!/bin/bash

# Extract data from database
sqlite3 ${HOME}/.local/reaction_time/data.sqlite3 "SELECT strftime('%s', time), reaction_time FROM data;" > data.dat
sqlite3 ${HOME}/.local/reaction_time/data.sqlite3 "SELECT strftime('%s', time), avg(reaction_time) FROM data GROUP BY time;" > avg.dat

cat << EOF | gnuplot
set term png
set output "plot-$(date +%Y-%m-%d).png"
set datafile separator "|"

set xlabel "Sample"
set ylabel "Reaction time (ms)"
plot "data.dat" pt 7 ps .5  lc 3 t "Raw", "avg.dat" w lines ls 10 lc 1 t "Average"
EOF

rm data.dat
