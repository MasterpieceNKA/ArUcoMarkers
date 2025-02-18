#include <iostream>
#include <cmath>
#include <string>
#include <filesystem>
#include <stdio.h>
#include <sstream>

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect/aruco_detector.hpp>

int main(int, char**){
    std::cout << "\nGenerating ArUcoMarkers!\n\n";

    float mmToPixel{3.7f};
    float mmSize{35.0f};
    int sidePixels = 240; //(int) std::floorf(mmToPixel*mmSize);
    int borderBits = 1;
    int backgroundPadding = 80;

    cv::Mat markerImageBackGround(sidePixels+backgroundPadding, sidePixels+backgroundPadding, CV_8UC1, cv::Scalar(255));

    cv::Mat markerImage;
    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
    std::filesystem::path dataRootDir("output");
    for(int id = 0; id < 50; id++){
        cv::aruco::generateImageMarker(dictionary, id, sidePixels, markerImage, borderBits);

        char buffer[100];
        sprintf_s(buffer, 100, "%03d", id);
        std::ostringstream out("");
        out << "marker_4x4_" << buffer << ".png";


        std::filesystem::path filePath = dataRootDir / out.str(); 
        std::cout << "generating " << filePath.string() << "\n";

        cv::Mat outImage = markerImageBackGround.clone();
        markerImage(cv::Range::all(), cv::Range::all()).copyTo(outImage(cv::Range(0, sidePixels), cv::Range(0,sidePixels)));

        cv::Point text_position(sidePixels+5, sidePixels+backgroundPadding-10);
        int font_size = 3; 
        cv::Scalar font_Color(0); 
        int font_weight = 6; 
        cv::putText(outImage, std::to_string(id), text_position, cv::FONT_HERSHEY_PLAIN, font_size, font_Color, font_weight);
   

        cv::imwrite(filePath.string(), outImage);
    }    

    return 0;
}
