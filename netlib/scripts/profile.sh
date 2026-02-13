#!/bin/bash
set -ex
make all
mkdir -p log
sudo dtrace -x ustackframes=100 -n 'profile-997 /pid == $target/ { @[ustack()] = count(); }' -o log/out.stacks -c "$*"
stackcollapse.pl log/out.stacks > log/out.folded
flamegraph.pl log/out.folded > log/flamegraph.svg
