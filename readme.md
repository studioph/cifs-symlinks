# Symlinks

Reproducible example for [this issue](https://github.com/python/cpython/issues/102503) with `os.walk` not detecting symlinks correctly on mounted CIFS filesystems due to a bug in the kernel CIFS driver.

## Running
1. Install samba (`sudo apt install samba`)
2. Fill out `smb.conf` with the needed values and place at `/etc/samba/smb.conf`
3. Restart samba to pick up new config (`sudo systemctl restart smbd.service`)
4. Mount the share somewhere (`sudo mount -f cifs -o guest,vers=3.0,uid=1000,gid=1000,mfsymlinks //127.0.0.1/symlinks /mnt/cifs`)
5. Run `mklinks.sh` to create some symbolic links from the client (`mklinks.sh /mnt/cifs`)
    - This will make some links in `results` that point to `../source`
6. Compile the C program to show the discrepency between the system calls (`gcc -o testdent testdent.c`)
7. Run the C program and point it to the CIFS mount point results folder: `./testdent /mnt/cifs/results`
    - The output should look like the following:
```
linked
D_TYPE=Regular file
LSTAT type=Link
STAT type=Directory

link.txt
D_TYPE=Regular file
LSTAT type=Link
STAT type=Regular file
```
Where the `D_TYPE` value is incorrectly reported for both symlinks

## Notes
- `os.walk` uses `scandir` which uses `readdir` system call
  
  - Relies on `d_type` property of struct returned by `readdir` to determine if file, dir, link, etc
    
  - Done to avoid making extra `stat` system call for each item
    
  - If `DT_UNKOWN`, falls back to `stat`
    
  - `d_type` incorrectly reported as `DT_REG` with CIFS instead of `DT_DIR/DT_LNK` or `DT_UNKNOWN`
    
- `pathlib` uses `os.path.is_` which uses `stat` system call
  
- CIFS does not natively support symlinks
  
  - Can emulate symlinks using `mfsymlinks` mount option in order to create symlinks from the client
    
  - Existing symlinks not created by client are presented as hardlinks and do not have the above issue with the wrong `d_type` value
    

## Solutions

- Use NFS if possible (full POSIX compliance)
  
- Use alternative traversal methods (i.e. `pathlib`)
  
  - Less efficient (extra `stat` system call for each item to determine if file or folder)