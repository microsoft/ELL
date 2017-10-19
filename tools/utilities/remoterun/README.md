## remoterun.py

This tool (optionally) copies a directory to a remote computer and then (optionally) runs a specified command with the copied directory as the working directory.

## Prerequisites

The tool relies on the following python libraries, all available via `conda install <library-name>`:

* paramiko
* getpass

## Usage

```
python remoterun.py [-h] [--source_dir SOURCE_DIR] [--target_dir TARGET_DIR]
                    [--username USERNAME] [--password PASSWORD]
                    [--command [COMMAND [COMMAND ...]]] [--logfile LOGFILE]
                    [--verbose] [--cleanup CLEANUP]
                    ipaddress
```

*Limitations:* 
Only a single level of hierarchy for the `target_dir` is supported.  
Only the immediate contents of `source_dir` are copied (non-recursive copy).
