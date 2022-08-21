echo -e "Enter file name : \c"
read file_name

if [ ! -f $file_name ]
then
    echo "File does not exist"
    exit 0
fi

echo -e "Enter string to search : \c"
read string

count=$(grep -o $string $file_name | wc -l)

if [ $count -eq 0 ]
then
    echo "string not found"
    exit 0
else
    echo "string found $count times"
fi


arr=(`grep -o -n $string $file_name | cut -d : -f 1 | uniq -c`)

echo "------------------------------------"
echo -e "\tLine number \tFrequency"
echo "------------------------------------"

for(( i=0; i<${#arr[@]}; i+=2 ))
do 
	echo -e "\t${arr[$i+1]} \t\t${arr[$i]}"
done
echo "------------------------------------"
echo

echo "Enter new string to replace '$string' with : "
read new_string

sed -i "s|$string|$new_string|g" $file_name






