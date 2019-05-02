for i in *.txt
do
str1=${i%.txt}
echo $str1
cp ../../IMGDATA/"$str1".jpg .
done