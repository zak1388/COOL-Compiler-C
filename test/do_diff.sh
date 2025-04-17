#!/bin/bash

echo "" > results.txt
for file in out/*; do 
    file=$(basename $file); 
    out=$(diff -q {out,ref}/$file); 
    let $? && echo $out && echo $out >> results.txt; 
done
