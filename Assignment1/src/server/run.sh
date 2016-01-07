#! /bin/bash -x
make -C ../../ clean 
make -C ../../ common 
make -C ../../ server 
./myserver 342
