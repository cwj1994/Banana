// v1.2.1@20180325

#include "FaceLivenessDetect.h"

#define NUM_FACE_DETECT 20  // 每帧图像中检测到的最大脸数
#define NUM_FACE_TRACK  10  // 当前处于跟踪状态下的脸数
#define NUM_FACE_CANDI  10  // 当前处于候选状态下的脸数
#define NUM_FACE_DETECT_WIDTH 5  // 每帧图像中检测到的最大脸数
#define NUM_FACE_TRACK_WIDTH  6  // 当前处于跟踪状态下的脸数
#define NUM_FACE_CANDI_WIDTH  11  // 当前处于候选状态下的脸数
int faceDetect[NUM_FACE_DETECT][NUM_FACE_DETECT_WIDTH]; // x,y,w,h,type(0-新, 1-跟踪, 2-候选)
int faceTrack[NUM_FACE_TRACK][NUM_FACE_TRACK_WIDTH];   // x,y,w,h,ID,life(<=0-跟踪失败)
int faceCandi[NUM_FACE_CANDI][NUM_FACE_CANDI_WIDTH];  // x, y, w, h, life, move, proc, x0, y0, w0, h0

CFaceLivenessDetect::CFaceLivenessDetect()
{
    nFaceDetect=0; nFaceTrack=0; nFaceCandi=0;
    nID = 0; // 跟踪目标的ID号
    life_track = 20; // 被跟踪框的最大生命数
    maxMoveT = 5;  // 运动确认的阈值
    countT = 10;    // 真脸的确认阈值
    candi_life_inc = 2;  // 每检测到1次，备选脸生命值增加值
    candi_life_top = 20; // 备选脸生命值的上限
}
CFaceLivenessDetect::~CFaceLivenessDetect()
{}

int left;  // x coordinate for top-left corner of box
	int top;  // y coordinate for top-left corner of box
	int right;  // x coordinate for bottom-right corner of box
	int bottom;  // y coordinate for bottom-right corner of box
	float detection_confidence;

// type: 1-detected, 2-candidates, 3-tracked
// left==right: invalid
struct BoundingBox CFaceLivenessDetect::getFaceDetect(int index, int type)
{
    struct BoundingBox bbox;
    bbox.left = 0; bbox.top = 0; bbox.right = 0; bbox.bottom = 0; bbox.detection_confidence = 0;
    int num = getFaceNum(type);
    if (index>=num){        
        return bbox;
    }
    if (type==0){
        bbox.left = faceDetect[index][0]; bbox.top = faceDetect[index][1]; 
        bbox.right = faceDetect[index][0]+faceDetect[index][2]-1; 
        bbox.bottom = faceDetect[index][1]+faceDetect[index][3]; 
        bbox.detection_confidence = 1;
    }
    else if (type==1){
        bbox.left = faceCandi[index][0]; bbox.top = faceCandi[index][1]; 
        bbox.right = faceCandi[index][0]+faceCandi[index][2]-1; 
        bbox.bottom = faceCandi[index][1]+faceCandi[index][3]; 
        bbox.detection_confidence = 1;
    }
    else if (type==2){
        bbox.left = faceTrack[index][0]; bbox.top = faceTrack[index][1]; 
        bbox.right = faceTrack[index][0]+faceTrack[index][2]-1; 
        bbox.bottom = faceTrack[index][1]+faceTrack[index][3]; 
        bbox.detection_confidence = 1;
    }
    else{}
    
    return bbox;
}

int CFaceLivenessDetect::getFaceNum(int type)
{
    int num = (type==0? nFaceDetect: (type==1? nFaceCandi: nFaceTrack));
    return num;
}


// for DL only which means we remove the fast detection part
//vector<Rect> faces;
// int setInitFace(struct BoundingBox face)
// based on the assumption that we can get multiple faces in one frame
int CFaceLivenessDetect::setInitFace(vector<struct BoundingBox> faces)
{
    nFaceDetect = faces.size();
    for ( size_t i = 0; i < faces.size(); i++ ){
        faceDetect[i][0] = cvRound(faces[i].left);
        faceDetect[i][1] = cvRound(faces[i].top);
        faceDetect[i][2] = cvRound(faces[i].right - faces[i].left + 1);
        faceDetect[i][3] = cvRound(faces[i].bottom - faces[i].top + 1);
        faceDetect[i][4] = 0; // temp
    }
    
    CFaceLivenessDetect::filterFace();
    
    return nFaceDetect;  // updated nFaceDetect
}

