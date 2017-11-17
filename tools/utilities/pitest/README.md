# Raspberry Pi test tools

## Requirements

paramiko (SSH client)

``` 
conda install paramiko
```

getpass (for safely prompting for a password)

```
conda install getpass
```

## Using remoterunner with make_profile to execute a profile app remotely:

First build the profiler:
```
bin/make_profiler.sh d_I160x160x3NCMNCMNBMNBMNBMNBMNB1A.ell profile_pi_par4 --target pi3 -vec -par -th 4
```

Then push it to the remote and build:

```
python tools/utilities/pitest/remoterunner.py 157.54.156.100 --source_dir ./profile_pi_par4 --target_dir profile_pi_par4
```

