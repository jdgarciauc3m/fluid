#!/bin/bash

#do_test
#$1 -> program to be measured
#$2 -> input_file
do_test() {
PROG=$1
INFILE=$2
EVFLAGS=""
for EVFILE in `ls $HOSTDIR/*lst`
do
  for EVENT in `cat $EVFILE`
  do
    EVFLAGS=" -e $EVENT $EVFLAGS"
  done
done
echo "Running $PROG"

NUMITER=500
for NUMTHREADS in 1 2 4 8 16 32 64 128
do
  sudo /sbin/sysctl vm.drop_caches=3
  echo "Running $PROG $NUMTHREADS $NUMITER $INFILE"
  perf stat $EVFLAGS -o $HOSTDIR/`basename $PROG`.$NUMITER.$NUMTHREADS.perf -r10 $PROG $NUMTHREADS $NUMITER $INFILE 
done
}

#$1 -> Input File
HOSTDIR=`hostname`
do_test bin/fanimate_tbb $1 
do_test bin/animate_tbb $1 