//void findFace(Mat& img)
void CFaceLivenessDetect::findFace()
{
    // call deep learning model to get human faces (input & output: nFaceDetect, faceDetect)
    // tbd (by DeepLearning Model part)
   // confirmFace();
    
    // track faces
    updateTracking();
    //printf("face tracking: %d\n", nFaceTrack);
    
    // eliminate False face
    eliminateFalse();
    //for (int i=0; i<nFaceCandi;i++)
    //    printf(" Candi: %d: life=%d, move=%d, moved=%d\n", i+1, faceCandi[i][4], faceCandi[i][5], faceCandi[i][6]);
    
    // draw face boxes
    //drawface(frame);   // for debug ONLY 可以删除掉，结果就是Mat& img也不需要了。findFace()是空。
}


void CFaceLivenessDetect::findFace(Mat& frame, CascadeClassifier& cascade, bool tryflip, double scale)
{
    // detect face using fast face datector
    // (nFaceDetect: number of face candidates, faceDetect: bounding boxes of all the face candidates)
    detectface( frame, cascade, tryflip, scale);
    //printf("face original found: %d\n", nFaceDetect);

    // call deep learning model to get human faces (input & output: nFaceDetect, faceDetect)
    // tbd (by DeepLearning Model part)
    confirmFace();

    // track faces 
    updateTracking();
    //printf("face tracking: %d\n", nFaceTrack);

    // eliminate False face
    eliminateFalse();
    //for (int i=0; i<nFaceCandi;i++)
    //    printf(" Candi: %d: life=%d, move=%d, moved=%d\n", i+1, faceCandi[i][4], faceCandi[i][5], faceCandi[i][6]);

    // draw face boxes
    //drawface(frame);
}

// 消除嵌套的小脸
void CFaceLivenessDetect::filterFace()
{
    int i, j, k;
    int x1, y1, w1, h1, x2, y2, w2, h2;
    // 将每张脸与其后的脸比较，较小且中心落到另一张脸内的脸将被标记为假脸
    for (i=0; i<nFaceDetect-1; i++){
        w1=faceDetect[i][2];
        h1=faceDetect[i][3];
        x1=faceDetect[i][0]+w1/2;
        y1=faceDetect[i][1]+h1/2;        
        for (j=i+1; j<nFaceDetect; j++){
            w2=faceDetect[j][2];
            h2=faceDetect[j][3];
            x2=faceDetect[j][0]+w2/2;
            y2=faceDetect[j][1]+h2/2;            
            if (w1*h1<w2*h2 && x1>x2-w2/2 && x1<x2+w2/2 && y1>y2-h2/2 && y1<y2+h2/2){
                faceDetect[i][4] = 1;
                break;
            }
            if (w1*h1>w2*h2 && x2>x1-w1/2 && x2<x1+w1/2 && y2>y1-h1/2 && y2<y1+h1/2){
                faceDetect[j][4] = 1;
                break;
            }
        }
    }
    
    // 删除假脸
    i=0;
    while (i<nFaceDetect){
        if (faceDetect[i][4] == 1){
            // delete this false face
            for (k=i+1; k<nFaceDetect; k++){
                for (j=0; j<NUM_FACE_DETECT_WIDTH; j++){
                    faceDetect[k-1][j] = faceDetect[k][j];
                }
            }
            nFaceDetect--;
        }
        else
            i++;
    }
    
}

