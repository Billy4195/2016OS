#!bin/bash

echo "untrack/input.txt" | ./quicksort
diff output1.txt untrack/output.txt > /dev/null
if [ $? -eq 1 ]
then
  echo "ST failed"
else
  echo "ST success"
fi
diff output2.txt untrack/output.txt > /dev/null
if [ $? -eq 1 ]
then
  echo "MT failed"
else
  echo "MT success"
fi
