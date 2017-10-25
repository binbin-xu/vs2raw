# VS2Raw

please put images into the following structure:

Given a folder `dataset`:

```
dataset/cam0/data -- where you put the rgb images
dataset/cam0/data.csv -- where you put a csv file that contains filenames of all rgb images
dataset/depth0/data -- where you put the depth images
dataset/depth0/data.csv -- where you put a csv file that contains filenames of all depth images
dataset/cameraInfo.txt -- where you put intrinsic of the camera.
````

A sample `cameraInfo.txt` could look like this:

```
#Image Res.(w,h):
640 480
#focus lens     :
606.0509716312624 609.2072669938981
#principal pt   :
327.88802413335253 239.04273996525234
#Undist. coeff. :
0 0 0
```

Then please run:

```
vs2Raw dataset
```