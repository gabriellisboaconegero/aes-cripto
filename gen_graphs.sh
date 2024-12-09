gnuplot -persist << EOFMarker
        set datafile separator ","
        set ylabel "Tempo (ms)"
        set xlabel "Tamanho KBytes"
        set terminal png size 900,500
        set output "comparacao.png"
        set title "Comparação AES"
        plot "data.csv" using 1:2 with lines title "OURS: Enc", \
             "data.csv" using 1:3 with lines title "OURS: Dec", \
             "data.csv" using 1:4 with lines title "OPENSSL: Enc", \
             "data.csv" using 1:5 with lines title "OPENSSL: Dec"

EOFMarker
