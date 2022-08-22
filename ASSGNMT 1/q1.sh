echo -e "Enter 1st number : \c"
read a

echo -e "Enter 2nd number : \c"
read b

echo  "$a,$b"

echo "$b and $a"

echo "Numbers reveresed : "

echo "$(echo $a|rev) and $(echo $b|rev)"