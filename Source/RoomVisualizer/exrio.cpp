#include "RoomVisualizer.h"
#include "ImfRgbaFile.h"
#include "ImfArray.h"
#include "ImfHeader.h"
#include "ImfRgba.h"
#include "rgbe.h"

using namespace std;
using namespace Imf;
using namespace Imath;

bool writeExrImage(const string& filename,
	const float* image,
	int width,
	int height,
	int channels)
{
	Rgba* pixels = new Rgba[width*height];
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			int idx = j + (height - i - 1)*width;
			pixels[idx].r = image[channels*idx];
			if (channels > 1) {
				pixels[idx].g = image[channels*idx + 1];
				pixels[idx].b = image[channels*idx + 2];
			}
			else {
				pixels[idx].g = image[idx];
				pixels[idx].b = image[idx];
			}
			if (channels > 3) {
				pixels[idx].a = image[channels*idx + 3];
			}
			else {
				pixels[idx].a = 1;
			}
		}
	}
	RgbaOutputFile file(filename.c_str(), width, height, WRITE_RGBA);
	file.setFrameBuffer(pixels, 1, width);
	file.writePixels(height);
	delete pixels;
	return true;
}


bool readExrImage(const string& filename, float*& image, int& width, int& height) {
	RgbaInputFile f(filename.c_str());
	Box2i dw = f.dataWindow();
	width = dw.max.x - dw.min.x + 1;
	height = dw.max.y - dw.min.y + 1;
	Array2D<Rgba> pixels;
	pixels.resizeErase(height, width);
	f.setFrameBuffer(&pixels[0][0] - dw.min.x - dw.min.y * width, 1, width);
	f.readPixels(dw.min.y, dw.max.y);
	image = new float[3 * width*height];
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			int idx = width*(height - i - 1) + j;
			image[3 * idx] = pixels[i][j].r;
			image[3 * idx + 1] = pixels[i][j].g;
			image[3 * idx + 2] = pixels[i][j].b;
		}
	}
	return true;
}

void* readHdrImage(const string& filename, float*& image, int& width, int& height) {
	rgbe_header_info info;
	FILE* file = fopen(filename.c_str(), "rb");

	RGBE_ReadHeader(file, &width, &height, &info);
	image = new float[3 * width*height];
	RGBE_ReadPixels_RLE(file, image, width, height);
	float expadj = info.exposure;
	for (int i = 0; i < width*height * 3; ++i) {
		image[i] /= expadj;
	}
	// Flip y-coordinate (standard is 0,0 at top)
	float* row = new float[3 * width];
	for (int i = 0; i < height / 2; ++i) {
		memcpy(row, image + 3 * (i*width), 3 * width*sizeof(float));
		memcpy(image + 3 * (i*width), image + 3 * ((height - i - 1)*width), 3 * width*sizeof(float));
		memcpy(image + 3 * ((height - i - 1)*width), row, 3 * width*sizeof(float));
	}
	delete[] row;
	fclose(file);
	return image;
}