# Cleopatra

This is a light-weight backdoor remote access program

A program to execute any command on your PC remotely (PC have to be turned on).
Even though it is intended to be a educative malware. It can be used to remotely computer your PC 
and shut it down  on the event of a security threat. You 
can use your PC even if you are not in the physical vicinity of it.

The Cleopatra Daemon installed on your PC will connect with the Cleoptra 
backend server to receive jobs and run it and send back
the results

There are two APIs facilitating this service: `/api/fetch` and `/api/send`.
The daemon will send communicate with these 2 APIs. The daemon fetches
jobs from worker every 2 minutes and if there is a job available, the
daemon runs it and send back the output and exit code of the job

The daemon only uses less that 1 Mb RAM memory while running in the background
