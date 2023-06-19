loop=12
i=0
max_nodenum=100
rm -rf result && mkdir result
rm -rf visual_result && mkdir visual_result
for k in $(seq 0 $max_nodenum)
do
	rgb[$k]=$(printf %#X%X%X $[$[RANDOM%127]+128] $[$[RANDOM%127]+128] $[$[RANDOM%127]+128])
done

draw_parameter(){
	file_name=$1 && file_name=${file_name// /_}".png"
	file_name='"'$file_name'"'
	echo "gnuplot -persist <<EOF" > use_gnuplot.sh
	echo "set term pngcairo" >> use_gnuplot.sh
	echo "set output $file_name" >> use_gnuplot.sh
	echo "set title "'"'$1'"' >> use_gnuplot.sh

	draw_para=${raw#*$1}
	draw_para=${draw_para%$1*}
	draw_para=${draw_para#*:}
	count=0 && for tmp in $draw_para; do if [[ $tmp =~ 'ID' ]]; then count=$[count+1]; fi; done

	num=$count
	count=$[count-1]
	x_gap=$(echo | awk "{print 1/$num}")
	y_gap=$(echo | awk "{print 1/$2}")
	offset=$(echo | awk "{print $x_gap/2}")

	echo "set xrange[1:$[num+1]]" >> use_gnuplot.sh
	echo "set yrange[0:$2]" >> use_gnuplot.sh
	echo "set xtics offset graph $offset,0 autofreq 1,1,$num nomirror" >> use_gnuplot.sh
	echo "set ytics autofreq 0,1,$2 nomirror" >> use_gnuplot.sh
	echo "set nokey" >> use_gnuplot.sh
	echo "set grid" >> use_gnuplot.sh
	draw_para=$(echo $draw_para | sed s/[[:space:]]//g)
	while [ $count -gt 0 ]
	do
		j=0
		while [ 5 -gt $j ]
		do
			draw_para=${draw_para#*[}
			case $j in
				0)
					id=${draw_para%%]*}
					;;
				1)
					y0=${draw_para%%]*}
					;;
				2)
					y1=${draw_para%%]*}
					;;
				3)
					x0=${draw_para%%]*}
					;;
				4)
					x1=${draw_para%%]*}
					x1=$[x1+1]
					;;
			esac
			j=$[j+1]
		done
		count=$[count-1]

		tmp_rgb='"'${rgb[$id]}'"'
		echo "set style rectangle fc rgb $tmp_rgb fs solid 1 border lc rgb 'black' lw 1" >> use_gnuplot.sh
		echo "set object $id rectangle from $x0,$y0 to $x1,$y1" >> use_gnuplot.sh
		label='"'t$id'"'
		label_x=$[x0+x1]
		label_x=$(echo | awk "{print $label_x/2}")
		label_y=$[y0+y1]
		label_y=$(echo | awk "{print $label_y/2}")
		echo "set label $label at $label_x,$label_y center font '8'" >> use_gnuplot.sh
	done
	echo "plot 0" >> use_gnuplot.sh
	echo "exit()" >> use_gnuplot.sh
	echo "EOF" >> use_gnuplot.sh
	chmod +x use_gnuplot.sh
	./use_gnuplot.sh > /dev/null 2>&1
	rm use_gnuplot.sh
}

while [ $loop -gt $i ]
do
	echo =====Graph No.$[$i+1] Memory Reuse under three algorithms=====
	./memory_reuse_test $i
	grep -ri  memory_size result/result$i
	memory_size=$(grep -ri  memory_size result/result$i)
	memory_size=$(echo $memory_size | sed s/[[:space:]]//g)
	memory_size=${memory_size#*memory_size=}
	raw=$(cat result/result$i)
	mkdir visual_result/visual_result$i
	pushd visual_result/visual_result$i > /dev/null
	draw_parameter 'Eager Reuse' ${memory_size%%overhead*}
	memory_size=${memory_size#*memory_size=}
	draw_parameter 'Large tensor first' ${memory_size%%overhead*}
	memory_size=${memory_size#*memory_size=}
	draw_parameter 'Short lifetime first' ${memory_size%%overhead*}
	memory_size=${memory_size#*memory_size=}
	draw_parameter 'Global Optimization' ${memory_size%%overhead*}
	popd > /dev/null
	i=$[i+1]
	echo
done
