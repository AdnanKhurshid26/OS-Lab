echo "Enter first date of birth (dd/mm/yyyy): "
IFS='/' read -a D1 
date1=${D1[2]}-${D1[1]}-${D1[0]}
day1=$(date -d  $date1 +%A)


echo "Enter second date of birth (dd/mm/yyyy): "
IFS='/' read -a D2
date2=${D2[2]}-${D2[1]}-${D2[0]}
day2=$(date -d  $date2 +%A)

if [ $day1 = $day2 ]
then
    echo "Both dates are equal"
else
    echo "Both dates are not equal"
fi

cur_date=`date +%y-%m-%d`
echo "Current date is: $cur_date"
# # days1= $(date -d $cur_date +%s) - $(date -d $input1 +%s)

# # echo "Number of days between current date and first date of birth is: $days1"

#global variables
years=
months=
days=

calc_age () {
   start_date=$(date -d $1 '+%s')
   end_date=$(date -d $2 '+%s')
   days=$(( ( end_date - start_date )/(60*60*24) ))
    years=$(( days/365 ))
    days=$(( days%365 ))
    months=$(( days/30 ))
    days=$(( days%30 ))

}

calc_age $date1 $cur_date

echo "Age of Person 1 : $years years $months months $days days"

calc_age $date2 $cur_date

echo "Age of Person 2 : $years years $months months $days days"