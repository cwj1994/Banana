#ifndef FaceLivenessDetect_H
#define FaceLivenessDetect_H

#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace std;
using namespace cv;

// Exchanging detection results, -1 signals undefined value
struct BoundingBox {
	// Image coordinate convention: (0, 0) is the top-left corner, (1280, 720) is the bottom-right corner
	int left;  // x coordinate for top-left corner of box
	int top;  // y coordinate for top-left corner of box
	int right;  // x coordinate for bottom-right corner of box
	int bottom;  // y coordinate for bottom-right corner of box
	float detection_confidence;  // detection confidence from model
};

// Exchanging full recognition results, -1 signals undefined value
struct FullResult {
	int left, top, right, bottom;
	float detection_confidence;
	int person_id_in_database;  // Person ID in internal database
	float recognition_confidence;  // recognition confidence from model
};

// class definition
class CFaceLivenessDetect
{
public:
    CFaceLivenessDetect();
    ~CFaceLivenessDetect();
    int setInitFace(vector<struct BoundingBox> faces);
    void findFace();
    void findFace(Mat& frame, CascadeClassifier& cascade, bool tryflip, double scale);
    struct BoundingBox getFaceDetect(int index, int type);
    int getFaceNum(int type);
private:
    void drawface( Mat& img, Rect *facerects, int nFace, int shape=0);
    void appendTracker(int x, int y, int w, int h);
    int isNear(Rect bbox1, Rect bbox2, double fr);
    void filterFace();
    bool isSameSize(Rect bbox1, Rect bbox2);
    bool intersect(Rect bbox1, Rect bbox2);
    void detectface( Mat& img, CascadeClassifier& cascade, bool tryflip, double scale);
    void updateTracking();
    void drawface( Mat& img);
    void eliminateFalse();
    void confirmFace();
    
    int nFaceDetect, nFaceTrack, nFaceCandi;
    int nID; // 跟踪目标的ID号
    int life_track; // 被跟踪框的最大生命数
    int maxMoveT;  // 运动确认的阈值
    int countT;    // 真脸的确认阈值
    int candi_life_inc;  // 每检测到1次，备选脸生命值增加值
    int candi_life_top; // 备选脸生命值的上限
};

#endif
