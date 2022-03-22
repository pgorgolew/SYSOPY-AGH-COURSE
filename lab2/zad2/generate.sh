#!/bin/bash

dd if=/dev/urandom of=50_kb.txt bs=50K count=1

touch test_300.txt test_600.txt test_900.txt

for((i=1; i<=6; i++))
do
    cat 50_kb.txt >> test_300.txt
    cat 50_kb.txt >> test_600.txt
    cat 50_kb.txt >> test_900.txt
done

for((i=1; i<=6; i++))
do
    cat 50_kb.txt >> test_600.txt
    cat 50_kb.txt >> test_900.txt
done

for((i=1; i<=6; i++))
do
    cat 50_kb.txt >> test_900.txt
done