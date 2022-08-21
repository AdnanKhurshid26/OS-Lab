while true
do
	echo "Enter Ctr+C to exit"
    read -a arr
    if [ ${arr[0]} != "rm" ]
    then
        echo "Invalid command"
        exit 0
    fi
    
    if [ ${arr[1]} == "-c" ]
    then
        echo -e "Are you sure you want to clear the deleted files?(y/n)"
        read -n 1 ans
        if [ $ans == "y" ]
        then
            rm ./my_deleted_files/*
            echo "Files deleted"
        else
            echo "Files not deleted"
        fi
        exit 0
    else 
        file_to_del=${arr[1]}
    fi
    
    mv $file_to_del /my_deleted_files
    

    
done