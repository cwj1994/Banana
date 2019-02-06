# AutoLabelImages

AutoLabelImages is a child program of Darknet used to automatically generate labels for each frame of a video. This program is commonly used in the process of compressing bigger models and making them relevant to (and faster for) smaller projects. Additionally, this program is helpful for testing new models and finding specific frames that cause bugs.

### Installation and Running the Demo

AutoLabelImages is easy to install and run (the demo):

```sh
$ git clone https://gitlab.com/royceyang/AutoLabelImages.git
$ cd AutoLabelImages
$ bash rundemo.sh
```

Results will be saved in the test_videos directory.

AutoLabelImages only has two dependencies, and they are optional.

 - CUDA for GPU computation (faster).
 - OpenCV for support for a wider range of image types.

### Customization

If you would like to auto label custom objects, there are a few changes you need to make:
 - Weights: Train or obtain your own weights that you would like to use for detection.
 - Videos: Produce your own videos and put them in the test_videos directory.
 - Names: What objects do you want to detect? According to the weights that you use, update the classes in data/myawesomenames.names. Also remember to change the class count in cfg/myawesomecfg.data.
 - Makefile: Edit in the first 5 lines of Makefile to configure modes: GPU (needs CUDA installed), CUDNN, OpenCV, OpenMP, Debug.

### Running the Program

To run:

```sh
$ make # if you made changes in Makefile
$ ./AutoLabel detector demo cfg/myawesomecfg.data cfg/yolov2.cfg yolov2_498000.weights test_videos/TestVideo40.mp4 # you may replace yolov2_498000.weights and TestVideo40.mp4 with your own
$ ffmpeg -i test_videos/TestVideo40.mp4_%d_boxed.jpg -vcodec mpeg4 output.mp4 # if you want a video output of the results (remember to replace "TestVideo40.mp4" with the name of your own video)
```

Resulting labels will be saved in test_videos as *.txt. Resulting images will be saved in test_videos as *.jpg. Image files ending with _boxed are for debugging/reference only and are not essential.

### Todos

 - Reorganize output
 - Add support for image input (currently only supports video input)

