#!/bin/bash

cd train/
for x in *.jpeg; do (echo -n "$x "; convert $x -gravity Center -crop 50%x50% - | identify -verbose - > zz; grep mean zz|head -n 3 |cut -c 13-19|cut -d " " -f 1; grep geom zz|tr "x+" " " |cut -d " " -f 5-6) |paste - - - - - ; done | tee ../meanColor_Size_Train
cd ..

cd test/
for x in *.jpeg; do (echo -n "$x "; convert $x -gravity Center -crop 50%x50% - | identify -verbose - > zz; grep mean zz|head -n 3 |cut -c 13-19|cut -d " " -f 1; grep geom zz|tr "x+" " " |cut -d " " -f 5-6) |paste - - - - - ; done | tee ../meanColor_Size_Test
cd ..

# Commented out as files are already created and images at scale 1000 are not needed.
cd 1000_train/
for x in *.jpeg; do identify -verbose $x|grep standard|tail -n 1|sed "s/standard deviation/$x/" |tr -d :|cut -d " " -f 7,8 ; done | tee ../sd_Train
cd ..

cd 1000_test/
for x in *.jpeg; do identify -verbose $x|grep standard|tail -n 1|sed "s/standard deviation/$x/" |tr -d :|cut -d " " -f 7,8 ; done | tee ../sd_Test
cd ..

cd 1000_train/
for x in *.jpeg; do convert $x -gravity Center -crop 50%x50% -define convolve:scale='!' -bias 50% -morphology Convolve Laplacian:1 - | identify -verbose - |grep standard|tail -n 1|sed "s/standard deviation/$x/" |tr -d :|cut -d " " -f 7,8 ; done | tee ../noise_Train
cd ..

cd 1000_test/
for x in *.jpeg; do convert $x -gravity Center -crop 50%x50% -define convolve:scale='!' -bias 50% -morphology Convolve Laplacian:1 - | identify -verbose - |grep standard|tail -n 1|sed "s/standard deviation/$x/" |tr -d :|cut -d " " -f 7,8 ; done | tee ../noise_Test
cd ..
