
// fixes to SWIG typedefs so int64_t works properly on windows

#if defined(WIN32)

typedef long long 		    int64_t;
typedef unsigned long long 	uint64_t;
%apply long long { int64_t };
%apply unsigned long long { uint64_t };

#else

typedef long int 		    int64_t;
typedef unsigned long int 	uint64_t;
%apply long int  { int64_t };
%apply unsigned long int { uint64_t };

#endif


%include "stdint.i"

