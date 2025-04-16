#!/bin/bash

for file in out/*; do file=$(basename $file); diff -q {out,ref}/$file; done
