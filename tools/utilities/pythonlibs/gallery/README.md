# Model Gallery Python Utilities

## Prerequisites
Python 3.x, Anaconda

## copy_validation_set.py
This utility takes a CNTK val.map.txt and a path to the validation set, and copies the validation image files to a Raspberry Pi using SCP. 

The format of val.map.txt is something like:
```
val.zip@/ILSVRC2012_val_00000001.JPEG	65
val.zip@/ILSVRC2012_val_00000002.JPEG	970
val.zip@/ILSVRC2012_val_00000003.JPEG	230
val.zip@/ILSVRC2012_val_00000004.JPEG	809
```
Each entry contains a path to the image file (e.g. `val.zip@/ILSVRC2012_val_00000001.JPEG`) and the ground truth label (`65`). You don't have to worry about the `val.zip@/` part of the path, because you will provide the local path to your validation images to the script. The script then uses the basename of the image path to construct a new path relative to the actual local path of each image. 

### Usage
```
python copy_validation_set.py --help
usage: This script takes a CNTK val.map.txt and a path to the validation set, and copies maxfiles files
to a Raspberry Pi using scp

       [-h] [--maxfiles MAXFILES] [--destdir DESTDIR] [--labels LABELS]
       validation_map validation_path ipaddress

positional arguments:
  validation_map       val.map.txt file containing the filenames and classes
  validation_path      path to the validation set
  ipaddress            ip address of the Raspberry Pi to copy to

optional arguments:
  -h, --help           show this help message and exit
  --maxfiles MAXFILES  max number of files to copy (up to the size of the
                       validation set) (default: 200)
  --destdir DESTDIR    destination directory on the Raspberry Pi (default: /home/pi/validation)
  --labels LABELS      path to the labels to optionally copy
```

### Example
Copy up to 50 images from the validation set on the host machine to a Raspberry Pi (target) at the IP address of 192.168.1.100 by calling this script on the host machine (note: not on the Raspberry Pi):
```
(py36) python copy_validation_set.py val.map.txt source_path_to_validation_set 192.168.1.100 --maxfiles 50
```
The images will be copied to /home/pi/validation on the Raspberry Pi device. After copying the validation set, you can use `run_validation.py` to validate an ELL model on the device.

## run_validation.py
This utility performs a validation pass on a given model on a Raspberry Pi using scp, and retrieves the validation result. This requires the model to have been deployed to `/home/pi/pi3` and the validation set to be copied to `/home/pi/validation`.

### Workflow from your host machine:
1. Deploy a model to your target (Raspberry Pi3) using `drivetest.py` from `ELL_ROOT/build/tools/utilities/pitest`.
2. Copy the validation set to your target using `copy_validation_set.py` described above.
3. Run `run_validation.py` from your host machine to initiate validation on the target.

### Usage
```
python run_validation.py  --help
usage: This script performs a validation pass on a given model
on a Raspberry Pi using scp, and retrieves the validation result
This requires the model to have been deployed to /home/pi/pi3 and
the validation set to be copied to /home/pi/validation

       [-h] [--labels LABELS] [--maxfiles MAXFILES] model ipaddress

positional arguments:
  model                name of the model
  ipaddress            ip address of the Raspberry Pi to copy to

optional arguments:
  -h, --help           show this help message and exit
  --labels LABELS      name of the labels file (default: cntkVgg16ImageNetLabels.txt)
  --maxfiles MAXFILES  max number of files to copy (up to the size of the
                       validation set) (default: 200)
```

### Example
After the `v_I64x64x3CCMCCMCCCMCCCMCCCMF2048` model is deployed to /home/pi/pi3, run the validation test on at most 10 images by calling this script from your host machine (note: not on the Raspberry Pi). 

This will SSH to the Raspberry Pi at 192.168.1.100, call the Python bindings for the `v_I64x64x3CCMCCMCCCMCCCMCCCMF2048` model, and run through the images in the validation set. To do this, it deploys the validate.py and validate.sh helper scripts to the Raspberry Pi.

```
(py36) python run_validation.py --maxfiles 10 v_I64x64x3CCMCCMCCCMCCCMCCCMF2048 192.168.1.100
```
Note that some models do take a long time to run on the Raspberry Pi, and consume high CPU usage for the OpenBLAS operations. For these models, try a relatively small --maxfiles setting and increase as needed.

## generate_md.py
To be added