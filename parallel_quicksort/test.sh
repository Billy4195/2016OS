#!bin/bash

echo "untrack/input.txt" | ./quicksort
for out in ./output* 
do
  diff $out untrack/output.txt > /dev/null
  if [ $? -eq 1 ]
  then
    echo  $out failed
  else
    echo  $out success
  fi
done
