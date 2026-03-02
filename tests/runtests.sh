#!/usr/bin/env bash

for case in *.test; do
        ./../bin/calculator < "$case" > "${case%.*}.actual"
        cmp -s "${case%.*}.actual" "${case%.*}.expect"
        if [ "$?" -eq "1" ]; then
                echo "${case}: fail"
                diff -u1 "${case%.*}.actual" "${case%.*}.expect"
        else
                echo "${case}: pass"
        fi
        rm "${case%.*}.actual"
done
