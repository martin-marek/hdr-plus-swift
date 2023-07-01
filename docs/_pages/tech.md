---
permalink: /tech/
title: "Tech"
---

All photos contain noise, especially those captured in low light or high dynamic range settings. Since this noise is random, it can be reduced by averaging a burst of images. Flagship smartphones have been doing this for years (eg Google's [Night Sight](https://www.theverge.com/2018/10/25/18021944/google-night-sight-pixel-3-camera-samples), Apple's [Night Mode](https://support.apple.com/en-us/HT211306)) and it turns out that the same technology can be used with essentially any camera capable of capturing a burst of RAW images.

Fortunately, Google has published a [paper](http://static.googleusercontent.com/media/www.hdrplusdata.org/en//hdrplus.pdf) thoroughly explaining their burst photography pipeline. Our app is a simplified implementation of that pipeline.

### Alignment

When you capture a burst of images handheld or if the images contain movement (people, cars, stars, etc), the images need to be aligned to each other before they can be merged. In the case of astrophotography, the alignment is relatively simple since all the stars are moving in the same direction. In the case of portraits, this is a hard problem: a person can tilt their head, squint, and alter their smile, all at the same time. For these reasons, taking a long exposure is not an option to reduce noise. The only option is to capture a burst of shorter images and merge them using a pipeline like this.

In our pipeline, we first pick a "reference" image (typically the middle image) and try to align all the other "comparison" images to this image.

To align one of the comparison images with the reference image, we resize both of the images to a very small resolution. Afterwards, we divide the comparison image into a grid of tiles and try to align each tile to the reference image.

Once we have aligned the downscaled images, we use this alignment as a starting point and refine it using a slightly higher resolution image. We repeat this process multiple times until we align the original full-resolution images.

Below we illustrate this process on two images of letters. The letters are shifted and resized between the two images and the goal is to align the letters.

{% include figure image_path="/assets/images/tech/warping.gif" %}

Once the images are aligned, all that's left to do is to merge them.

### Merging

Our app uses merging approaches that are robust againt imperfect alignment of images caused by scene motion. It offers merging in the spatial domain and merging in the frequency domain. The latter is very similar to the approach described in the original [paper](http://static.googleusercontent.com/media/www.hdrplusdata.org/en//hdrplus.pdf). 

The motion robustness of the algorithms can be controlled by the paramter `Noise reduction`. Stronger noise reduction yields lower motion robustness and vice versa. 

To test motion robustness, a burst with strong scene motion is evaluated. The following figure shows a comparison between a single frame, the output from our app, and the output shown in Figure 6 of the original [paper](http://static.googleusercontent.com/media/www.hdrplusdata.org/en//hdrplus.pdf). The image is taken from Google's [HDR+ dataset](https://hdrplusdata.org/dataset.html) licensed under [CC BY-SA](https://creativecommons.org/licenses/by-sa/4.0/).

{% include figure image_path="/assets/images/tech/robustness_comparison.jpg" %}
