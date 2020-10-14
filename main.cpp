#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;

static int x_offset(490), y_offset(0), w1(855), w2(220), h(175);

int main() {
    VideoCapture cap("../solidWhiteRight.mp4");

    Mat frame, res, transform;

    while(frame.empty()) {
        cap >> frame;
    }

    namedWindow("Main", WINDOW_AUTOSIZE);
    createTrackbar("x offset", "Main", &x_offset, frame.size[1]);
    createTrackbar("y offset", "Main", &y_offset, frame.size[0]);
    createTrackbar("down width", "Main", &w1, frame.size[1]);
    createTrackbar("up width", "Main", &w2, frame.size[1]);
    createTrackbar("height", "Main", &h, frame.size[0]);

    namedWindow("Result", WINDOW_AUTOSIZE);

    std::vector<std::vector<Point>> pols;
    std::vector<Point> pol;
    std::vector<Point2f> persp_dst, persp_src;
    persp_dst.push_back(Point2f(0, 0));
    persp_dst.push_back(Point2f(frame.size[1], 0));
    persp_dst.push_back(Point2f(frame.size[1], frame.size[0]));
    persp_dst.push_back(Point2f(0, frame.size[0]));
    while(true) {
        cap >> frame;

        if(frame.empty()) {
            cap.set(CAP_PROP_POS_FRAMES, 0);
            continue;
        }

        frame.copyTo(res);

        pol.clear();
        pols.clear();
        pol.push_back(Point(x_offset - w1 / 2, frame.size[0] - y_offset));
        pol.push_back(Point(x_offset + w1 / 2, frame.size[0] - y_offset));
        pol.push_back(Point(x_offset + w2 / 2, frame.size[0] - y_offset - h));
        pol.push_back(Point(x_offset - w2 / 2, frame.size[0] - y_offset - h));
        pols.push_back(pol);
        polylines(frame, pols, true, Scalar(0, 255, 0), 2);

        putText(
            frame,
            format("FPS: %3.2f", cap.get(CAP_PROP_FPS)),
            Point(10, 25),
            FONT_HERSHEY_TRIPLEX,
            0.75,
            Scalar(0, 255, 0));

        putText(
            frame,
            format("Frame:        %5.0f", cap.get(CAP_PROP_POS_FRAMES)),
            Point(10, 50),
            FONT_HERSHEY_TRIPLEX,
            0.75,
            Scalar(0, 255, 0));

        putText(
            frame,
            format("Frames total: %5.0f", cap.get(CAP_PROP_FRAME_COUNT)),
            Point(10, 75),
            FONT_HERSHEY_TRIPLEX,
            0.75,
            Scalar(0, 255, 0));

        imshow("Main", frame);

        persp_src.clear();
        persp_src.push_back(Point2f(x_offset - w1 / 2, frame.size[0] - y_offset));
        persp_src.push_back(Point2f(x_offset + w1 / 2, frame.size[0] - y_offset));
        persp_src.push_back(Point2f(x_offset + w2 / 2, frame.size[0] - y_offset - h));
        persp_src.push_back(Point2f(x_offset - w2 / 2, frame.size[0] - y_offset - h));
        transform = getPerspectiveTransform(persp_src, persp_dst);
        warpPerspective(res, res, transform, { res.size[1], res.size[0] });

        imshow("Result", res);

        if(waitKey(10) == 27)
            break; // ESC
    }

    cap.release();
    return 0;
}
