## ELL Python Package

This folder contains the scripts needed to build the `conda` package containing the ELL compiler.

After building the Release version of ELL and _ELL_python target, you can run this:

```
conda build --python 3.6 --build-only .
```

The easiest way to test and debug the package is to "install" the package in developer mode, using the following command. 

```
conda develop .
```

This command should be run in the **build** package directory, i.e. `[ELL root]/build/interfaces/python/package`. This only needs to be done once. This causes Conda to treat the package as though it were installed, but without running the installer or creating a copy. Because the installer is not run, you will need to install any dependent packages in `meta.yaml` by hand. For now, two of the packages should be installed from the `microsoft-ell` channel; the rest are standard.

```
conda install -c microsoft-ell llvmdev
conda install -c microsoft-ell swig
conda install paramiko
conda install opencv
conda install github
```

After this, Jupyter pages can then import the `ell` module. Because the ELL package is installed in developer mode, any changes you make to the package's code will be visible after restarting the Jupyter kernel (which reloads all packages).
