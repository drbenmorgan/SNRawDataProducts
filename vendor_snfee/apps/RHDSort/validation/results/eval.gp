set grid
set xlabel "Max inversion distance"
set ylabel "Counts"

set title "Distribution of max inversion distance for run #104"
plot [0:1500] 'run-104-evaluation.report' u (0.5*(column(1)+column(2))):3 notitle with histeps
pause -1

set terminal pdfcairo
set output "snfee-rhdsort_run-104_evaluation.pdf"
replot
set output
set terminal qt


# - end