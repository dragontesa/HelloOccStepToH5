##!/usr/bin/sh
if [ "$#" -ne 1 ]; then
  echo "STEP file is not specified"
  exit
elif [ -z $1 ];then
  echo "STEP file is not specified"
  exit
fi

in_stepfile=$1
out_h5file=""

# if [ ! -z $1 ];then
#   in_stepfile=$1
# fi

#out_h5file="${in_stepfile%.*}".".h5"
if [ -z $2 ];then
  out_h5file=${in_stepfile%.*}.".h5"
fi

echo "step file is "$in_stepfile
echo "hd5 file is "$out_h5file

export LD_LIBRARY_PATH=/opt/occt/7.8.1/lib:/opt/hdf5/1.14/lib
./build/HelloOccStepToH5 $in_stepfile $out_h5file
./build/HelloOccStepToH5 $in_stepfile $out_h5file