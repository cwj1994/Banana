make
#./AutoLabel detector demo cfg/coco.data cfg/yolov3.cfg yolov3.weights test_videos/midea_adventure2.mp4
#./AutoLabel detector demo cfg/voc2.data cfg/yolov2.cfg yolov2c_480000.weights test_videos/luanxu2.mp4 -thresh 0.25
#./AutoLabel detector demo cfg/voc2.data cfg/yolov3.cfg yolov3_54000.weights test_videos/luanxu60.mp4 -thresh 0.2
./AutoLabel detector demo cfg/coco.data cfg/yolov3.cfg yolov3.weights test_videos/input.mp4 -thresh 0.3