// 消除假、静止、照片目标
void CFaceLivenessDetect::eliminateFalse()
{
    double fr = 1, fr_move = 0;
    int i,j, k, k1;
    double x10, y10, x20, y20;
    double mindist, dist;
    int minIndex;
    
    /*/ clear proc flag 
    for (j=0; j<nFaceCandi; j++)
        faceCandi[j][6] = 0;
    /*/
    //  
    i=0;
    while (i<nFaceCandi){  // for every candidate face
        // center x,y
        x10 = faceCandi[i][0] + faceCandi[i][2]/2.0;
        y10 = faceCandi[i][1] + faceCandi[i][3]/2.0;        
        // find the nearest new face
        minIndex = -1;
        mindist = 99999999;
        for (j=0; j<nFaceDetect; j++){       
            x20 = faceDetect[j][0] + faceDetect[j][2]/2.0;
            y20 = faceDetect[j][1] + faceDetect[j][3]/2.0;
            dist = ((x10-x20)*(x10-x20)+(y10-y20)*(y10-y20));
            if (mindist > dist){
                mindist = dist;
                minIndex = j;
            }
        }
     
        // if new face j == candidate face i
        Rect r1, r2;
        if (minIndex >= 0)
            r2 = Rect(faceDetect[minIndex][0], faceDetect[minIndex][1], faceDetect[minIndex][2], faceDetect[minIndex][3]);
        else
            r2 = Rect(0, 0, 0, 0);
        r1 = Rect(faceCandi[i][0], faceCandi[i][1], faceCandi[i][2], faceCandi[i][3]);
        if (minIndex>=0 && isNear(r1, r2, fr)<100 && isSameSize(r1, r2)){ // near and similar size
            for (k=0; k<4; k++)  // update candidate face with this new face
                faceCandi[i][k] = faceDetect[minIndex][k];
            faceCandi[i][4] += candi_life_inc;  // 加血
            if (faceCandi[i][4]>candi_life_top) faceCandi[i][4] = candi_life_top;
            // if moved
            r2 = Rect(faceCandi[i][7], faceCandi[i][8], faceCandi[i][9], faceCandi[i][10]);
            int dist = isNear(r1, r2, fr_move);
            if (dist>=100)
                faceCandi[i][5]++;
            if (true || faceCandi[i][6] < dist) // save the dist for debug
                faceCandi[i][6] = dist;
            
            // delete this new face
            for (k=minIndex+1; k<nFaceDetect; k++)  
                for (k1=0; k1<NUM_FACE_DETECT_WIDTH; k1++)
                    faceDetect[k-1][k1] = faceDetect[k][k1];
            nFaceDetect--;
        }
        else{
            faceCandi[i][4] -= 1;
        }
        i++;
    }
    
    
    // 删除死框和确认框
    i=0;
    while (i<nFaceCandi){
        if (faceCandi[i][4] <= 0){ // fail tracking
            // delete this candidate face
            for (k=i+1; k<nFaceCandi; k++)
                for (k1=0; k1<NUM_FACE_CANDI_WIDTH; k1++)
                    faceCandi[k-1][k1] = faceCandi[k][k1];
            nFaceCandi--;
        }
        else{
            // 真脸确认，从备选中删除，进入跟踪
            if (faceCandi[i][4] > countT && faceCandi[i][5] > maxMoveT){
                // 进入跟踪
                appendTracker(faceCandi[i][0], faceCandi[i][1], faceCandi[i][2], faceCandi[i][3]);
                
                // delete this candidate face
                for (k=i+1; k<nFaceCandi; k++) 
                    for (k1=0; k1<NUM_FACE_CANDI_WIDTH; k1++)
                        faceCandi[k-1][k1] = faceCandi[k][k1];
                nFaceCandi--;
            }
            else // 继续备选
                i++;
        }
    }
    
    // 添加新框
    // 滤除落到备选框内的新脸
    Rect r1, r2;
    for (j=0; j<nFaceDetect; ){
        r2 = Rect(faceDetect[j][0], faceDetect[j][1], faceDetect[j][2], faceDetect[j][3]); 
        for (i=0;i<nFaceCandi; i++){
            r1 = Rect(faceCandi[i][0], faceCandi[i][1], faceCandi[i][2], faceCandi[i][3]);
            if (intersect(r1, r2)){ // 包含关系
                // delete this new face
                for (k=j+1; k<nFaceDetect; k++)  
                    for (k1=0; k1<NUM_FACE_DETECT_WIDTH; k1++)
                        faceDetect[k-1][k1] = faceDetect[k][k1];
                j--;
                nFaceDetect--;
                break;
            }
        }
        j++;
    }
    
    // 滤除落到跟踪框内的新脸
    for (j=0; j<nFaceDetect; ){
        r2 = Rect(faceDetect[j][0], faceDetect[j][1], faceDetect[j][2], faceDetect[j][3]); 
        for (i=0;i<nFaceTrack; i++){
            r1 = Rect(faceTrack[i][0], faceTrack[i][1], faceTrack[i][2], faceTrack[i][3]);
            if (intersect(r1, r2)){ // 包含关系
                // delete this new face
                for (k=j+1; k<nFaceDetect; k++)  
                    for (k1=0; k1<NUM_FACE_DETECT_WIDTH; k1++)
                        faceDetect[k-1][k1] = faceDetect[k][k1];
                j--;
                nFaceDetect--;
                break;
            }
        }
        j++;
    }
    
    for (j=0; j<nFaceDetect; j++){
        faceCandi[nFaceCandi][0] = faceDetect[j][0];
        faceCandi[nFaceCandi][1] = faceDetect[j][1];
        faceCandi[nFaceCandi][2] = faceDetect[j][2];
        faceCandi[nFaceCandi][3] = faceDetect[j][3];
        faceCandi[nFaceCandi][4] = candi_life_inc;
        faceCandi[nFaceCandi][5] = 0;
        faceCandi[nFaceCandi][6] = 0;
        faceCandi[nFaceCandi][7] = faceDetect[j][0];
        faceCandi[nFaceCandi][8] = faceDetect[j][1];
        faceCandi[nFaceCandi][9] = faceDetect[j][2];
        faceCandi[nFaceCandi][10] = faceDetect[j][3];
        nFaceCandi++;
    }
    
}

