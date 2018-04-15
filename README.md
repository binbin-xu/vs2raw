# VS2Raw

This is a tool to convert dataset into `.raw` format that fits `kfusion` and `ofusion`.

### ViSim dataset

To convert a `ViSim` dataset into `.raw` format, please put images into the following structure:

Given a folder `dataset`:

```
dataset/cam0/data -- where you put the rgb images
dataset/cam0/data.csv -- where you put a csv file that contains filenames of all rgb images
dataset/depth0/data -- where you put the depth images
dataset/depth0/data.csv -- where you put a csv file that contains filenames of all depth images
dataset/cameraInfo.txt -- where you put intrinsic of the camera
```

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
./vs2Raw [path-to-ViSim-dataset] [camera-index] [optional: low-quality-image (0 or 1)]
```

### Co-fusion dataset

To convert a `Co-fusion` synthetic dataset into `.raw` format, please put images into the following structure:

Given a folder `dataset`:

```
dataset/colour -- where you put the rgb images
dataset/depth_original -- where you put the depth images
dataset/trajectories/gt-cam-0.txt -- where you put the GT camera trajectory
dataset/calibration.txt -- where you put intrinsic of the camera
```

A sample `calibration.txt` could look like this:

```
564.3 564.3 480 270 960 540
```

which follows format:

```
[focal lens x] [focal lens y] [image center x] [image center y] [image width] [image height] 
```

Then please run:

```
./cf2Raw [path-to-ViSim-Folder] [optional: use-noisy-depth (0 or 1)]
```

