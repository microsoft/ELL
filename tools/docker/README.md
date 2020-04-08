## ELL Docker Images

### Building Docker Images
The **ell-dependencies** Dockerfile contains the pre-reqs needed to build ELL, but does not include ELL itself.
This should only need to be generated once, as the base dependencies rarely change.
Note: this is used as the base ("FROM") image for the "ell" Dockerfile.
```
docker build -t ell-dependencies -f ell-dependencies.Dockerfile .
```

The **ell** Dockerfile builds ELL and all included tools. This can serve as a continuous integration and testing image.
* Note: this depends on the presence of a "ell-dependencies" image, as generated above.
* Note: we recommend building with the '--no-cache' flag, to ensure a clean build

*If called from the tools/docker directory (the context is set to the project root directory (../..):*
```
docker build --no-cache -t ell -f ell.Dockerfile ../..
```
*Alternatively, if this called from the ELL repo root directory:*
```
docker build --no-cache -t ell -f tools/docker/ell.Dockerfile .
```


### Running Docker Images
If you just want to run the ELL toolchain and get the results, you can do so by creating a container and directly calling the onnxImportAndCompile.py script:

```
# Start a docker container called "ell-task" using the "ell:latest" image
docker run  -d -t --name ell-task ell:latest

# Copy your onnx model over to the running container (Assumes you have a local "model.onnx" file)
docker cp model.onnx ell-task:/model/model.onnx

# Run the onnxImportAndCompile script; specify targets as comma-separated list (i.e. Raspberry Pi Zero, Pi 3)
docker container exec ell-task "python" /ELL/tools/utilities/onnxImportAndCompile/onnxImportAndCompile.py --target pi0,pi3

# Extract the artifacts
docker cp ell-task:/model .

# Remove the container
docker rm -fv ell-task

```

See [onnxImportAndCompile.py documentation](../utilities/onnxImportAndCompile) for additional args that can be passed to onnxImportAndCompile.py, language and custom onnx model path.


### Running Docker Images - Advanced

If you want to run the ELL commands yourself, you may boot up an "ell" container with the command:
```
docker run --rm -it "ell"
```
This will take you to a bash command prompt inside the ELL environment, under the directory ```/model```. All of the ELL code and binaries are available located under ```ELL```, or using the following environment variables:
* ```$ELL_ROOT=/ELL```
* ```$ELL_BUILD=/ELL/build/bin```