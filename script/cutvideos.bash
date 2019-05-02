for i in *.avi
do
str1=${i%.avi}
echo $str1
ffmpeg -i "$str1".avi -q:v 1 -r 5 "$str1"_%04d.jpg
done
