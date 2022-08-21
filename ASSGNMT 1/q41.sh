path="deleted_files/"

read -p "Enter the filename to be deleted, enter q " f

if [ "q" == "$f" ]
then
	rm -r $path
    mkdir $path
    exit 0
fi


filename=$(echo $f|rev|cut -d'/' -f1|rev)

extension=$(echo $filename|rev|cut -d'.' -f1|rev)
name=$(echo $filename|cut -d'.' -f1)

count=0
for n in $(ls $path|rev|cut -d'/' -f1|rev|cut -d'.' -f1|cut -d'_' -f1)
do
    if [ "$name" == "$n" ]
    then 
        count=$((count+1))
    fi
done

final_name=$name"_"$count"."$extension

if [ -f "$f" ]
then
    mv $f $final_name
    mv $final_name $path
else
    echo "Enter valid filename: "
fi