void CFaceLivenessDetect::appendTracker(int x, int y, int w, int h)
{
    nID++;
    faceTrack[nFaceTrack][0] = x;
    faceTrack[nFaceTrack][1] = y;
    faceTrack[nFaceTrack][2] = w;
    faceTrack[nFaceTrack][3] = h;
    faceTrack[nFaceTrack][4] = nID;
    faceTrack[nFaceTrack][5] = life_track;
    nFaceTrack++;    
}

// confirm lively faces 
//  (input: nFaceDetect, faceDetect, nFaceTrack, faceTrack, nFaceCandi, faceCandi)
//  (output: live_face)
void CFaceLivenessDetect::confirmFace()
{
    // to do from deep learning model
}

bool CFaceLivenessDetect::isSameSize(Rect bbox1, Rect bbox2)
{
    double diff_factor = 0.8; // 两个框的变长差异不要超过这个值
    
    // areas should not much different
    double a1 = bbox1.width * bbox1.height;
    double a2 = bbox2.width * bbox2.height;
    double minA, maxA;
    if (a1 > a2){ minA = a2; maxA = a1; }
    else        { minA = a1; maxA = a2; }
    return (minA/maxA > diff_factor*diff_factor);
    
}

int CFaceLivenessDetect::isNear(Rect bbox1, Rect bbox2, double fr)
{   
    // fr: 将第2个框四个方向扩充几倍
    /*
    double x = bbox2.x-fr*bbox2.width;
    double y = bbox2.y-fr*bbox2.height;
    double w = bbox2.width*(fr+fr+1.0);
    double h = bbox2.height*(fr+fr+1.0);
    double x1 = bbox1.x + bbox1.width/2.0;
    double y1 = bbox1.y + bbox1.height/2.0;
    return ((x1>x) && (x1<x+w) && (y1>y) && (y1<y+h));  // 第一个框的中心在扩展框内
    */
    
    
    // center of each box
    double x1 = bbox1.x + bbox1.width/2.0;
    double y1 = bbox1.y + bbox1.height/2.0;
    double x2 = bbox2.x + bbox2.width/2.0;
    double y2 = bbox2.y + bbox2.height/2.0;
    
    int distX = abs(x1-x2) *100 / (int)(bbox1.width *(fr+0.5));
    int distY = abs(y1-y2) *100 / (int)(bbox1.height*(fr+0.5));
    int dist = (distX > distY? (int)distX: (int)distY);
    
    return dist;
}

bool CFaceLivenessDetect::intersect(Rect bbox1, Rect bbox2)
{    
    // area of each box
    int a1 = bbox1.width*bbox1.height;
    int a2 = bbox2.width*bbox2.height;
    
    // (x1,y1): center of the smaller box
    double x1 = (a1<a2?  bbox1.x + bbox1.width /2.0: bbox2.x + bbox2.width /2.0);
    double y1 = (a1<a2?  bbox1.y + bbox1.height/2.0: bbox2.y + bbox2.height/2.0);
    double x2Left  = (a1>=a2? bbox1.x: bbox2.x);
    double x2Right = (a1>=a2? bbox1.x+bbox1.width:  bbox2.x+bbox2.width);
    double y2Top   = (a1>=a2? bbox1.y: bbox2.y);
    double y2Bott  = (a1>=a2? bbox1.y+bbox1.height: bbox2.y+bbox2.height);
    
    return ((x1>x2Left) && (x1<x2Right) && (y1>y2Top) && (y1<y2Bott));
}

