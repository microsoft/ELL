## datasetFromImages.py

Example usage:
```shell
python datasetFromImages.py --imageSize 64x64 --outputDataset test.gsdf --bgr True --folder data
```

Help:

```
python datasetFromImages.py -h

usage: Creates ELL dataset file from list of labelled images
       [-h] [--imageSize IMAGESIZE] [--outputDataset OUTPUTDATASET]
       [--bgr BGR] [--positiveLabel POSITIVELABEL]
       [--exampleList EXAMPLELIST | --folder FOLDER]

optional arguments:
  -h, --help            show this help message and exit
  --imageSize IMAGESIZE
                        each image example data is cropped and scaled to width
                        x height e.g. 224x224
  --outputDataset OUTPUTDATASET
                        save dataset to this file name, default is dataset.txt
  --bgr BGR             specify True if output data should be in BGR format
                        (default True)
  --positiveLabel POSITIVELABEL
                        if examples define a binary classification (e.g. A,
                        not A), specify which class is the positive label.
  --categories CATEGORIES
                        if examples define a multi-class classification (e.g.
                        A, B, C), specifies the class category names index
                        file.
  --exampleList EXAMPLELIST
                        path to the file containing list of examples, where
                        each line is a label number followed by whitespace
                        path to image
  --folder FOLDER       path to a folder, with sub-folders containing images.
                        Each sub-folder is a class and the images inside are
                        the examples of that class
```
### What is this tool used for?
The datasetFromImages tool creates ELL compatible datasets from images, which can then be used by ELL's trainers to produce a trained predictor.

Images are specified using `--folder` option or the `--exampleList` options.

#### --folder
Use this option when your images are in a hierarchical folder structure e.g.
```
data/
   squirrel/
       squirrel1.jpg
       squirrel2.jpg
       ...
   dog/
       dog1.jpg
       dog2.jpg
       ...
   bird/
       bird1.jpg
       bird2.jpg
       ...
   ...
```
Running:
```shell
python datasetFromImages.py --imageSize 64x64 --outputDataset myDataset.gsdf --folder data
```
Will create a dataset where all files under data/squirrel will acquire the `squirrel` label, all files under data/dog will acquire the `dog` label, all files under bird will acquire the `bird` label and so on.

#### --exampleList
Use this option to specify a list of labels and paths to images via a text file. For example, let's say you have your labels and images listed in a file called `myExamples.txt`, which looks like:
```
    1.0 /data/squirrel1.jpg
    1.0 /data/other/squirrel2.jpg
    ...
    -1.0 /data/bird1.jpg
    -1.0 /moreData/table1.jpg
```
Then running:
```shell
python datasetFromImages.py --imageSize 64x64 --outputDataset myDataset.gsdf --exampleList myExamples.txt
```
Will create a dataset with squirrel images labeled 1.0 and the others labelled -1.0.

#### --positiveLabel
Used in conjunction with `--folder` when you want to create a binary classification dataset. For example, this could be the case when building a squirrel detector model, where squirrel images are 1.0 and everything else is -1.0.

```
data/
   squirrel/
       squirrel1.jpg
       squirrel2.jpg
       ...
   dog/
       dog1.jpg
       dog2.jpg
       ...
   bird/
       bird1.jpg
       bird2.jpg
       ...
```
Running:
```shell
python datasetFromImages.py --imageSize 64x64 --outputDataset myDataset.gsdf --folder data --positiveLabel squirrel
```
Will produce a dataset where all files under `squirrel` are labeled with 1.0, and everything else is -1.0.

#### --categories
Used in conjunction with `--folder` when you want to create a multi-class dataset. For example, this could be the case when building a classifier for `squirrel`, `dog` and `cat`. Say you have the example data in a the following folder structure:

```
data/
   squirrel/
       squirrel1.jpg
       squirrel2.jpg
       ...
   dog/
       dog1.jpg
       dog2.jpg
       ...
   bird/
       bird1.jpg
       bird2.jpg
       ...
```

Running:
```shell
python datasetFromImages.py --imageSize 64x64 --outputDataset myDataset.gsdf --folder data --categories myCategories.txt
```
Will allow you to provide a class categories file which lists each class on a separate line. The order of the classes is the class index for those examples e.g. if `myCategories.txt` were used above, then:
```
bird
dog
squirrel
```
Will create a dataset where `bird` examples are class `0`, `dog` examples are class `1`, and squirrel examples are class `2`. A classifier trained with this dataset will output predictions as a vector of 3 values in the corresponding order of bird at position 0, dog at position 1, and squirrel at position 2.

If this parameter is excluded, then a `categories.txt` file will be created for you. The order of the entries will be the order that the tool enumerates the folders in.
