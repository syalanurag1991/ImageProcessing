The programs can be executed in the following way:

Billinear Interpolation
BilinearInterpolation <-s/-d> <target-Width/H-Scale> <target-Height/V-Scale> <input_filename.raw> <output_filename.raw>
		
Film Effect
FilmEffect <target_filename.raw> <input_filename.raw> <output_filename.raw> <histogram_filename.csv>

Histogram EQ
HistogramEQ <equalization method> <input_filename.raw> <output_filename.raw> <histogram_filename.csv>

Noise Removal
NoiseRemoval <N-for Filter block size> <original_filename.raw> <input_filename.raw> <output_filename.raw>

Oil Painting Effect
OilPaintingEffect <n-bit color> <N-for Nearest-Neighbour block size> <input_filename.raw> <output_filename.raw>

RGB to CMY
RGBtoCMY <input_filename.raw> <output_filename.raw>

RGB to GrayScale
RGBtoGrayscale <grayscale method> <input_filename.raw> <output_filename.raw>

Circular Warping
CircularWarping <-w/-d> <input_filename.raw> <output_filename.raw>
w-warp | d-dewarp

Image Rotation - Roate by any angle
ImageRotation <anti-clockwise angle> <input_filename.raw> <output_filename.raw>

Image Stitching
ImageStitch <left_image.raw> <middle_image.raw> <right_image.raw> <output_filename.raw>

Color halftoning - GrayScale - Dithering
Dithering <Dithering Method> <Number of Tones/Gray Levels> <Kernel Size> <input_filename.raw> <output_filename.raw>
Available dithering methods: 1 - Fixed 2 - Random 3 - Bayer Matrix
Available tones/gray levels: 2 - Dual Tone 4 - Quad Tone
Available kernel sizes for dithering: 2, 4 and 8

Color halftoning - GrayScale and RGB - Error Diffusion
ErrorDiffusion <Diffusion Method> <Number of Tones/Gray Levels> <-f/-uf> <input_filename.raw> <output_filename.raw>
Available error diffusion methods: 1 - Floyd–Steinberg 2 - JJN 3 - Stucki
Available tones/gray levels: 2 - Dual Tone 4 - Quad Tone
Use -f to use median filter, else use -nf for no filtering

Objects in the image - Counting stars and there sizes
ObjectsInImage.exe <Foreground> <input_filename.raw> <output_filename.raw>
Foreground: 0-Black 1-White, Background (set automatically) = Opposite of Foreground

Morphological Operation - Shrinking, Thinning and Skeletonizing
MorphologicalFiltering <Morphing operation> <Foreground> <threshold> <input_filename.raw> <output_filename.raw>
Available Operations: 1-Shrinking 2-Thinning 3-Skeletonizing
Foreground: 0-Black 1-White, Background (set automatically) = Opposite of Foreground

Counting Game
CountingGame <Foreground> <input_filename.raw> <output_filename.raw>
Foreground: 0-Black 1-White, Background (set automatically) = Opposite of Foreground

In every program, the user will be asked to enter the image properties as follows:
Enter image size as <height> <width> <1-grayscale/3-color>, e.g. 100 50 3

You can create a separate folder where you can keep all the source and result images.
Or, all the source images can be kep in the bin folder itself and the above commands can be run from that folder.

Example 1:
./MorphologicalFiltering 2 0 50 ../../SourceImages/Lena.raw ../../SourceImages/jigsaw_1.raw ../../ResultImages/jigsaw_1_thinning.raw

Example 2:
./NoiseRemoval 11 ../../SourceImages/Lena.raw ../../SourceImages/Lena_mixed.raw ../../ResultImages/Lena_Clean_11.raw