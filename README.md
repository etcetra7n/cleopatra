# Cleopatra

A program to execute any command on your PC remotely (PC have to be turned on)

The Cleopatra Service Daemon installed on your PC will connect with a Cleoptra 
worker service (hosted on cloudlare) to receive jobs and run it and send back
the results. 

There are two APIs facilitating this service: `/api/fetch-daemon-job` and 
`/api/send-results-to-worker`. The daemon will send communicate with these 
2 APIs. The daemon fetches jobs from worker every 2 minutes and if there is
a job available, the daemon runs it and send back the output and exit code 
of the job
