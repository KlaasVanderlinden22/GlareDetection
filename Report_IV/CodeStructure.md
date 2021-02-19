```
Glare_detection
  |- CMakeLists.txt
  |- include
  |    |- Image.h
  |    |- MatLib.h
  |    |- Queue.h
  |    |- Ellipse.h
  |    |- stb_image.h
  |    |- stb_image_resize.h
  |    |- stb_image_write.h
  |- src
  |    |- main.c
  |    |- Image.c
  |    |- MatLib.c
  |    |- Queue.c
  |    |- Ellipse.c
  |- images
 ```
The stb_image header files are the basis for loading and saving images.
The Image files defines a bunch of functions to use on images. This includes the loading of an image where it automatically allocates memory, but also converting an image to grayscale or threshold it, and extracting blobs from images.
The MatLib files are a library to work with matrix operations (needed for fitting the ellipse).
The Queue files define a 'queue' structure that can dynamically change size (needed for saving points of blobs because size is not known in advance).
The Ellipse files define the 'ellipse' struct and contain the MVEE method, the function to join ellipses and to check collisions.
