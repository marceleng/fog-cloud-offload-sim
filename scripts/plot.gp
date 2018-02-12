set datafile separator ','
set terminal svg font "SFSS1200" fsize 14
set output "res.svg"
set xlabel 'Arrival time (s)'
set ylabel 'Service time (s)'
set xrange [0:500]
plot "<head -n 1000000 fog_sim_res_2000.00_1000000000.csv" using 3:($4-$3) every 10 with dots notitle
