#include "Image.h"
#include <math.h>
#include "Queue.h"


#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../include/stb_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "../include/stb_image_resize.h"

typedef struct Point_{
   int x,y;
} Point;
    


void Image_load(Image *img, const char *fname) {
    img->data = stbi_load(fname, &img->width, &img->height, &img->channels, 0);
	printf("Image loaded with width of %dpx, height of %dpx and %d channels \n",img->width,img->height,img->channels);    
    if(img->data != NULL) {
        img->size = img->width * img->height * img->channels;
	    img->allocation_ = STB_ALLOCATED;
    }
    else {
        printf("Error in loading image\n");
        exit(1);
    }   
}

void Image_create(Image *img, int width, int height, int channels) {
    size_t size = width * height * channels;
    img->data = malloc(size);
    
    if(img->data != NULL) {
        img->width = width;
        img->height = height;
        img->size = size;
        img->channels = channels;
	    img->allocation_ = SELF_ALLOCATED;
    }
    else {
        printf("Error in allocating memory\n");
        exit(1);
    }
}

void Image_resize(const Image *orig, Image *resized) {
    Image_create(resized, orig->width/4, orig->height/4,orig->channels);    
    stbir_resize_uint8(orig->data, orig->width, orig->height, 0,
		       resized->data, resized->width, resized->height,0, resized->channels);
}


void Image_save(const Image *img, const char *fname) {
    // Check if the file name ends in one of the .jpg/.JPG/.jpeg/.JPEG or .png/.PNG
    if (!strcmp(strrchr(fname,'.'),".jpg")) {
        stbi_write_jpg(fname, img->width, img->height, img->channels, img->data, 100);
    } else if(!strcmp(strrchr(fname,'.'),".png")) {
        stbi_write_png(fname, img->width, img->height, img->channels, img->data, img->width * img->channels);
    } else {
        printf("Error in saving image\n");
    }
}

void Image_free(Image *img) {
    if(img->allocation_ != NO_ALLOCATION && img->data != NULL) {
        if(img->allocation_ == STB_ALLOCATED) {
            stbi_image_free(img->data);
        } else {
            free(img->data);
        }
        img->data = NULL;
        img->width = 0;
        img->height = 0;
        img->size = 0;
        img->allocation_ = NO_ALLOCATION;
    }
}

void Image_to_gray(const Image *orig, Image *gray) {
    int channels = orig->channels == 4 ? 2 : 1;
    Image_create(gray, orig->width, orig->height, channels);
  
    for(unsigned char *p = orig->data, *pg = gray->data; p != orig->data + orig->size; p += orig->channels, pg += gray->channels) {
        *pg = (uint8_t)((*p + *(p + 1) + *(p + 2))/3.0);
        if(orig->channels == 4) {
            *(pg + 1) = *(p + 3);
        }
    }
}

void Image_to_threshold(const Image *orig, Image *threshold, const int *threshold_value) {
    Image_create(threshold, orig->width, orig->height, orig->channels);
    
    for(unsigned char *p = orig->data, *pt = threshold->data; p != orig->data + orig->size; p += orig->channels, pt += threshold->channels) {
        if(*p <= *threshold_value) {
            *pt = (uint8_t)(0.0);
        }
	    else {
	        *pt = (uint8_t)(255.0);
	    }
    }
}

void Image_to_blobs(const Image *orig, Image *blobs, int *nb_blobs){
    Image_create(blobs, orig->width, orig->height,orig->channels);

    //Check for non zeros

    if(orig->data[0] == 0 && !memcmp(orig->data, orig->data+1, orig->size)){
        memcpy(blobs->data, orig->data, orig->size);
	printf("No non-zero elements.\n");
        return;
    }

    int i,j,k,l;
    int r = orig->height;
    int c = orig->width;
    uint8_t id = 1;
    uint8_t *pixel_ID[r];
    for(i=0;i<r;i++){
        pixel_ID[i] = (uint8_t*)calloc(c,sizeof(int));
    }
    Queue open_list;
    Queue_Init(&open_list, sizeof(Point));

    for(i=1; i<r-1; i++){
        for(j=1;j<c-1;j++){
            int index = j+c*i;
            if(orig->data[index] == 0 || pixel_ID[i][j] > 0){
                continue;
            }
            Point start = {j,i};
            Queue_Enqueue(&open_list,&start);

            while(!Queue_Empty(&open_list)){
                Point top;
                Queue_Dequeue(&open_list, &top);

                pixel_ID[top.y][top.x] = id;
                
		// Add 8-connected neighbours 
                for(k=top.y-1; k<=top.y+1; k++){
                    for(l=top.x-1; l<=top.x+1; l++){
                        int index = l+c*k;
                        if(orig->data[index] == 0 || pixel_ID[k][l] > 0){
                           continue;
                        }
                        Point next = {l,k};
                        pixel_ID[k][l] = id;
                        Queue_Enqueue(&open_list,&next);

                    }
                }
            }
	    id++;
        }
    }
       
    *nb_blobs = (int)id;

    i = 0;
    j = 0;
    for(unsigned char *pb = blobs->data; pb != blobs->data + blobs->size; pb += blobs->channels) {
	    *pb = (uint8_t)pixel_ID[i][j];
        j++;
	    if(j>=c){
	        j = 0;
	        i++;
	    }
    }
}

Mat* Image_getblobpoints(const Image *orig, int blob_id){
    int i = 0;
    int j = 0;
    Queue pointList;
    
    Queue_Init(&pointList, sizeof(Point));
    
    for(unsigned char *p = orig->data; p != orig->data + orig->size; p += orig->channels) {
        if (*p == blob_id){
	        Point next = {j,i} ;
	        Queue_Enqueue(&pointList,&next);
	    }
        j++;
        if(j>=orig->width){
            j = 0;
            i++;
        }
    }

    int k = 0;
    int len = Queue_Size(&pointList);
    
    Mat* points = newmat(2,len,0);
    while(!Queue_Empty(&pointList)){
	    Point top;
	    Queue_Dequeue(&pointList, &top);
	    points->entries[k] = top.x;
	    points->entries[k+len] = top.y;
 	    k++;
    }

    return(points);
}



