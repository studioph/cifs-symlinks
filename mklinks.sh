#!/bin/bash

set -euo pipefail
IFS=$'\n\t'

# Where you have CIFS mounted
MOUNT_POINT="$1"

pushd "$MOUNT_POINT"

mkdir -p results
pushd results
ln -s ../source/real.txt link.txt
ln -s ../source/real linked