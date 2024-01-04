import os
import pathlib
import sys

here = pathlib.Path(sys.argv[1])

def pathwalk(root):
    for entry in root.iterdir():
        if entry.is_dir():
            pathwalk(entry)
        else:
            print(f"{entry}")


def oswalk(start):
    for root, dirs, files in os.walk(start, followlinks=True):
        print(f'root={root}')
        print(f'dirs={dirs}')
        print(f'files={files}')

