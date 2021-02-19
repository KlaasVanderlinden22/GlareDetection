# Report III

## Optimize the glare algorithm
As of now, the glare algorithm is able to recognize glares and fit a minimum volume enclosing ellipse around them. More details can be found [here](https://gitlab.kuleuven.be/rob-ecs/2020/arcs2020/-/blob/master/report_2/software/Arm-SW-perception/Glare_detection/Report_II.md).

Still a few problems remains, which will be the subject of Report III and IV.

### Determining the shape we want to recognize

The first addition should be determining what characteristics of the ellipse should have. The association relations for this were already mentioned in Report II but I will repeat them here.


>The shape of an ellipse can be determined by knowing the size of its two axis. [See this image.](https://cookierobotics.com/007/radii_rotation.png) In this case, we desire one long axis and a small aspect ratio.

```math
\lambda_1 > {\lambda_1}_{threshold}
```

```math
AR = \frac{\lambda_2}{\lambda_1} < AR_{threshold}
```

>The threshold for length of the largest axis is determined experimentally by looking how long the axis of the fitted ellipse that correponds to the 'glare' is. This is a benchmark for all other pictures. This value is not yet determined.

>The threshold for aspect ratio of the two axes is determined experimentally by looking how what the aspect ratio of the fitted ellipse that correponds to the 'glare' is. This is a benchmark for all other pictures. This value is not yet determined.


One goal of this Report is to experiment with other metal objects (like clamps),  
just to have a "data set" that can be used later, or by other students  
now doing their master thesis around "intelligent vision", as suggested by prof. Bruyninckx.

It is important to have a good and consistent set-up so we can compare different objects and their glare in the same lighting condition.

The set-up proposed is quite simple and explained in this picture.

<figure float="left">
<img src="https://drive.google.com/uc?export=view&id=1VAs4oj0eJbP9FWMDbCknHeTGwqKhv9Bf" width="380"/>
<figcaption>Figure: Set-up</figcaption>
</figure>

It will results in pictures like this.

<figure float="left">
<img src="https://drive.google.com/uc?export=view&id=1jv8v35KJlHZ5uVUubgYPWuxS-gvfuN4M" width="380"/>
<figcaption>Figure: Data</figcaption>
</figure>


Using these pictures, I will fit ellipses around the glares and make a data base of glares corresponding to a specific shape with the characteristics of their fitted ellipse, i.e. the length of the longest direction and the aspect ratio.

The glare detection algorithm use this data base to compare the shapes detected in new pictures to try and find the shape.

### Optimizing the MVEE algorithm
There are two possible ways to go about this

#### Contour using Snake algorithm
The Minimum Volume Enclosing Ellipse algorithm fits an ellipse around a number of points. As of now, it takes into account _all_ the points of a 'blob' but this is not necessary as it could use just the _contour_ of the 'blob' as the contour encloses all the points of the blob and thus the ellipse will do too. 


<figure float="left">
<img src="https://drive.google.com/uc?export=view&id=1zZX0E9f9naGASjUe9fngtluPsBNa7NA0" width="380"/>
<figcaption>Figure: Contour</figcaption>
</figure>


The contour detection would be implemented with the Snakes Algorithm ([https://en.wikipedia.org/wiki/Active_contour_model](https://en.wikipedia.org/wiki/Active_contour_model))

All the points are per definition contained inside the ellipse but, as the professor mentioned, it is important that the blob has a close to perfect 'filling ratio'. So to check this, we check if the subset of points contained by the minimum enclosing ellipse is almost identical to the subset of points of the blob. 

Let $`I`$ be the set of all pixels. Let $`A`$ be the subset of $`I`$ of all pixels contained by the ellipse and let $`B`$ be the subset of $`I`$ of all pixels belonging to the blob. Note that $`B`$ is also a subset of $`A`$ as the ellipse encloses all points of the blob. Let #($`A`$) denoted the number of elements inside of $`A`$. Then the filling ratio $`FR`$ is defined as 

```math
FR = 1-\frac{\#(A-B)}{\#(A)}
```

This introduces a magic number, namely the required FR for a blob to suffice.

In the picture below, the figure above had a good $`FR`$ and the figure below a bad $`FR`$ .

<figure float="left">
<img src="https://drive.google.com/uc?export=view&id=1G2XE4OqX_lkq9Sg8NbDApMpf6jFpA_dh" width="380"/>
<figcaption>Figure: Filling Ratio</figcaption>
</figure>

#### Area algorithm
An other possibility is to implement a sort-of-Snakes Algorithm but instead of 'snakes' we use 'areas'. 

The following paper is an example of this.

ISPRS Journal of Photogrammetry and Remote Sensing  
Volume 77, March 2013, Pages 57-65  
Extracting polygonal building footprints from digital surface models: A fully-automatic global optimization framework  
Mathieu Brédif, Olivier Tournaire, Bruno Vallet, Nicolas Champion  

This seems to be more complicated than the first option but maybe more interesting.

### Connect two or more blobs if they belong to the same tube  

In the current version of the method, when a tube crosses another one, the lower tube will be detected as two seperate ellipses because they don't connect. 

<figure float="left">
<img src="https://drive.google.com/uc?export=view&id=1bC04tU3Ov_xCw6jx2TZ8vEwsdhJPhfli" width="380"/>
<figcaption>Figure: Crossing tubes</figcaption>
</figure>

A possible relation I can think of is that if two blobs have the same largest eigenvalues or 'long direction' _and_ the center of one blob lies on the 'long direction' of the other, the two blobs belong to the same tube. 

Let $`v_{11}`$ and $`v_{12}`$ denoted the eigenvector of the largest eigenvalue of the first and second ellipse. Let $`AR_{1}`$ and $`AR_{2}`$ denoted the aspect ratio of the first and second ellipse. Let $`r_{1}`$ and $`r_{2}`$ denoted the centre of the first and second ellipse. If

```math
v_{11} = v_{12}
```
```math
AR_{1} = AR_{2} 
```
```math
r_{1} = r_{2}+c*v_{12} 
```

Then the two blobs are parallel, have the same aspect ratio and have centres on a line represented by the eigenvector. This implies that they belong to the same object.

Then we can combine these blobs into one and fit an ellipse around the combined blob.

<figure float="left">
<img src="https://drive.google.com/uc?export=view&id=1sJlxz4MogReMlXO4KkGM6FsZurwOLztq" width="380"/>
<figcaption>Figure: Crossing tubes with ellipses</figcaption>
</figure>


## Energy function for 'glare-y-ness'

An energy function allocates every pixel a certain amount of 'energy' based on a predefined definition of this 'energy'.

In the case of Seam Carving ([https://en.wikipedia.org/wiki/Seam_carving](https://en.wikipedia.org/wiki/Seam_carving)), egdes, more specifically the amount of 'edgy-ness', is associated with energy through an energy function, which assignes an amount of energy to a region based on the "edgy-ness" of that region. The path of 'least energy' is then found and removed.

In our case we would like to allocate 'energy' based on 'glare-y-ness'. 
  
Professor Bruyninckx already made some valid comments.
  
>The energy function _correlates_ (in the form of a function with "energy" as "units) the intensities of _many_ pixels, not just one.  
  
  > A decade ago, a master student has used "gradient function" as "energy": the glare on a smoothly curved surface has a smooth gradient in intensity; irrespective of the absolute value of that intensity (_except_ where the intensity value _saturates_ the pixel).  

This is very interesting but seems like a totally new approach when compared to Report I and II and not an extension to those Reports. This is something that can be look at when there is time to spare.



