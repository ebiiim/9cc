#!/bin/sh

# docker build -t compilerbook https://www.sigbus.info/compilerbook/Dockerfile

if [ "$#" -eq 0 ]; then
    docker run --rm -it -v $PWD:/9cc -w /9cc compilerbook
else
    docker run --rm -v $PWD:/9cc -w /9cc compilerbook $@
fi
