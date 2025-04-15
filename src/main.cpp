#include <iostream>
#include <cmath>
#include <string>
#include <filesystem>
#include <stdio.h>
#include <sstream>
#include <string_view>

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect/aruco_detector.hpp>

#include <podofo/podofo.h>

int main(int argc, char* argv[]){
    std::cout << "\nGenerating ArUcoMarkers!\n\n";

    double imageScaleXY{0.35};
    double pdfMarginSize{35.0};

    auto pageSize = PoDoFo::PdfPageSize::A4;

    float mmToPixel{3.7f};
    float mmSize{30.0f};
    int sidePixels = 240; //(int) std::floorf(mmToPixel*mmSize);
    int borderBits = 1;
    int backgroundPadding = 80;

    int marginBorderBits = 15;

    int markerImageSize = sidePixels+backgroundPadding+marginBorderBits;

    cv::Mat markerImageBackGround(markerImageSize, markerImageSize, CV_8UC1, cv::Scalar(255));

    cv::Mat markerImage;
    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
    std::filesystem::path dataRootDir("output");
    std::filesystem::create_directories(dataRootDir);

    std::vector<std::filesystem::path> imagePaths;

    for(int id = 0; id < 50; id++){
        cv::aruco::generateImageMarker(dictionary, id, sidePixels, markerImage, borderBits);

        char buffer[100];
        sprintf_s(buffer, 100, "%03d", id);
        std::ostringstream out("");
        out << "marker_4x4_" << buffer << ".png";


        std::filesystem::path filePath = dataRootDir / out.str();
        std::cout << "generating " << filePath.string() << "\n";

        imagePaths.push_back(filePath);

        cv::Mat outImage = markerImageBackGround.clone();
        markerImage(cv::Range::all(), cv::Range::all()).copyTo(
            outImage(cv::Range(marginBorderBits, marginBorderBits+sidePixels), cv::Range(marginBorderBits,marginBorderBits+sidePixels))
        );

        cv::Point text_position(markerImageSize-backgroundPadding+5, markerImageSize-10);
        int font_size = 3; 
        cv::Scalar font_Color(0); 
        int font_weight = 6; 
        cv::putText(outImage, std::to_string(id), text_position, cv::FONT_HERSHEY_PLAIN, font_size, font_Color, font_weight);
   

        cv::imwrite(filePath.string(), outImage);
    }
    
    // write images to pdf
    std::filesystem::path pdfFilePath = dataRootDir / "aruco_markers.pdf";
    PoDoFo::PdfMemDocument document;
    PoDoFo::PdfPainter painter;

    double paintPositionX{0.0};
    double paintPositionY{0.0};

    try
    {
        double pageWidth{0.0};
        double pageHeight{0.0};
        {
            PoDoFo::PdfPage& page = document.GetPages().CreatePage(PoDoFo::PdfPage::CreateStandardPageSize(pageSize));
            pageWidth = page.GetRectRaw().Width;
            pageHeight = page.GetRectRaw().Height;
            painter.SetCanvas(page);
            // reset paint positions
            paintPositionX = pdfMarginSize;
            paintPositionY = pageHeight - pdfMarginSize;
        }
        
        try
        {
            size_t counter_k{0};
            for(auto& imagePath: imagePaths){
                std::unique_ptr<PoDoFo::PdfImage> img = document.CreateImage();
                img->Load(imagePath.string().c_str());
                double imgWidth = img->GetWidth()*imageScaleXY;
                double imgHeight = img->GetHeight()*imageScaleXY;

                // checks if image will lie outside the page width 
                if(paintPositionX+imgWidth+pdfMarginSize > pageWidth){                   
                    paintPositionX = pdfMarginSize;
                    paintPositionY -= imgHeight;
                }
                // check if image will lie outside the page height
                if(paintPositionY-imgHeight-pdfMarginSize < pdfMarginSize){
                    // create a new page
                    PoDoFo::PdfPage& page = document.GetPages().CreatePage(PoDoFo::PdfPage::CreateStandardPageSize(pageSize));
                    pageWidth = page.GetRectRaw().Width;
                    pageHeight = page.GetRectRaw().Height;
                    painter.SetCanvas(page);
                    // reset paint positions
                    paintPositionX = pdfMarginSize;
                    paintPositionY = pageHeight - pdfMarginSize;
                }
                painter.DrawImage(*img.get(), paintPositionX, (paintPositionY-imgHeight), imageScaleXY, imageScaleXY);
                // draw vertical lines one either side of the image
                painter.DrawLine(paintPositionX,            (paintPositionY-imgHeight), paintPositionX,             paintPositionY);
                painter.DrawLine(paintPositionX+imgWidth,   (paintPositionY-imgHeight), paintPositionX+imgWidth,    paintPositionY);
                // draw horizontal lines above and below the image
                painter.DrawLine(paintPositionX,   paintPositionY,             paintPositionX+imgWidth,    paintPositionY);
                painter.DrawLine(paintPositionX,   (paintPositionY-imgHeight), paintPositionX+imgWidth,    (paintPositionY-imgHeight));

                paintPositionX += imgWidth;
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << "image writer error: " << e.what() << '\n';
            painter.FinishDrawing();
        }

        painter.FinishDrawing();
        // Set some additional information on the PDF file.
        document.GetMetadata().SetCreator(PoDoFo::PdfString("na"));
        document.GetMetadata().SetAuthor(PoDoFo::PdfString("na"));
        document.GetMetadata().SetTitle(PoDoFo::PdfString("AruCo Markers"));
        document.GetMetadata().SetSubject(PoDoFo::PdfString("AruCo Markers"));
        document.GetMetadata().SetKeywords(std::vector<std::string>({ "AruCo", "OpenCV" }));
        document.Save(std::string_view(pdfFilePath.string()));      
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
