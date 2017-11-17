# Notes about Posix runtime

## pthread datatypes as expressed in LLVM IR for various platforms

### MacOS

```
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.13.0"

%struct._opaque_pthread_t = type { i64, %struct.__darwin_pthread_handler_rec*, [8176 x i8] }
%struct.__darwin_pthread_handler_rec = type { void (i8*)*, i8*, %struct.__darwin_pthread_handler_rec* }
%struct._opaque_pthread_attr_t = type { i64, [56 x i8] }
%struct._opaque_pthread_mutex_t = type { i64, [56 x i8] }
%struct._opaque_pthread_mutexattr_t = type { i64, [8 x i8] }
%struct._opaque_pthread_cond_t = type { i64, [40 x i8] }
%struct._opaque_pthread_condattr_t = type { i64, [8 x i8] }
%struct._opaque_pthread_once_t = type { i64, [8 x i8] }
```

*Note:* The `pthread_t` type gets translated into a _pointer_ to a `%struct._opaque_pthread_t`, whereas the others are used directly. 

```
// Types
pthread_t thread;
pthread_attr_t thread_attr;
pthread_mutex_t mutex;
pthread_mutexattr_t mutexattr;
pthread_cond_t cond;
pthread_condattr_t condattr;
pthread_once_t once;
```

Gets translated to
```
...
%6 = alloca %struct._opaque_pthread_t*, align 8
%7 = alloca %struct._opaque_pthread_attr_t, align 8
%8 = alloca %struct._opaque_pthread_mutex_t, align 8
%9 = alloca %struct._opaque_pthread_mutexattr_t, align 8
%10 = alloca %struct._opaque_pthread_cond_t, align 8
%11 = alloca %struct._opaque_pthread_condattr_t, align 8
%12 = alloca %struct._opaque_pthread_once_t, align 8
```


### Raspbian Jesse (on Pi3)

```
target datalayout = "e-m:e-p:32:32-i64:64-v128:64:128-n32-S64"
target triple = "armv6-unknown-linux-gnueabihf"

%union.pthread_attr_t = type { i32, [32 x i8] }
%union.pthread_mutex_t = type { %"struct.(anonymous union)::__pthread_mutex_s" }
%"struct.(anonymous union)::__pthread_mutex_s" = type { i32, i32, i32, i32, i32, %union.anon }
%union.anon = type { i32 }
%union.pthread_mutexattr_t = type { i32 }
%union.pthread_cond_t = type { %struct.anon }
%struct.anon = type { i32, i32, i64, i64, i64, i8*, i32, i32 }
%union.pthread_condattr_t = type { i32 }
```

*Note:* The `pthread_t` and `spinlock_t` types get translated into an `int32`, whereas the others are used directly. 

```
// Types
pthread_t thread;
pthread_attr_t thread_attr;
pthread_mutex_t mutex;
pthread_mutexattr_t mutexattr;
pthread_cond_t cond;
pthread_condattr_t condattr;
pthread_spinlock_t spinlock;
pthread_once_t once;
```

Gets translated to

```
...
%thread = alloca i32, align 4
%thread_attr = alloca %union.pthread_attr_t, align 4
%mutex = alloca %union.pthread_mutex_t, align 4
%mutexattr = alloca %union.pthread_mutexattr_t, align 4
%cond = alloca %union.pthread_cond_t, align 8
%condattr = alloca %union.pthread_condattr_t, align 4
%spinlock = alloca i32, align 4
%once = alloca i32, align 4
```


## Dragonboard (Linaro)

```
target datalayout = "e-m:e-i64:64-i128:128-n32:64-S128"
target triple = "aarch64-unknown-linux-gnu"

%union.pthread_attr_t = type { i64, [56 x i8] }
%union.pthread_mutex_t = type { %"struct.(anonymous union)::__pthread_mutex_s", [8 x i8] }
%"struct.(anonymous union)::__pthread_mutex_s" = type { i32, i32, i32, i32, i32, i32, %struct.__pthread_internal_list }
%struct.__pthread_internal_list = type { %struct.__pthread_internal_list*, %struct.__pthread_internal_list* }
%union.pthread_mutexattr_t = type { i64 }
%union.pthread_cond_t = type { %struct.anon }
%struct.anon = type { i32, i32, i64, i64, i64, i8*, i32, i32 }
%union.pthread_condattr_t = type { i32, [4 x i8] }
```

*Note:* The `pthread_t` type gets translated into an `int64`, `pthread_spinlock_t` gets translated to an `int32`, and the rest are used directly. 

```
// Types
pthread_t thread;
pthread_attr_t thread_attr;
pthread_mutex_t mutex;
pthread_mutexattr_t mutexattr;
pthread_cond_t cond;
pthread_condattr_t condattr;
pthread_spinlock_t spinlock;
pthread_once_t once;
```

Gets translated to

```
%thread = alloca i64, align 8
%thread_attr = alloca %union.pthread_attr_t, align 8
%mutex = alloca %union.pthread_mutex_t, align 8
%mutexattr = alloca %union.pthread_mutexattr_t, align 8
%cond = alloca %union.pthread_cond_t, align 8
%condattr = alloca %union.pthread_condattr_t, align 4
%spinlock = alloca i32, align 4
%once = alloca i32, align 4
```
