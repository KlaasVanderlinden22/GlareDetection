#include "Image.h"
#include <stdio.h>
#include "MatLib.h"
#include "Ellipse.h"
#include "string.h"


int main(void) {

	// Load image 
	Image img;
	Image_load(&img, "../images/image10.jpg");

	// Resize image
	//Image resized;
	//Image_resize(&img, &resized);	
		
	// Convert image to grayscale
	Image gray_img;
	Image_to_gray(&img, &gray_img);

	// Iterate over different thresholds
	
	for(int i=255;i>=240;i-=5) {	
		printf("For threshold of %d.\n", i);
		printf("--------------------\n");
		
		// Convert grayscale image to thresholded image
		Image threshold_img;
		int threshold = i;
		Image_to_threshold(&gray_img, &threshold_img, &threshold);
			
		// Convert threshold image to blob image
		Image blobs_img;
		int nb_blobs;
		Image_to_blobs(&threshold_img, &blobs_img, &nb_blobs);
	
		printf("Number of blobs detected: %d\n", nb_blobs);

		// Loop over every blob
		for(int blob_id = 1;blob_id <= nb_blobs; blob_id++){
			
			// Get blob points	
			Mat* points = Image_getblobpoints(&blobs_img, blob_id);
		
			// Select blob that are big enough
			if (points->col < 500){
			    continue;
			}
			printf("Blob %d has %d points.\n",blob_id, points->col);

			// Fit minimum volume enclosing ellipse around points
			Ellipse E;
                	MVEE(points,&E);
			printf("Ellipse found with cx:%f, cy:%f,\n",E.cx,E.cy); 
			printf("		   theta:%f,\n", E.theta);
			printf("	      	   l1:%f, l2:%f\n", E.l1, E.l2);
			
		}

		// Save images
		char bname[21];
		snprintf(bname,20 , "blobs_image_%d.png", i);
                printf("Saved image with blobs at %s\n", bname);
                Image_save(&blobs_img, bname);
		Image_free(&blobs_img);

		char name[24];
		snprintf(name,24 , "threshold_image_%d.png", i);
		printf("Saved thresholded image at %s\n", name);	
		Image_save(&threshold_img, name);
		Image_free(&threshold_img);

		printf("\n");
	}

	// Save images
	Image_save(&gray_img, "gray_image.png");
	

	// Free memory
	Image_free(&img);
	Image_free(&gray_img);
	

}
