#!/bin/bash
HOSTDIR=`hostname`
mkdir -p $HOSTDIR
echo "Creating directory: $HOSTDIR" 1>&2
perf list | grep "Hardware event" | awk '{print $1}' > $HOSTDIR/hwevents.lst
perf list | grep "Hardware cache event" | awk '{print $1}' > $HOSTDIR/cachevents.lst
perf list | grep "Software event" | awk '{print $1}' > $HOSTDIR/swevents.lst
