# System programming 

This project aims to build a polling system using 
- Sockets : communication between the polling server and clients, enabling real-time data exchange. 
- Threads : handling concurrent operations, allowing the server to manage multiple client connections simultaneously
- Interprocess communication (IPC) methods : communication between different processes, contributing to the overall efficiency and reliability of the system
   
# Compilation & run

C programs : 

    compile : make
    compile & run server : make valgrindPoller
    compile & run client : make pollSwayer

Bash programs : 

    ./create_input.sh politicalParties.txt 100
    ./tallyVoters.sh tallyResultsFile.txt
    ./processLogFile.sh pollLog.txt
