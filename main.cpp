/**
 * Sebuah program untuk membuat Virtual Background dari sebuah video
 * @author Azzam Wildan Maulana - 5024201010
 * */

#include <bits/stdc++.h>      // All std lib on cpp
#include <opencv2/opencv.hpp> // OpenCV core
#include "yaml-cpp/yaml.h"    // lib-YAML

using namespace cv;

YAML::Node config;
YAML::Node env;
std::string bg_path, cam_path;

int lowh = 0, lows = 9, lowv = 241, highh = 68, highs = 246, highv = 255;
bool tresholding;

/**
 * @brief init YAML to load yaml file on config/
 * */
void initYAML()
{
    try
    {
        config = YAML::LoadFile("config/color.yaml");
        env = YAML::LoadFile("config/env.yaml");
    }
    catch (YAML::BadFile &e)
    {
        std::cout << "read error!" << std::endl;
    }
}
/**
 * @brief load all configurations on config/color.yaml
 * */
void loadConfig()
{
    lowh = config["hue_min"].as<int>();
    highh = config["hue_max"].as<int>();
    lows = config["saturation_min"].as<int>();
    highs = config["saturation_max"].as<int>();
    lowv = config["value_min"].as<int>();
    highv = config["value_max"].as<int>();

    bg_path = env["bg_path"].as<String>();
    cam_path = env["cam_path"].as<String>();
    tresholding = env["is_tresholding"].as<int>();
}

/**
 * @brief save all configurations on config/color.yaml
 * */
void saveConfig()
{
    std::ofstream fout("config/color.yaml");

    config["hue_min"] = lowh;
    config["hue_max"] = highh;
    config["saturation_min"] = lows;
    config["saturation_max"] = highs;
    config["value_min"] = lowv;
    config["value_max"] = highv;

    fout << config << std::endl;
    fout.close();
}

int main()
{
    // Init and using YAML
    initYAML();
    loadConfig();

    // Load video
    VideoCapture kamera;
    kamera.open(cam_path);

    // Load and resize VBG
    Mat vbg;
    vbg = imread(bg_path);
    resize(vbg, vbg, Size(640, 360));

    /******************
     * UNTUK TRESHOLD *
     ******************/
    if (tresholding)
    {
        namedWindow("treshold", WINDOW_AUTOSIZE);
        createTrackbar("H-", "treshold", &lowh, 179);
        createTrackbar("H+", "treshold", &highh, 179);
        createTrackbar("S-", "treshold", &lows, 255);
        createTrackbar("S+", "treshold", &highs, 255);
        createTrackbar("V-", "treshold", &lowv, 255);
        createTrackbar("V+", "treshold", &highv, 255);
    }

    while (1)
    {
        // Resampling frame
        Mat buffer;
        kamera.read(buffer);
        resize(buffer, buffer, Size(640, 360));

        // Convert ke HSV
        Mat buffer_hsv;
        cvtColor(buffer, buffer_hsv, COLOR_BGR2HSV);

        // Treshold warna background, contoh disini menggunakan background hijau
        Mat background_mask;
        inRange(buffer_hsv, Scalar(lowh, lows, lowv), Scalar(highh, highs, highv), background_mask);

        // Membuat inverted mask, digunakan untuk meletakkan frame aslinya nanti
        Mat image_mask;
        bitwise_not(background_mask, image_mask);

        // Meletakkan frame asli sesuai masknya
        Mat original_img_buffer;
        bitwise_and(buffer, buffer, original_img_buffer, image_mask);

        // Meletakkan VBG nya
        Mat background_img_buffer;
        bitwise_and(vbg, vbg, background_img_buffer, background_mask);

        // Menggabungkan hasilnya
        Mat final_img;
        add(original_img_buffer, background_img_buffer, final_img);

        /******************
         * UNTUK TRESHOLD *
         ******************/
        if (tresholding)
        {
            imshow("raw", buffer);
            imshow("background_mask", background_mask);
            saveConfig(); // Selalu menyimpan hasil konfigurasi HSV ke config/color.yaml
        }
        else
        {
            imshow("final_img", final_img);
        }

        if (waitKey(1) == 27)
            break;
    }

    return 0;
}