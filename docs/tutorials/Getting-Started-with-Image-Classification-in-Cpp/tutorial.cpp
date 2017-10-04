////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     tutorial.cpp
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <fstream>
#include <sstream>
#include <chrono>
#include <opencv2/opencv.hpp>

// Include the model interface file for the ELL model we compiled
#include "model.i.h"

// Include a helper that has useful functions such as preparing data for the model
#include "tutorialHelpers.h"

// Helper function to get an image from the camera using OpenCV
static cv::Mat getImageFromCamera(cv::VideoCapture& camera)
{
    cv::Mat frame;
    camera >> frame;
    return frame;
}

// Helper function to get an image from file using OpenCV
static cv::Mat getImageFromFile(const std::string& filename)
{
    return cv::imread(filename);
}

// Return a vector of strings corresponding to the model's recognized categories or classes.
// The order of the strings in this file are expected to match the order of the
// model's output predictions.
static std::vector<std::string> getCategoriesFromFile(const std::string& filename)
{
    std::vector<std::string> categories;

    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line))
    {
        if (line.length() > 0) categories.emplace_back(line);
    }

    return categories;
}

int main(int argc, char** argv )
{
    if ( argc < 2 )
    {
        printf("usage: tutorial categories.txt <imageFile>\n");
        return -1;
    }
    // Open the video camera. To use a different camera, change the camera index.
    cv::VideoCapture camera(0);

    // Read the category labels
    auto categories = getCategoriesFromFile(argv[1]);

    // Get the model's input dimensions. We'll use this information later to resize images appropriately.
    TensorShape inputShape;    
    model_GetInputShape(0, &inputShape);

    // Create a vector to hold the model's output predictions
    std::vector<float> predictions(model_GetOutputSize());

    // Declare a value to hold the prediction times
    std::vector<double>  predictionTimes;
    double meanTimeToPredict = 0.0;

    while (cv::waitKey(1) == 0xFF)
    {
        // Get an image from the camera. If you'd like to use a different image, change this to call getImageFromFile,
        // or load the image from some other source.
        cv::Mat image = getImageFromCamera(camera);

        // Prepare an image for processing. 
        // - Resize and center-crop to the required width and height while preserving aspect ratio.
        // - OpenCV gives the image in BGR order, if needed, re-order the channels to RGB.
        // - Convert the OpenCV result to a std::vector<float>
        auto input = tutorialHelpers::PrepareImageForModel(image, inputShape.columns, inputShape.rows);

        auto start = std::chrono::steady_clock::now();        
        // Get the predicted classes using the model's predict function on the image input data. 
        // The predictions are returned as a vector with the probability that the image
        // contains the class represented by that index.
        model_predict(input, predictions);
        auto end = std::chrono::steady_clock::now();

        // Get the value of the top 5 predictions and their index, which represents the top five most 
        // confident matches and the class or category they belong to.
        auto top5 = tutorialHelpers::GetTopNPredictions(predictions, 5);

        // Draw header text that represents the top5 predictions
        std::stringstream headerText;
        for (auto element : top5)
        {
            headerText << "(" << std::floor(element.second * 100.0) << "%) " << categories[element.first] << "  ";
        }
        tutorialHelpers::drawHeader(image, headerText.str());

        // Draw footer text representing the mean evaluation time
        std::stringstream footerText;
        meanTimeToPredict = std::floor(tutorialHelpers::getMeanDuration(predictionTimes, std::chrono::duration<double>(end - start).count()) * 1000);
        footerText << meanTimeToPredict << "ms/frame";
        tutorialHelpers::drawFooter(image, footerText.str());

        // Display the image using opencv
        cv::imshow("ELL model", image);
    };

    std::cout << "Mean time to predict: " << meanTimeToPredict << "ms/frame" << std::endl;

    return 0;
}