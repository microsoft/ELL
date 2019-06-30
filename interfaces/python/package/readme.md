## ELL Python Package

This folder contains the scripts needed to build the `conda` package containing the ELL compiler.

This assumes you have already followed the INSTALL-* instructions in the root of this repo and
you have built the Release version of ELL and _ELL_python target.

To build the ELL python package:

```
conda build --python 3.6 --build-only .
```

The easiest way to test and debug the package is to "install" the package in developer mode, using the following command.

```
conda develop .
```

This command should be run in the **build** package directory, i.e. `[ELL root]/build/interfaces/python/package`. This only needs to be done once. This causes Conda to treat the package as though it were installed, but without running the installer or creating a copy. Because the installer is not run, you will need to install any dependent packages in `meta.yaml` by hand.

After this, Jupyter pages can then import the `ell` module. Because the ELL package is installed in developer mode, any changes you make to the package's code will be visible after restarting the Jupyter kernel (which reloads all packages).
