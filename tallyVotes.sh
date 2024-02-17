#!/bin/bash

# Check number of arguments
if [ $# -ne 1 ]
    then if [ $# -gt 1 ]  
        then echo "Too many arguments. Try again."
        exit 1
    elif [ $# -lt 1 ]
        then echo "Not enough arguments. Try again."
        exit 1
    fi
fi

# Check if the file inputFile exists and if it is a regular file
inputFile="inputFile.txt"

if [ ! -e $inputFile ] 
    then echo "File inputFile.txt does not exist."
    exit 1
elif [ ! -f $inputFile ]
    then echo "File inputFile.txt is not a regular File."
    exit 1
fi

# Must have read rights on inputFile.txt
if [ ! -r $inputFile ] 
    then echo "Dont have read rights for inputFile.txt"
    exit 1
fi

# Create inputFile file
tallyResultsFile="$1"
touch "$tallyResultsFile"

# Check existance of the new file
if [ ! -e $tallyResultsFile ] 
    then echo "File tallyResultsFile.txt does not exist."
    exit 1
elif [ ! -f $tallyResultsFile ]
    then echo "File tallyResultsFile.txt is not a regular File."
    exit 1
fi

# Check if already has data. Wipe them in case it has
truncate -s 0 "$tallyResultsFile"

# Creating array of all uniques parties
line_count=0
unique_parties=()
unique_parties_count=0

exec < "$inputFile" # Redirect standard input to the file inputFile.txt
while read line
do
    index=($line)
    party="${index[2]}" # The third string is the party

    if [[ ! " ${unique_parties[*]} " =~ " $party " ]]
        then unique_parties+=("$party")
        unique_parties_count=$(($unique_parties_count+1))
    fi
    line_count=$(($line_count+1))
done

# Write to the file the unique parties with their total votes
total_votes=0
already_voted=()
for ((i=0; i<$unique_parties_count; i++))
do
    party_votes=0

    exec < "$inputFile" # Redirect standard input to the file inputFile.txt
    while read line
    do
        index=($line)
        party="${index[2]}" # The third string is the party
        name_surname="${index[0]} ${index[1]}"

        if [[ ${unique_parties[i]} == $party ]] && [[ ! " ${already_voted[*]} " =~ " $name_surname " ]]
            then party_votes=$(($party_votes+1))
            already_voted+=($name_surname)
        fi
    done

    echo "${unique_parties[i]} $party_votes" >> "$tallyResultsFile"
    total_votes=$(($total_votes+party_votes))
done

echo "TOTAL $total_votes" >> "$tallyResultsFile"