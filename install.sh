#!/bin/bash

mkdir -p ~/.gdb.d/

# Copy the .gdbinit file, and save the previous one if it exists.
if [ -f ~/.gdbinit ]; then
    cp ~/.gdbinit ~/.gdbinit.old
fi
cp ./.gdbinit ~/.gdbinit

cp ./texture-viewer.py ~/.gdb.d/
