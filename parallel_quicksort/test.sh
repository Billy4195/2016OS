#!bin/bash

echo "untrack/input.txt" | ./quicksort
diff output1.txt untrack/output.txt
if [ -z $? ]
then
  echo "ST failed"
else
  echo "ST success"
fi
