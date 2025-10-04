# Cleopatra

This is a light-weight backdoor access malware program made for educational purposes only

A program to execute any command on your PC remotely (PC have to be turned on).
Even though it is intended to be a educative malware, it can have a
lot of ethical uses as well. It can be used to remotely computer your PC 
and shut it down  on the event of a security threat, or because you forgot 
to turn it off before leaving your home for a 10 day trip to a different country. You 
can use your PC even if you are not in the physical vicinity of it.

The Cleopatra Service Daemon installed on your PC will connect with the Cleoptra 
worker server(hosted on a server) to receive jobs and run it and send back
the results. 

There are two APIs facilitating this service: `/api/fetch` and `/api/send`.
The daemon will send communicate with these 2 APIs. The daemon fetches
jobs from worker every 2 minutes and if there is a job available, the
daemon runs it and send back the output and exit code of the job

The daemon only uses less that 1 Mb RAM memory while running in the background


