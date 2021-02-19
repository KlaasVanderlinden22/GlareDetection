# Report II

```
Klaas Vanderlinden
```
## Summary of method
This algorithm attempts to recognize where a metal bar is located and how it is oriented based on its reflective properties, i.e. the 'glare' seen by the camera.

It works by dynamically lowering a threshold for a grayscale image. Throughout this successive lowering of the threshold, we expect that glare produced by the metal bars to be a specific shape, i.e. a long ellipse. We stop when the fitted ellipse corresponds to the shape we expect. The position and orientation of the fitted ellipse then correspond to those of the metal bar.

## Association relations 
* First association = glare is bright
→ threshold (pre-processing)
* Second association = glare is continuous
→ blob detection (pre-processing ←→ sensor feature)
* Third association = glare is ellipse
→ fit minimum volume enclosing ellipse and obtain center and pose (sensor feature ←→ object sensor feature)
* Fourth association = glare of metal bars is long ellipse
→ filter out blobs that don't fit the shape we expect (object sensor feature ←→ object feature)

### First association
The **first association relation** used in this algorithm is the fact that the 'glare' consists of pixels with a high intensity. Therefore, we transform the image to grayscale, which is a good representation of the intensity of the image, and threshold it. 

The threshold value we choose dynamically lower until our shapes match so it is not a *magic number* here. The step by which we lower the threshold and the lower limit of the are magic numbers.

The lower limit of the threshold is related the lighting condition. When lighting is bad, the limit should be lower because glare is not as strong.
In this case, the limit for the lighting condition in the image is determined experimentally by looking for which threshold the noise becomes too large too handle. This resulted in a lower limit of 240.

```c
int LOWER_LIMIT_THRESHOLD = 240;
```

The threshold step is a trade-off between accuracy and speed. Thus, the threshold step should not be too small (worst case it is 1) because then it requires a lot of calculations to determine the ellipse of every blob while not introducing much benefit because the blobs don't change much for a small change in the threshold value. It also should not be too large (worst case is 255) because the lower the threshold, the more noise we introduce and the larger the threshold step, the faster we introduce the noise.

The threshold step is chosen to be 5 because it seems to be a good balance of accuracy and speed.
```c
int THRESHOLD_STEP = 5;
```

### Second association
The **second association relation** used in this algorithm is the fact that the 'glare' should be continuous. Therefore, we detect 'blobs' which are pixels of the same value that connect to each other by a 8-way fashion. 

No magic numbers involved here.

### Third association
The **third association relation** used in this algorithm is the fact that the 'glare' should be an ellipse. Therefore, we fit an ellipse around the points of the blobs we have detected.

The fitting is an optimization problem and only stops when the error is less than a certain tolerance.
This tolerance is a magic number and it can be seen as a measure of accuracy of the position of the center and the length of the axis.
It is set to 0.001 which roughly means the values it produces are accurate to the third decimal place. This should be sufficiently accurate for this application.

```c
int TOLERANCE = 0.001;
```


### Fourth association
The **fourth association relation** used in this algorithm is the fact that the 'glare' of an metal bar should be a long ellipse. Therefore, we compare the ellipses we have fitted to the desired shape we want to detect.

