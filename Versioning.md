# Guidance on ELL's versioning
#### v1.0.0

ELL's versioning follows [Semantic Versioning 2.0.0](https://semver.org/) and
has the pattern of xx.yy.nn. The version number has an impact on
 * the ELL toolchain
 * the ELL Python API ("ELL API")
 * the emitted API that is the result of compiling a model ("emitted API").
 Currently, this could be either a C++ or a Python API.

xx - represents the MAJOR version number.

yy - represents the MINOR version number.

nn - represents the PATCH version number.

An omission of a component of the version number indicates that it applies to
all series of versions that satisfy the given constraint. For example, version
2 represents all versions of the form 2.yy.nn using the aforementioned
representation. Similarly, version 1.3 represents all versions of the form
1.3.nn.

**Incrementing the MAJOR version number** should be done when
 * a major new feature is added to the ELL toolchain and is showcased by a new
 tutorial demonstrating this feature, when applicable
   * for example, "Getting started with audio keyword spotting on the Raspberry
   Pi" would cause an increment, but "Active cooling your Raspberry Pi 3" would
   not, since it is not related to the ELL toolchain.
 * a major internal change has been made to the ELL toolchain
   * for example, a change in algorithms used that results in a significant
   performance improvement of emitted models
 * a large feature has been added to the ELL API or emitted API
 * the ELL API or emitted API has been changed in a way that is **not**
 backwards compatible
 * a breaking change has been made to ELL's model serialization format

**Incrementing the MINOR version number** should be done when
 * there is customer-facing behavior that has changed when models are compiled
    * things such as changing the way a model is (de)serialized
    * new types of models can be compiled
    * for example, changing the default convolution method that gets selected
    would require an increment.
 * new features are added to the ELL API or emitted API that are backwards
 compatible

**Incrementing the PATCH version number** should be done when
 * anything else changes within the ELL toolchain, Python API, or emitted API
   * bug fixes, performance improvements, refactoring of code

Not all changes to the repository warrant a version change. For example,
 * test code changes
 * script updates that don't impact use of the ELL toolchain
 * reformatting of code for styling changes
 * tutorial or documentation updates that amount to typo corrections or style
 updates

A version update can also be made outside of the aforementioned reasons as a
means to indicate significant development

The goal of the ELL API backwards compatibility is to support code written
against the ELL API of an older version of ELL. As long as the major version of
ELL is the same, the code will continue to work as intended. For example, code
authored against the ELL v1.2 API will continue to work for ELL v1.8 API, but
not the ELL v2.0 API.

The goal of the emitted API backwards compatibility is to support code written
against the emitted API created by an older version of ELL. As long as the
major version of ELL is the same, the code will continue to build and work as
intended. For example, code authored against the emitted API of a model
compiled with v1.2 of the ELL toolchain will continue to build and work against
the emitted API of the model when compiled with v1.8 of the ELL toolchain, but
not with the emitted API of the model (assuming the model is properly upgraded)
when compiled with v2.0 of the ELL toolchain.

The goal of the ELL toolchain backwards compatibility is to support the
compilation of models that have been created by an older version of ELL. As
long as the major version of ELL is the same, the toolchain of a newer version
of ELL can compile a model that has been created for an older version of ELL.
Note that forward compatibility is not supported, i.e., the version used to
create the model is the **minimum** required version of ELL to compile the
model.

|v1.2           | v1.5          | v1.8          | v2.1          | v2.3          |
|---------------|---------------|---------------|---------------|---------------|
| Authored      | Supported     | Supported     | Not supported | Not supported |
| Not supported | Authored      | Supported     | Not supported | Not supported |
| Not supported | Not supported | Authored      | Not supported | Not supported |
| Not supported | Not supported | Not supported | Authored      | Supported     |
| Not supported | Not supported | Not supported | Not supported | Authored      |
