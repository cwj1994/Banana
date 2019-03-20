// v1.2.1@20180325

#include "FaceLivenessDetect.h"


static void help()
{
    cout << "\nThis program demonstrates the fast detection of face_liveness.\n"
    "During execution:\n\tHit any key to quit.\n"
    "\tUsing OpenCV version " << CV_VERSION << "\n" << endl;
}

void display( Mat& img, CFaceLivenessDetect& fld);

int main( int argc, const char** argv )
{
    VideoCapture capture;
    Mat frame, image;
    string inputName;
    bool tryflip;
    CascadeClassifier cascade;
    double scale;
    string cascadeName;
    
    CFaceLivenessDetect fld; // fld means face liveness detection
    
    // read command line parameters
    cv::CommandLineParser parser(argc, argv,
        "{help h||}"
        "{cascade| detection_model|}"
        "{scale|1|}{try-flip||}{@filename||}"
    );
    if (parser.has("help"))
    {
        help();
        return 0;
    }
    cascadeName = parser.get<string>("cascade");
    scale = parser.get<double>("scale");
    if (scale < 1)
        scale = 1;
    tryflip = parser.has("try-flip");
    inputName = parser.get<string>("@filename");
    if (!parser.check())
    {
        parser.printErrors();
        return 0;
    }
    
    // if using deep learning face detector then we can skip this
    // load classifier    
    if( !cascade.load( cascadeName ) )
    //if( !cascade.load( "detection_model.xml" ) )
    {
        cerr << "ERROR: Could not load model" <<": " << cascadeName << endl;
        help();
        return -1;
    }
    
    // open camera
    if( inputName.empty() || (isdigit(inputName[0]) && inputName.size() == 1) )
    {
        int camera = inputName.empty() ? 0 : inputName[0] - '0';
        //
        if(!capture.open(camera))
        //if(!capture.open("testvideo.avi"))
            cout << "Capture from camera #" <<  camera << " didn't work" << endl;
    }

    // start work    
    double t;
    if( capture.isOpened() )  // using camrea
    {
        cout << "Video capturing has been started ..." << endl;

        for(;;)
        {
            // running speed
            t = (double)getTickCount();
            
            // read a new frame
            capture >> frame;
            if( frame.empty() )
                break;

            // Detect face using Deep Learning Model - for ruiwei
            // TBD do faces -> setInitFace
            
            // transform format (Bbox) to vector
            vector<struct BoundingBox> faces; // for example
            
            // transfer faces to fu
            int nAvailableFace = fld.setInitFace(faces);
            printf( "avalable init faces = %d \n", nAvailableFace);
            
			// find faces go back to face detect cpp file
           // findFace( frame, cascade, tryflip, scale);
            fld.findFace();
            
            // running speed
            t = (double)getTickCount() - t;
            printf( "detection time = %g ms\n", t*1000/getTickFrequency());

            display(frame, fld);
            
            char c = (char)waitKey(10);
            if( c == 27 || c == 'q' || c == 'Q' )
                break;
        }
    }

    return 0;
}

void display( Mat& img, CFaceLivenessDetect& fld)
{
    const static Scalar colors[] = {
        Scalar(0,255,255), Scalar(255,0,0), Scalar(0,0,255), Scalar(0,255,0),
        Scalar(255,128,0), Scalar(0,128,255), Scalar(255,0,255)
    };
    size_t i;
    struct BoundingBox bbox;
    
    int type = 1;
    for ( i = 0; i < fld.getFaceNum(type); i++ )
    {
        Point center;
        Scalar color = colors[type];
        bbox = fld.getFaceDetect(i, type);
        rectangle( img, cvPoint(cvRound(bbox.left), cvRound(bbox.top)),
               cvPoint(cvRound(bbox.right), cvRound(bbox.bottom)),
               color, 3, 8, 0);
    }
    
    type = 2;
    for ( i = 0; i < fld.getFaceNum(type); i++ )
    {
        Scalar color = colors[type];
        bbox = fld.getFaceDetect(i, type);
        rectangle( img, cvPoint(cvRound(bbox.left), cvRound(bbox.top)),
               cvPoint(cvRound(bbox.right), cvRound(bbox.bottom)),
               color, 3, 8, 0);     
    }
   
    imshow( "result", img );
}

