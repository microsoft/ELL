Example LLVM output from simple ELL models
==========================================

Identity example
----------------

To generate LLVM IR:

```
 > cd build/bin/Release 
 > compile -imf ..\..\examples\data\identity.model -ocf ../../../private/examples/compiled_code/identity.asm
```

To generate ARM M0 assembly:

```
 > cd private/binaries/llvm/scripts
 > llcM0 ../../../examples/compiled_code/identity.asm
```

Times-two example
-----------------

```
 > cd build/bin/Release 
 > compile -imf ../../examples/data/times_two.model -ocf ../../../private/examples/compiled_code/times_two.asm

 > cd private/binaries/llvm/scripts
 > llcM0 ../../../examples/compiled_code/times_two.asm
```
