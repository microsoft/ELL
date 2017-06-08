Useful optimizations for FP math:

-ffast-math
-fno-signed-zeros
-freciprocal-math -- allow division operations to be reassociated
-fstrict-enums -- enable optimizations based on enum value range

-funroll-loops
-fno-unroll-loops
-freroll-loops
-fno-reroll-loops
-fmglobal-merge -- enable merging of globals

-fwhole-program-vtables (requires -flto)
-flto -- enable LTO in 'full' mode (???)

-save-temps -- save intermediate compilation results

