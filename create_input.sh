#!/bin/bash

# Check number of arguments
if [ $# -ne 2 ]
    then if [ $# -gt 2 ]  
        then echo "Too many arguments. Try again."
        exit 1
    elif [ $# -lt 2 ]
        then echo "Not enough arguments. Try again."
        exit 1
    fi
fi

politicalParties="$1"
numLines="$2"

# Check if numLines is a valid number
if [ $numLines -lt 0 ]
    then echo "Please give positive number."
	exit 1
fi

# Check if the file exists and if it is a regular file
if [ ! -e $politicalParties ] 
    then echo "File politicalParties.txt does not exist."
    exit 1
elif [ ! -f $politicalParties ]
    then echo "File politicalParties.txt is not a regular File."
    exit 1
fi

# Must have read rights
if [ ! -r $politicalParties ] 
    then echo "Dont have read rights"
    exit 1
fi

# Creating array of all parties politicalParties.txt has
line_count=0
parties=()

exec < "$1" # Redirect standard input to the file politicalParties.txt
while read line
do
    line_count=$(($line_count+1))
    parties+=($line)
done

# Create inputFile file and check if exists
inputFile="inputFile.txt"
touch "$inputFile"

# Check existance of the new file
if [ ! -e $inputFile ] 
    then echo "File inputFile.txt does not exist."
    exit 1
elif [ ! -f $inputFile ]
    then echo "File inputFile.txt is not a regular File."
    exit 1
fi

# Check if already has data. Wipe them in case it has
truncate -s 0 "$inputFile"

# Arrays with every letter depending on the category (Upper, Lower, Vowel, Non vowel)
upper_vowels=("A" "E" "I" "O" "U")
upper_non_vowels=("B" "C" "D" "F" "G" "H" "J" "K" "L" "M" "N" "P" "Q" "R" "S" "T" "V" "W" "X" "Y" "Z")

lower_vowels=("a" "e" "i" "o" "u")
lower_non_vowels=("b" "c" "d" "f" "g" "h" "j" "k" "l" "m" "n" "p" "q" "r" "s" "t" "v" "w" "x" "y" "z")

# Filling inputFile.txt with names-surnames and parties ($2 is the number of wanted lines)
for ((i=0; i<$2; i++))
do
    # Name assemble

    name=""
    name_lenght=$((RANDOM % 10 + 3))    # Randomly picking the lenght of the name
    random_name_start=$((RANDOM % 2 + 1))   # Randomly if the name will start with vowel or not

    if [ $random_name_start -eq 1 ]
        then random_upper=$((RANDOM % ${#upper_vowels[@]}))
        name+="${upper_vowels[random_upper]}"
    elif [ $random_name_start -eq 2 ]
        then random_upper=$((RANDOM % ${#upper_non_vowels[@]}))
        name+="${upper_non_vowels[random_upper]}"
    fi  

    for ((j=0; j<name_lenght - 1; j++))
    do
        vowel_or_non=$((RANDOM % 10 + 1))    # Pick what will be the next letter (vowel or not)
        if [ $vowel_or_non -le 6 ]
            then random_lower=$((RANDOM % ${#lower_vowels[@]}))
            name+="${lower_vowels[random_lower]}"
        elif [ $vowel_or_non -gt 6 ]
            then random_lower=$((RANDOM % ${#lower_non_vowels[@]}))
            name+="${lower_non_vowels[random_lower]}"
        fi           
    done        

    # Surname assemble

    surname="" 
    surname_lenght=$((RANDOM % 10 + 3)) # Randomly picking the lenght of the name
    random_surname_start=$((RANDOM % 2 + 1))    # Randomly if the name will start with vowel or not

    if [ $random_surname_start -eq 1 ]
        then random_upper=$((RANDOM % ${#upper_vowels[@]}))
        surname+="${upper_vowels[random_upper]}"
    elif [ $random_surname_start -eq 2 ]
        then random_upper=$((RANDOM % ${#upper_non_vowels[@]}))
        surname+="${upper_non_vowels[random_upper]}"
    fi  
    
    for ((j=0; j<name_lenght - 1; j++))
    do
        vowel_or_non=$((RANDOM % 10 + 1))   # Pick what will be the next letter (vowel or not)
        if [ $vowel_or_non -le 6 ]                                
            then random_lower=$((RANDOM % ${#lower_vowels[@]}))
            surname+="${lower_vowels[random_lower]}"
        elif [ $vowel_or_non -gt 6 ]
            then random_lower=$((RANDOM % ${#lower_non_vowels[@]}))
            surname+="${lower_non_vowels[random_lower]}"
        fi           
    done  

    # Party pick

    random_party=$((RANDOM % line_count))   # Picking random party
    party="${parties[random_party]}"

    echo "$name $surname $party" >> "$inputFile"
done