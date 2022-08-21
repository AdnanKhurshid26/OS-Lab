echo "Enter path of the directory: "
read direc

echo -e "Number of files in current directory and its subdirectories(excluding directories): " \c
find $direc -type f | wc -l

echo -e "Number of files in current directory and its sub-directories (including directories): " \c
find $direc | wc -l