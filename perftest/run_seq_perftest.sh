#!/bin/bash

#$1 -> program to be tested
#$2 -> input file
do_test() {
PROG=$1
INFILE=$2
NUMTHREADS=1
EVFLAGS=""
for EVFILE in `ls $HOSTDIR/*lst`
do
  for EVENT in `cat $EVFILE`
  do
    EVFLAGS=" -e $EVENT $EVFLAGS"
  done
done
echo "Running $PROG"

for NUMITER in `seq 100 100 2000`
do
  sudo /sbin/sysctl vm.drop_caches=3
  echo "Running $PROG $NUMTHREADS $NUMITER $INFILE"
  perf stat $EVFLAGS -o $HOSTDIR/`basename $PROG`.$NUMITER.perf -r10 $PROG $NUMTHREADS $NUMITER $INFILE 
done
}

#$1 -> Input File
HOSTDIR=`hostname`
do_test bin/fanimate $1 
do_test bin/animate $1 
