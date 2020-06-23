#!/bin/bash

cd ../src/
make clean all
cp ./history ../tests/temphistory
cd ../tests

for file in *.in.txt
do
  echo "Running test on file: $file"
  ./temphistory < $file > temp
  diff -w temp $(echo $file | sed "s/.in/.out/g") | grep "^>" | wc -l
done

rm temphistory temp