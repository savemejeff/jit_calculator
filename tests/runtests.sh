#!/usr/bin/env bash

cp -r ./../bin ./bin/

for case in *.test; do
        touch "temp"
        while read -r line; do
                printf "$line\n" | ./../cal.sh >> "temp"
        done < "$case"
        cmp -s "temp" "${case%.*}.expect"
        if [ "$?" -eq "1" ]; then
                echo "${case}: fail"
                diff -u1 "temp" "${case%.*}.expect"
        else
                echo "${case}: pass"
        fi
        rm "temp"
done

rm -r ./bin
