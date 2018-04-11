The programs can be run in the following way:

Texture Classification
TextureClassifcation <Number of clusters> <Balance> <Normalize> <Initializer> <use Feature Reduction> <input_images_folder>

Texture Segmentation
TextureSegmentation <Number of clusters> <Laws' filter size> <Balance> <Normalize> <Initializer> <window size> <use Feature Reduction> <input_filename.raw> <output_filename.raw>

Basic Edge Detection
BasicEdgeDetection <detection filter> <threshold percent (integer)> <print histogram> <denoise> <input_filename.raw> <output_filename.raw>

In every program, the user will be asked to enter the image properties as follows:
Enter image size as <height> <width> <1-grayscale/3-color>, e.g. 100 50 3

You can create a separate folder where you can keep all the source and result images.
Or, all the source images can be kep in the bin folder itself and the above commands can be run from that folder.
Example:
./MorphologicalFiltering 2 0 50 ../../SourceImages/Lena.raw ../../SourceImages/jigsaw_1.raw ../../ResultImages/jigsaw_1_thinning.raw