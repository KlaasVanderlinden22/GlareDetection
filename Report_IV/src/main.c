#include "Image.h"
#include <stdio.h>
#include "MatLib.h"
#include "Ellipse.h"
#include "string.h"
#include "math.h"


int main(void) {

	// Load image 
	Image img;
	Image_load(&img, "../images/image.jpg");

	// Resize image
	//Image resized;
	//Image_resize(&img, &resized);	
		
	// Convert image to grayscale
	Image gray_img;
	Image_to_gray(&img, &gray_img);

	// Iterate over different thresholds
	
	for(int i=255;i>=200;i-=10) {		
		printf("--------------------\n");
		printf("For threshold of %d.\n", i);
		printf("--------------------\n");
		printf("\n");
		
		// Convert grayscale image to thresholded image
		Image threshold_img;
		int threshold = i;
		Image_to_threshold(&gray_img, &threshold_img, &threshold);
		
		
		printf("Blobs\n");
		printf("--------------------\n");
			
		// Convert threshold image to blob image
		Image blobs_img;
		int nb_blobs = 0;
		Image_to_blobs(&threshold_img, &blobs_img, &nb_blobs);
	
		printf("Number of blobs detected: %d\n", nb_blobs);
		
		if (nb_blobs == 0){
			printf("\n");
			continue;
		}

		// Loop over every blob
		Ellipse ellipses_all[nb_blobs];
		int ellipse_id =0;
		
		int pixelsThreshold = 100;
		printf("Size threshold is %d pixels.\n", pixelsThreshold);
		
		for(int blob_id = 1; blob_id <= nb_blobs; blob_id++){
			
			
			// Get blob points	
			Mat* pixels = Image_getblobpixels(&blobs_img, blob_id);
		
			// Select blob that are big enough
			
			if (pixels->col < pixelsThreshold){
			    continue;
			}
			printf("Blob %d has %d points.\n",blob_id, pixels->col);
			
			// Fit minimum volume enclosing ellipse around points
			Ellipse E;
			E.id = blob_id;
			E.pixels = pixels;
                	MVEE(&E);
			//showellipse(&E);
			
			ellipses_all[ellipse_id] = E;
			ellipse_id++;	
		}
		
		//Shorten array to only contain useful data
		Ellipse ellipses[ellipse_id];
		memcpy(&ellipses,&ellipses_all,sizeof(ellipses));
		
		
		printf("\n");
		printf("Ellipses\n");
		printf("--------------------\n");
				
		
		size_t length = sizeof(ellipses)/sizeof(ellipses[0]);
		for (int k = 0; k < length; k++){
			showellipse(&ellipses[k]);
		}
		
		
		
		// Join blobs belonging to same object
		printf("\n");
		printf("Attempting to join ellipses\n");
		printf("--------------------\n");
		
		joinellipses(ellipses,&length);
		
		
		//Check if ellipse cross eachother --> one object on top of the other
		printf("\n");
		printf("Checking if ellipses overlap\n");
		printf("--------------------\n");
		
		EllipseCollideTest test;
		overlap(&test, ellipses, length);
		
		
		//Final overview		
		printf("\n");
		printf("Overview\n");
		printf("--------------------\n");
		
		
		printf("Number of objects is %ld \n\n", length);
		for (int k = 0; k < length; k++){
			showellipse(&ellipses[k]);
		}
		
		// Save images
		
		char name[24];
		snprintf(name,24 , "threshold_image_%d.png", i);
		printf("Saved thresholded image at %s\n", name);	
		Image_save(&threshold_img, name);
		Image_free(&threshold_img);
		
		char bname[21];
		snprintf(bname,20 , "blobs_image_%d.png", i);
		printf("Saved image with blobs at %s\n", bname);
		Image_save(&blobs_img, bname);
		Image_free(&blobs_img);


		printf("\n");
	}

	// Save images
	Image_save(&gray_img, "gray_image.png");
	

	// Free memory
	Image_free(&img);
	Image_free(&gray_img);
	

}
