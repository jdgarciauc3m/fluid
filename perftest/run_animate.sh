do_test() {
PROG=$1
INFILE=$2
for ITERATIONS in `seq 10 10 100`
do
  KTIME=`$PROG 1 $ITERATIONS $2 | grep time | sed 's/Simulation time: //'`
  echo animate $ITERATIONS ' ' $KTIME
done
}

do_test_tbb() {
PROG=$1
INFILE=$2
for ITERATIONS in `seq 10 10 100`
do
  for THREADS in 1 2 4 8 16
  do
    KTIME=`$PROG $THREADS $ITERATIONS $2 | grep time | sed 's/Simulation time: //'`
    echo animate $ITERATIONS ' ' $KTIME
  done
done
}

do_test bin/fanimate $1
do_test bin/animate $1
do_test_tbb bin/fanimate_tbb $1
do_test_tbb bin/animate_tbb $1
