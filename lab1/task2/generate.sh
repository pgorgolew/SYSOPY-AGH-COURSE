#!/bin/bash

for((i=1; i<=10; i++))
do
    dd if=/dev/urandom of=large$i.txt bs=100M count=1
    dd if=/dev/urandom of=medium$i.txt bs=50M count=1
    dd if=/dev/urandom of=small$i.txt bs=10M count=1
done

echo "" > empty.txt