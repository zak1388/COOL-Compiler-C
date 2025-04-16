#!/bin/bash

rm out/*;

for file in ../../ECS652U-cw-group1/testsuite/lexer/*; do \
    ../build/lexer $file > out/$(basename $file).out 2>  out/$(basename $file).err & \
done
