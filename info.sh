#!/bin/bash

echo "Computer Name: $(hostname)"
echo "Operating System: $(uname)"
echo "Kernel version: $(lsb_release -a | grep "Description" | awk '{print $2, $3, $4}')"
echo "Kernel release: $(lsb_release -a | grep "Release"| awk '{print $2}')"
echo "Architecture: $(lscpu | grep "Architecture" | awk '{print $2}')"
echo "CPU: $(lscpu | grep 'Model name' | awk '{print $3, $4, $5}')"
echo "RAM: $(free -h | awk '/^Mem/ {print $2}')"
echo "Memory: $(free -h | awk '/^Mem/ {print $3}')"
echo -e "IP Address:\t\n$(ip a | grep -E "inet" | awk '{print "\t"$2}')"
minUID=`grep -E '^UID_MIN' /etc/login.defs | awk '{print $2}'`
maxUID=`grep -E '^UID_MAX' /etc/login.defs | awk '{print $2}'`
user=`awk -F: -v minUID="$minUID" -v maxUID="$maxUID" '{if ($3 >= minUID && $3 <= maxUID) print $1}' /etc/passwd`
sorted_user=$(echo $user | tr ' ' '\n' | sort | tr '\n' ',')
echo -e "User:\t$sorted_user"
echo -e "Username:\t$(whoami)"
echo -e "Process:\t\n$(ps -uax | grep "^root"| tr -d "[]" | awk 'BEGIN{print"\tPID\tCOMMAND"}{print "\t"$2"\t"$11}')"
port=`sudo ss -tulpn | grep -E "^u|^t"| awk '{print "\t"$1"\t"$2"\t"$5"\t"$7}'`
listport=$(echo "$port" | grep -oE ":[[:digit:]]+"| tr -d ":" | sort | uniq | tr "\n" ",") 
echo -e "Port Opening: $listport\t\n$port"

