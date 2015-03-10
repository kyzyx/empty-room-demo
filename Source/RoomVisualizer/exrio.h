#pragma once
#include <string>

bool writeExrImage(const std::string& filename,
	const float* image,
	int width,
	int height,
	int channels);

bool readExrImage(const std::string& filename, float*& image, int& width, int& height);
void* readHdrImage(const std::string& filename, float*& image, int& width, int& height);