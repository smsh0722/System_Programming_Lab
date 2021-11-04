#!/bin/bash
make

a=$(( $1 + $2))
b=$(( $1 - $2))
echo $a $b > answer.txt

echo $1 $2 | ./w3 > output.txt

diff answer.txt output.txt > result.txt
make clean


