Example LLVM output from simple ELL models
==========================================

Identity example
----------------

To generate LLVM IR and a header file:

```
 > cd build/bin/Release 
 > compile -imf ../../examples/data/identity.model --ir --header
```

To generate ARM M0 assembly:

```
 > cd private/binaries/llvm/scripts
 > llc -mtriple=armv6m-unknown-none-eabi -march=thumb -mcpu=cortex-m0 -float-abi=soft -mattr=+armv6-m,+v6m -filetype=asm ../../../examples/compiled_code/identity.ll
``` 

Times-two example
-----------------

```
 > cd build/bin/Release 
 > compile -imf ../../examples/data/times_two.model --ir --header

 > cd private/binaries/llvm/scripts
 > llc -mtriple=armv6m-unknown-none-eabi -march=thumb -mcpu=cortex-m0 -float-abi=soft -mattr=+armv6-m,+v6m -filetype=asm ../../../examples/compiled_code/times_two.ll
```
