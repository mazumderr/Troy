#!/usr/bin/env bash

pwd
cd ../../../
pwd
ls
make
fileout="programming_assignment_6-test_file_"
mkdir temp/
for i in `seq 1 3`;
do
   ./a.exe io/in/assignment6/programming_assignment_6-test_file_$i.c > "temp/${fileout}${i}.c";
done

for i in `seq 1 3`;
do 
   diff "io/out/assignment6/${i}out.txt" "temp/programming_assignment_6-test_file_${i}.c";
   if [ $? != 0 ]; then
      exit 255
   fi
done
rm -rf temp/*
rmdir temp/
make clean
