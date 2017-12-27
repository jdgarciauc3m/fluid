do_test() {
PROG=$1
INFILE=$2
NUMTHREADS=1
NUMITER=500
EVFLAGS=""
for EVFILE in `ls $HOSTDIR/*lst`
do
  for EVENT in `cat $EVFILE`
  do
    EVFLAGS=" -e $EVENT $EVFLAGS"
  done
done
echo "Running $PROG"
/sbin/sysctl vm.drop_caches=3
echo "Running $PROG $NUMTHREADS $NUMITER $INFILE"
perf stat $EVFLAGS -o $HOSTDIR/`basename $PROG`.perf -r10 $PROG $NUMTHREADS $NUMITER $INFILE 
}

do_test_tbb() {
PROG=$1
INFILE=$2
NUMITER=500
EVFLAGS=""
for EVFILE in `ls $HOSTDIR/*lst`
do
  for EVENT in `cat $EVFILE`
  do
    EVFLAGS=" -e $EVENT $EVFLAGS"
  done
done
for NUMTHREADS in 1 2 4 8 16 32 64
do
  echo "Running $PROG wiht $NUMTHREADS threads"
  /sbin/sysctl vm.drop_caches=3
  echo "Running $PROG $NUMTHREADS $NUMITER $INFILE"
  perf stat $EVFLAGS -o $HOSTDIR/`basename $PROG`.$NUMTHREADS.perf -r10 $PROG $NUMTHREADS $NUMITER $INFILE 
done
}

#$1 -> Input File
HOSTDIR=`hostname`
do_test bin/fanimate $1 
do_test bin/animate $1 
do_test_tbb bin/fanimate_tbb $1
do_test_tbb bin/animate_tbb $1
