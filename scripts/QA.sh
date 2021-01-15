#!/bin/bash
# Rong tao

function QA() {
    while true
    do
        echo -n "$1 [Y/N]: "
        read x
        case "$x" in
        Y|y|yes|Yes|YES ) return 0;;
        N|n|no|No|NO  ) return 1;;
        * ) echo "Input yes or no!"
        esac
    done
}

