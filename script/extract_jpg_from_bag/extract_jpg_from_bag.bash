COUNTER=0
for i in `cat baglist.txt`
do
mkdir -p /home/etcvision/Desktop/ROSbag/all/images/$COUNTER/
roslaunch export.launch bag_in:=$i
mv ~/.ros/frame*.jpg /home/etcvision/Desktop/ROSbag/all/images/$COUNTER/
let COUNTER=COUNTER+1
done