// 根据当前帧检测到的'人脸'更新跟踪列表，并留下不属于跟踪列表的'脸'
void CFaceLivenessDetect::updateTracking()
{
    double fr = 1;
    int i,j, k, k1;
    double x10, y10, x20, y20;
    double mindist, dist;
    int minIndex;
    
    i=0;
    while (i<nFaceTrack){  // for every tracked face
        // center x,y
        x10 = faceTrack[i][0] + faceTrack[i][2]/2.0;
        y10 = faceTrack[i][1] + faceTrack[i][3]/2.0;
        x20 = faceDetect[0][0] + faceDetect[0][2]/2.0;
        y20 = faceDetect[0][1] + faceDetect[0][3]/2.0;
        // find the nearest new face
        minIndex = 0;
        mindist = ((x10-x20)*(x10-x20)+(y10-y20)*(y10-y20));
        for (j=0; j<nFaceDetect; j++){
            x20 = faceDetect[j][0] + faceDetect[j][2]/2.0;
            y20 = faceDetect[j][1] + faceDetect[j][3]/2.0;
            dist = ((x10-x20)*(x10-x20)+(y10-y20)*(y10-y20));
            if (mindist > dist){
                mindist = dist;
                minIndex = j;
            }
        }
        // if new face j == tracked face i
        Rect r1, r2;
        if (minIndex >= 0)
            r2 = Rect(faceDetect[minIndex][0], faceDetect[i][1], faceDetect[i][2], faceDetect[i][3]);
        else
            r2 = Rect(0, 0, 0, 0);        
        r1 = Rect(faceTrack[i][0], faceTrack[i][1], faceTrack[i][2], faceTrack[i][3]);        
        if (minIndex>=0 && isNear(r1, r2, fr)<100 && isSameSize(r1, r2)){ // corresponding
            for (k=0; k<4; k++)  // update tracked face with this new face
                faceTrack[i][k] = faceDetect[minIndex][k];
            faceTrack[i][5] = life_track;  // 满血
            for (k=minIndex+1; k<nFaceDetect; k++)  // delete this new face
                for (k1=0; k1<NUM_FACE_DETECT_WIDTH; k1++)
                    faceDetect[k-1][k1] = faceDetect[k][k1];
            nFaceDetect--;
            i++;
        }
        else{
            faceTrack[i][5] -= 1;
            if (faceTrack[i][5] <= 0){ // fail tracking
                // delete this tracked face
                for (k=i+1; k<nFaceTrack; k++)  // delete this new face
                    for (k1=0; k1<NUM_FACE_TRACK_WIDTH; k1++)
                        faceTrack[k-1][k1] = faceTrack[k][k1];
                nFaceTrack--;
            }
            else
                i++;
        }
    }
}


void CFaceLivenessDetect::drawface( Mat& img)
{
    const static Scalar colors[] = {
        Scalar(255,0,0), Scalar(0,0,255), Scalar(255,128,0), Scalar(255,255,0), Scalar(0,255,0),
        Scalar(0,128,255), Scalar(0,255,255), Scalar(255,0,255)
    };
    for ( size_t i = 0; i < nFaceCandi; i++ )
    {
        //printf("bbox_copy     %d/%d (x,y,w,h): %d, %d, %d, %d\n", i, nFace, facerects[i].x, facerects[i].y, facerects[i].width, facerects[i].height);
        //printf("bbox %d/%d (x,y,w,h): %d, %d, %d, %d\n", i, nFace, r.x, r.y, r.width, r.height);
        Point center;
        Scalar color = colors[0];

        rectangle( img, cvPoint(cvRound(faceCandi[i][0]), cvRound(faceCandi[i][1])),
               cvPoint(cvRound((faceCandi[i][0] + faceCandi[i][2]-1)), cvRound((faceCandi[i][1] + faceCandi[i][3]-1))),
               color, 3, 8, 0);
        /*
        center.x = cvRound((r.x + r.width*0.5));
        center.y = cvRound((r.y + r.height*0.5));
        radius = cvRound((r.width + r.height)*0.25);
        circle( img, center, radius, color, 3, 8, 0 );
        */
    }
    
    for ( size_t i = 0; i < nFaceTrack; i++ )
    {
        //printf("bbox_copy     %d/%d (x,y,w,h): %d, %d, %d, %d\n", i, nFace, facerects[i].x, facerects[i].y, facerects[i].width, facerects[i].height);
        //printf("bbox %d/%d (x,y,w,h): %d, %d, %d, %d\n", i, nFace, r.x, r.y, r.width, r.height);
        Point center;
        Scalar color = colors[1];

        rectangle( img, cvPoint(cvRound(faceTrack[i][0]), cvRound(faceTrack[i][1])),
               cvPoint(cvRound((faceTrack[i][0] + faceTrack[i][2]-1)), cvRound((faceTrack[i][1] + faceTrack[i][3]-1))),
               color, 3, 8, 0);        
    }
    
    /*
    for ( size_t i = 0; i < nFaceDetect; i++ )
    {
        //printf("bbox_copy     %d/%d (x,y,w,h): %d, %d, %d, %d\n", i, nFace, facerects[i].x, facerects[i].y, facerects[i].width, facerects[i].height);
        //printf("bbox %d/%d (x,y,w,h): %d, %d, %d, %d\n", i, nFace, r.x, r.y, r.width, r.height);
        Point center;
        Scalar color = colors[2];

        rectangle( img, cvPoint(cvRound(faceDetect[i][0]), cvRound(faceDetect[i][1])),
               cvPoint(cvRound((faceDetect[i][0] + faceDetect[i][2]-1)), cvRound((faceDetect[i][1] + faceDetect[i][3]-1))),
               color, 3, 8, 0);        
    }
    */
    
    //printf("\n");
    imshow( "result", img );
}

