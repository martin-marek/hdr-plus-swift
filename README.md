# Burst Photo

This is a macOS app written in Swift / SwiftUI / Metal that implements a simplified version of HDR+, the computational photography pipeline in Google Pixel phones. You can read more about HDR+ in Google's paper [Burst photography for high dynamic range and low-light imaging on mobile cameras](http://static.googleusercontent.com/media/www.hdrplusdata.org/en//hdrplus.pdf).

If you are a researcher or you prefer Python/PyTorch, I suggest you check out [hdr-plus-pytorch](https://github.com/martin-marek/hdr-plus-pytorch).

# Example

I took a burst of 51 images at ISO 51,200 on Sony A7S III and boosted it by +1.35EV. Here's a [comparison](https://burst.photo/assets/images/gallery/monika_stars.jpg) of a single image from the burst vs. a merge of all the images.

![](docs/assets/images/home/monika_stars.jpg)

To test motion-robustness, I compared a raw image, output from the app using `Robustness` set to `High`, and output from the app using `Robustness` set to `Low`. You can compare my results to Google's by opening Figure 6 in their [paper](http://static.googleusercontent.com/media/www.hdrplusdata.org/en//hdrplus.pdf). Image taken from Google's [HDR+ dataset](https://hdrplusdata.org/dataset.html) licensed under [CC BY-SA](https://creativecommons.org/licenses/by-sa/4.0/).

![](docs/assets/images/tech/robustness_comparison.jpg)

For more examples, please visit [burst.photo/gallery/](https://burst.photo/gallery/).

# Usage

To process a burst of RAW images, simply drag-and-drop them into the app. Only DNG files are supported by default - but if you download and install [Adobe DNG Converter](https://helpx.adobe.com/camera-raw/using/adobe-dng-converter.html), Burst Photo will be able to convert most RAW formats in the background. You can read more at [burst.photo/help/](https://burst.photo/help/).

![](docs/assets/images/help/drag-and-drop.jpg)

# Download

You can download the app from the [Mac App Store](https://burst.photo/download/) or as a [GitHub release](https://github.com/martin-marek/hdr-plus-swift/releases).

# Features
- [x] DNG support
- [x] RAW support (requires Adobe DNG Converter to be installed)
- [x] robust merge (simplified)
- [x] Bayer sensor support (good)
- [x] non-Bayer sensor support (beta)
- [x] preserves lens profiles
- [x] mutli-threaded image loading
- [x] align+merge running in pure Metal
- [x] native Intel, Apple Silicon support

# TODO
- [ ] super-resolution
- [ ] robust merge in frequency domain
- [ ] fix progressbar getting stuck loading the first image
- [ ] 32-bit output - how can this be achieved in Adobe DNG SDK?

Please feel free to contribute to any of these features or suggest other features.

# Acknowledgements

This product includes DNG technology under license by Adobe.
