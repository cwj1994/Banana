COUNT=1
for i in *.jpg
do
str1=${i%.jpg}
echo $str1
mv "$str1".jpg images/images_2_"$COUNT".jpg
let COUNT=COUNT+1
done
