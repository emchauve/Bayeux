#!/usr/bin/env bash

opwd=$(pwd)

function my_exit ()
{
    cd ${opwd}
    exit $1
}

if [ -d __build ]; then
    rm -fr __build
    rm -fr __install
fi

rm -f *~
rm -f test_serializable_1.txt

my_exit 0

# end