void CFaceLivenessDetect::detectface( Mat& img, CascadeClassifier& cascade, bool tryflip, double scale )
{
    
    int nFace;
    vector<Rect> faces;    
    Mat gray, smallImg;

    cvtColor( img, gray, COLOR_BGR2GRAY );
    double fx = 1;    
    resize( gray, smallImg, Size(), fx, fx, INTER_LINEAR );
    equalizeHist( smallImg, smallImg );

    cascade.detectMultiScale( smallImg, faces,
        scale, 2, 0
        //|CASCADE_FIND_BIGGEST_OBJECT
        //|CASCADE_DO_ROUGH_SEARCH
        |CASCADE_SCALE_IMAGE,
        Size(30, 30) );
    
    nFaceDetect = faces.size();
    for ( size_t i = 0; i < faces.size(); i++ ){
        faceDetect[i][0] = cvRound(faces[i].x);
        faceDetect[i][1] = cvRound(faces[i].y);
        faceDetect[i][2] = cvRound(faces[i].width);
        faceDetect[i][3] = cvRound(faces[i].height);
        faceDetect[i][4] = 0;
    }
    
    filterFace();
    
    return;
}

// shape: 0-rectangle, 1-circle
void CFaceLivenessDetect::drawface( Mat& img, Rect *facerects, int nFace, int shape)
{
    const static Scalar colors[] =
    {
        Scalar(255,0,0),
        Scalar(255,128,0),
        Scalar(255,255,0),
        Scalar(0,255,0),
        Scalar(0,128,255),
        Scalar(0,255,255),
        Scalar(0,0,255),
        Scalar(255,0,255)
    };
    for ( size_t i = 0; i < nFace; i++ )
    {
        Rect r = facerects[i];
        //printf("bbox_copy     %d/%d (x,y,w,h): %d, %d, %d, %d\n", i, nFace, facerects[i].x, facerects[i].y, facerects[i].width, facerects[i].height);
        //printf("bbox %d/%d (x,y,w,h): %d, %d, %d, %d\n", i, nFace, r.x, r.y, r.width, r.height);
        Point center;
        Scalar color = colors[i%8];
        int radius;

        double aspect_ratio = (double)r.width/r.height;
        //if( 0.75 < aspect_ratio && aspect_ratio < 1.3 )
        if (shape==0)
        {
            {
                rectangle( img, cvPoint(cvRound(r.x), cvRound(r.y)),
                       cvPoint(cvRound((r.x + r.width-1)), cvRound((r.y + r.height-1))),
                       color, 3, 8, 0);
            }
        }
        else{
            {
                center.x = cvRound((r.x + r.width*0.5));
                center.y = cvRound((r.y + r.height*0.5));
                radius = cvRound((r.width + r.height)*0.25);
                circle( img, center, radius, color, 3, 8, 0 );
            }
        }
    }
    //printf("\n");
    imshow( "result", img );
}

