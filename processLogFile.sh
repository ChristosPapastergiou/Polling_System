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

# Check if the file pollLog exists and if it is a regular file
pollLog="$1"

if [ ! -e $pollLog ] 
    then echo "File pollLog.txt does not exist."
    exit 1
elif [ ! -f $pollLog ]
    then echo "File pollLog.txt is not a regular File."
    exit 1
fi

# Must have read rights on pollLog.txt
if [ ! -r $pollLog ] 
    then echo "Dont have read rights for pollLog.txt"
    exit 1
fi

# Create inputFile file
pollerResultsFile="pollerResultsFile.txt"
touch "$pollerResultsFile"

# Check existance of the new file
if [ ! -e $pollerResultsFile ] 
    then echo "File pollerResultsFile.txt does not exist."
    exit 1
elif [ ! -f $pollerResultsFile ]
    then echo "File pollerResultsFile.txt is not a regular File."
    exit 1
fi

# Check if already has data. Wipe them in case it has 
truncate -s 0 "$pollerResultsFile"

# Creating array of all uniques parties pollLog has
line_count=0
unique_parties=()
unique_parties_count=0

exec < "$pollLog"    # Redirect standard input to the file pollLog.txt
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
for ((i=0; i<$unique_parties_count; i++))
do
    party_votes=0
    
    exec < "$pollLog"   # Redirect standard input to the file pollLog.txt
    while read line
    do
        index=($line)
        party="${index[2]}"  # The third string is the party

        if [[ ${unique_parties[i]} == $party ]]
            then party_votes=$(($party_votes+1))
        fi
    done

    echo "${unique_parties[i]} $party_votes" >> "$pollerResultsFile"
    total_votes=$(($total_votes+party_votes))
done

echo "TOTAL $total_votes" >> "$pollerResultsFile"