The shape of an ellipse can be determined by knowing the size of its two axis. [See this image.](https://cookierobotics.com/007/radii_rotation.png)
In this case, we desire one long axis and a small aspect ratio.
```math
    \lambda_1 > {\lambda_1}_{threshold}
```
```math
    AR = \frac{\lambda_2}{\lambda_1} < AR_{threshold}
```

The magic numbers are the number of pixels (i.e. size of the ellipse), the length of the largest axis of the ellipse (i.e. how long in absolute terms) and the aspect ratio of the two axes (i.e. how long in relative terms).

The number of pixels is used to filter out the noise and select blob which have the potential to be possible 'glare' blobs. This number is determined experimentally by looking how big the blob that corresponds to the 'glare' was. It was possible to speak about a 'glare' blob when the number of pixels is higher than 500.
```c
int NB_OF_PIXELS = 500;
```

The threshold for length of the largest axis is determined experimentally by looking how long the axis of the fitted ellipse that correponds to the 'glare' is. This is a benchmark for all other pictures. This value is not yet determined.
```c
int L1_THRESHOLD = ...;
```

The threshold for aspect ratio of the two axes is determined experimentally by looking how what the aspect ratio of the fitted ellipse that correponds to the 'glare' is. This is a benchmark for all other pictures. This value is not yet determined.
```c
int AR_THRESHOLD = ...;
```


## Processing image
First we load in the image and concert it to grayscale. We use [std_image](https://solarianprogrammer.com/2019/06/10/c-programming-reading-writing-images-stb_image-libraries/).
```c
//Load image
Image img;
Image_load(&img);

//Concert to grayscale
Image gray
Image_to_gray(&img, &gray);
Image_free(&img);
```
## Algorithm
The algorithm is a while loop that runs until the shapes of the minimum volume enclosing ellipses are in line with our expectations of the shapes. If the shapes are not found, we lower the threshold.
```cpp
//Initialise
bool shapesFound = false;
int threshold_value = 255;

//Magic numbers
int LOWER_LIMIT_THRESHOLD = 240;
int THRESHOLD_STEP = 5;t
double TOLERANCE = 0.001;
double L1_THRESHOLD = ;
double AR_THRESHOLD = ;

//Algorithm
while(!shapesFound){
    //Thresholding
    ...
    //Blob detection
    ...
    //Loop over every blob
        //Fitting MVEE
        ...
        //Comparing MVEE to desired shape
        ...
    //Lowering threshold
    ...
}
```
### Thresholding
We threshold the image so every pixel below the threshold is set black and every pixel above is set to white.
```c
//Thresholding
Image_to_threshold(&gray,&threshold,&threshold_value);
```
### Blob detection
In this [blob detection algorithm](https://2020.robotix.in/tutorial/imageprocessing/blob_detection/) ,we define a blob as a group of white pixels where every pixel is the 8-connected neighbours of one of the other pixels. 

We already filter based on the number of pixels in a blob to reduce noise.


```c
//Blob detection
Image blobs;
int nb_blobs=0;
Image_to_blobs(&threshold, &min_size_blob, &blobs, &nb_blobs);
```
### Fitting MVEE
We fit the ellipse around all the points of each blob using the [Khachiyan's method](https://www.google.com/url?sa=t&source=web&rct=j&url=https://stackoverflow.com/questions/1768197/bounding-ellipse/1768440%231768440&ved=2ahUKEwij6M66lPLsAhXwSxUIHfzYAaQQ4-4CMAN6BAgLEAk&usg=AOvVaw1nr8Mhh5su0Se283P7fIL3).
```c
//Allocate memory for shapes
Ellipse* shapes = (Ellipse*)malloc(nb_blobs*sizeof(Ellipse));

// Loop over every blob
for(int blob_id = 1; blob_id <= nb_blobs; blob_id++) {
    //Get points of a blob
    Mat* points;          
    Image_getblobpoints(&blobs, blob_id, &points);
    
    //Fit MVEE around blob
    if(points->col < nb_points_threshold){
        continue;
    }
    Ellipse ellipse;
    mvee(&points, &ellipse);
        
    ...
    
```
With
```c
typedef struct {
    int row;
    int col;
    double* data;
} Mat;
```
And
```c
typedef struct {
    double center[];
    Mat A;
    double l1;
    double l2;
    double theta;
} Ellipse;
```

### Comparing MVEE to desired shape
We compare the ellipse we found to the shape requirements we have set.

```cpp
    ...
    
    //Comparing MVEE to desired shape
    bool shapeMatch = compare(&ellipse, &radius1, &aspect_ratio_radii);
    
    // If shape doesn't match, shape not found
    if(!shapeMatch){
        shapesFound = false;
        break;
    }
    
    // If shape matches, add to list
    shapes[blob_id] = ellipse;
    Ellipse_free(&ellipse);
    
    // If all shapes matches, shapes are found
    if(blob_id==nb_blobs){
        shapesFound = true;
    }
}
//End for loop
```

### Lowering threshold
When shapes are not found, we lower the threshold and try again.
```c
//Lowering threshold 
if(!shapesFound){
    threshold_value -= threshold_step;
    free(shapes); 
    Image_free(&threshold);   
    Image_free(&blobs);
}   
```
## Post-processing
```c
//Save image
Image_save("threshold.jpg", threshold);

//Free memory
Image_free(&threshold);   
Image_free(&blobs);
```



