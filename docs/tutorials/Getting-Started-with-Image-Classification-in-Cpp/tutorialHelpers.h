////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     tutorialHelpers.h
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>
#include <numeric>
#include <algorithm>
#include <opencv2/opencv.hpp>

class tutorialHelpers
{
public:
    // Prepare an image for processing by an ELL model. 
    // - Resize and center-crop to the required width and height while preserving aspect ratio.
    // - OpenCV gives the image in BGR order, if needed, re-order the channels to RGB.
    // - Convert the OpenCV result to a std::vector<float>
    static std::vector<float> PrepareImageForModel(cv::Mat& image, int requiredWidth, int requiredHeight, bool reorderToRGB = false)
    {
        cv::Mat resultImage = cv::Mat::zeros(requiredWidth, requiredHeight, image.type() );
        std::vector<float> result(resultImage.total() * resultImage.channels());
        
        // Create a region of interest that defines a center crop, keeping the initial aspect ratio
        // but whose destination dimensions are compatible with the requiredWidth and requiredHeight
        cv::Rect roi;
        if (image.rows >= image.cols)
        {
            float scale = ( (float) requiredWidth ) / image.cols;

            roi.width = image.cols;
            roi.height = (int)(image.rows * scale);
            roi.x = 0;
            roi.y = (image.rows - roi.height) / 2;
        }
        else
        {
            float scale = ( (float) requiredHeight ) / image.rows;
            
            roi.width = int(image.cols * scale);
            roi.height = image.rows;
            roi.x = (image.cols - roi.width) / 2;
            roi.y = 0;
        }
        // Crop the image to the region of interest
        cv::Mat centerCropped = image(roi);
        // Resize to the required dimensions
        cv::resize(centerCropped, resultImage, resultImage.size());
        // Re-order if needed
        if (reorderToRGB)
        {
            cv::cvtColor(resultImage, resultImage, cv::COLOR_BGR2RGB);
        }

        // Convert the cv::Mat into a vector of floats
        result.assign(resultImage.datastart, resultImage.dataend);

        return result;
     }

    // Returns up to the top N predictions (that exceed our threshold) as a vector of std::pair, 
    // where the first represents the index or class of the prediction and the second 
    // represents that probability or confidence value.
    static std::vector<std::pair<size_t, float>> GetTopNPredictions(const std::vector<float>& predictions, size_t topN = 5, double threshold = 0.20)
    {
        std::vector<std::pair<size_t, float>> result;

        // initialize original index locations
        std::vector<size_t> indexes(predictions.size());
        std::iota(indexes.begin(), indexes.end(), 0);

        // sort indexes based on comparing prediction values
        std::sort(indexes.begin(), indexes.end(), [&predictions](size_t index1, size_t index2) 
        {
            return predictions[index1] > predictions[index2]; 
        });

        // Make a std::pair and append it to the result for N predictions
        for (size_t i = 0; i < topN; i++)
        {
            if (predictions[indexes[i]] > threshold)
            {
                result.emplace_back(std::pair<size_t, float>({ indexes[i], predictions[indexes[i]] }));
            }
        }

        return result;
    }

    // Add a duration to a vector and calculate the mean duration.
    static double getMeanDuration(std::vector<double>& accumulated, double duration, size_t maxAccumulatedExntries = 30)
    {
        accumulated.emplace_back(duration);
        if (accumulated.size() > maxAccumulatedExntries) accumulated.erase(accumulated.begin());
        auto meanFunction = [&accumulated](double a, double b) {return a + b / accumulated.size(); };

        return std::accumulate(accumulated.begin(), accumulated.end(), 0.0, meanFunction);
    }

    // Helper to draw a header text block on an image
    static void drawHeader(cv::Mat& image, const std::string& text)
    {
        int blockHeight = 40;
        drawTextBlock(image, text, cv::Point(0, 0), cv::Scalar(50, 200, 50), blockHeight);
    }

    // Helper to draw a footer text block on an image
    static void drawFooter(cv::Mat& image, const std::string& text)
    {
        int blockHeight = 40;
        drawTextBlock(image, text, cv::Point(0, image.rows - blockHeight), cv::Scalar(200, 100, 100), blockHeight);
    }

    // Helper to draw a colored text block with black text inside
    static void drawTextBlock(cv::Mat& image, const std::string& text, cv::Point topLeft, cv::Scalar color, int height = 40)
    {
        double fontScale = 0.7;
        cv::rectangle(image, topLeft, cv::Point(image.cols, topLeft.y + height), color, cv::FILLED);
        cv::putText(image, text, cv::Point(topLeft.x + height / 4, topLeft.y + (int)(height * 0.667)),
                    cv::FONT_HERSHEY_COMPLEX_SMALL, fontScale, cv::Scalar(0, 0, 0), 1, cv::LINE_AA);
    }
};