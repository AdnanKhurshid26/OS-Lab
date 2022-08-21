#! /usr/bin/bash

echo "Enter name of file: "
read file_name

if [ -f $file_name ]
then
    echo "File exists"

    number_of_lines=`wc --lines < $file_name`

    echo "Number of lines in file: $number_of_lines"

else
    echo "File does not exist"
    echo "Created file"
    touch $file_name
    echo "Enter text to be written in file. Press Ctrl+D to exit"
    cat >> $file_name
fi