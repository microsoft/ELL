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
    // Prepare an image for processing by an ELL model. Typically, this involves:
    // - Resize and center-crop to the required width and height while preserving aspect ratio.
    //   Simple resize may result in a stretched or squashed image which will affect the model's ability
    //   to classify images.
    // - OpenCV gives the image in BGR order, so we may need to re-order the channels to RGB.
    // - Convert the OpenCV result to a std::vector<float> for use with the ELL model.
    static std::vector<float> PrepareImageForModel(cv::Mat& image, int requiredWidth, int requiredHeight, bool reorderToRGB = false)
    {
        cv::Mat resultImage = cv::Mat::zeros(requiredWidth, requiredHeight, image.type());

        cv::Rect roi;
        if (image.rows > image.cols) // Tall (more rows than columns)
        {
            roi.y = (image.rows - image.cols) / 2;
            roi.height = image.cols;
            roi.x = 0;
            roi.width = image.cols;
        }
        else // Wide (more columns than rows)
        {
            roi.y = 0;
            roi.height = image.rows;
            roi.x = (image.cols - image.rows) / 2;
            roi.width = image.rows;
        }

        // Center crop the image maintaining aspect ratio
        cv::Mat centerCropped = image(roi);

        // Resize to model's requirements
        cv::resize(centerCropped, resultImage, resultImage.size());

        // Re-order color channels if needed
        if (reorderToRGB)
        {
            cv::cvtColor(resultImage, resultImage, cv::COLOR_BGR2RGB);
        }

        // Return as a vector of floats
        return { resultImage.datastart, resultImage.dataend };
     }

    // Returns the top N scores that exceed a given threshold. The result is a vector of std::pair,
    // where the first element of each pair is the index and the second is the score.
    static std::vector<std::pair<size_t, float>> GetTopN(const std::vector<float>& predictions, size_t topN = 5, double threshold = 0.20)
    {
        // initialize original index locations
        std::vector<size_t> indexes(predictions.size());
        std::iota(indexes.begin(), indexes.end(), 0);

        // sort indexes based on comparing prediction values
        std::partial_sort(indexes.begin(), indexes.begin() + topN, indexes.end(), [&predictions](size_t index1, size_t index2)
        {
            return predictions[index1] > predictions[index2];
        });

        // Make a std::pair and append it to the result for N predictions
        std::vector<std::pair<size_t, float>> result;
        for (size_t i = 0; i < topN; ++i)
        {
            if (predictions[indexes[i]] >= threshold)
            {
                result.emplace_back(indexes[i], predictions[indexes[i]]);
            }
        }

        return result;
    }

    // Add a duration to a vector and calculate the mean duration.
    static double GetMeanDuration(std::vector<double>& accumulated, double duration, size_t maxAccumulatedEntries = 30)
    {
        accumulated.emplace_back(duration);

        if (accumulated.size() > maxAccumulatedEntries)
        {
            accumulated.erase(accumulated.begin());
        }

        auto meanFunction = [&accumulated](double a, double b) { return a + b / accumulated.size(); };

        return std::accumulate(accumulated.begin(), accumulated.end(), 0.0, meanFunction);
    }

    // Draw a colored text block with black text inside
    static void DrawTextBlock(cv::Mat& image, const std::string& text, cv::Point topLeft, cv::Scalar color, int height = 40)
    {
        const double fontScale = 0.7;

        cv::rectangle(image, topLeft, cv::Point(image.cols, topLeft.y + height), color, CV_FILLED);

        cv::putText(image, text, cv::Point(topLeft.x + height / 4, topLeft.y + (int)(height * 0.667)),
                    cv::FONT_HERSHEY_COMPLEX_SMALL, fontScale, cv::Scalar(0, 0, 0), 1, CV_AA);
    }

    // Draw a header text block on an image
    static void DrawHeader(cv::Mat& image, const std::string& text)
    {
        const int blockHeight = 40;

        DrawTextBlock(image, text, cv::Point(0, 0), cv::Scalar(50, 200, 50), blockHeight);
    }

    // Draw a footer text block on an image
    static void DrawFooter(cv::Mat& image, const std::string& text)
    {
        const int blockHeight = 40;

        DrawTextBlock(image, text, cv::Point(0, image.rows - blockHeight), cv::Scalar(200, 100, 100), blockHeight);
    }
};
