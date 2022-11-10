---
permalink: /guide/
title: "Guide"
---

Our app takes as input a burst of images and stacks them to produce a single output image with reduced noise (you can read more about the process [here](/tech/)). Below is a step-by-step guide to using the app.

### Capturing images

The app relies on aligning and merging a burst of similar images. Ideally, try to capture at least 5 - 10 sharp images without significant movement between them. It is okay if you take the burst handheld or if a person moves slightly between the photos. However, try to avoid motion blur and significant movement between images (for example a person tilting their head or changing their smile).

### Image formats

The only *natively* supported image format is DNG. However, if you download and install [Adobe DNG Converter](https://helpx.adobe.com/camera-raw/using/adobe-dng-converter.html), Burst Photo will be able to convert most RAW formats in the background.

### Loading images

To process a burst of images, please drag-and-drop them into the app. You can either drag-and-drop individual images or a folder containing the whole burst.

{% include figure image_path="/assets/images/help/drag-and-drop.jpg" %}

### Settings

There are three settings in the app: `Tile size`, `Search distance`, and `Robustness`. The default settings should work well in most cases. However, if you want to get the best results possible, please read about [our tech](/tech/) and follow the tips below.

Each image is aligned as a grid of tiles. `Tile size` controls the size of these tiles in pixels. For photographs with lots of motion, it is preferable to use small tiles, so that the images can be aligned precisely. On the other hand, if there is little motion and lots of noise, it is preferable to use large tiles.

`Search distance` controls the maximum alignment magnitude between images. It should be set to "Low" if all the images are very similar. Conversely, if there is significant movement between images, it should be set to "High".

`Robustness` is perhaps the most important setting: it controls the tradeoff between noise reduction and motion artifacts. You can read more about it [here](/tech/#merging).


### Tips
- The default `Tile size` (16) and `Search distance` (Medium) usually work well.
- If you see any motion artifacts, increase `Robustness`.
- If you see too much noise, decrease `Robustness`.
- To achieve the highest dynamic range, underexpose your images. Shadows can typically be boosted but if the highlights are clipped, they can’t be recovered.
