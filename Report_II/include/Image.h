#include <stdlib.h>
#include <stdint.h>
#include "MatLib.h"

enum allocation_type {
    NO_ALLOCATION, SELF_ALLOCATED, STB_ALLOCATED
};

typedef struct {
    int width;
    int height;
    int channels;
    size_t size;
    uint8_t *data;
    enum allocation_type allocation_;
} Image;

typedef struct {
    double cx;
    double cy;
    double slope;
    int min_x;
    int max_x;
    int min_y;
    int max_y;
    int n_pixels;
    int id;
} Blob;

void Image_load(Image *img, const char *fname);
void Image_create(Image *img, int width, int height, int channels);
void Image_resize(const Image *orig, Image *resized);
void Image_save(const Image *img, const char *fname);
void Image_free(Image *img);
void Image_to_gray(const Image *orig, Image *gray);
void Image_to_threshold(const Image *orig, Image *threshold, const int *threshold_value);
void Image_to_blobs(const Image *orig, Image *blobs, int *nb_blobs);
Mat* Image_getblobpoints(const Image *orig, int blob_id);
