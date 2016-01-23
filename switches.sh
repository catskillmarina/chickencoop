#!/usr/pkg/bin/bash

while [ 1 ]
do
    clear
    echo "Turn relays on Enter o"
    echo "Turn relays off enter k"
    echo "Exit q"
    echo
    echo -n "--> "
    read choice
    if [ $choice == 'k' ]
    then
        clear
        turnoff=sasdfsad
        while [ $turnoff != "q" ]
        do
            clear
            echo "Enter 1 to turn off relay 1"
            echo "Enter 2 to turn off relay 2"
            echo "Enter 3 to turn off relay 3"
            echo "Enter 4 to turn off relay 4"
            echo "echo q to exit"
            echo
            echo -n "---> "
            read turnoff
            if [ $turnoff == "1" ]
            then
                ./turn-off-1
            fi
            if [ $turnoff == "2" ]
            then
                ./turn-off-2
            fi
            if [ $turnoff == "3" ]
            then
                ./turn-off-3
            fi
            if [ $turnoff == "4" ]
            then
                ./turn-off-4
            fi
        done 
    fi

    if [ $choice == 'o' ]
    then
        clear
        turnon=sasdfsad
        while [ $turnon != "q" ]
        do
            clear
            echo "Enter 1 to turn on relay 1"
            echo "Enter 2 to turn on relay 2"
            echo "Enter 3 to turn on relay 3"
            echo "Enter 4 to turn on relay 4"
            echo "echo q to exit"
            echo
            echo -n "---> "
            read turnon
            if [ $turnon == "1" ]
            then
                ./turn-on-1
            fi
            if [ $turnon == "2" ]
            then
                ./turn-on-2
            fi
            if [ $turnon == "3" ]
            then
                ./turn-on-3
            fi
            if [ $turnon == "4" ]
            then
                ./turn-on-4
            fi
        done 
    fi

    if [ $choice == 'q' ]
    then
        exit 0
    fi
done
