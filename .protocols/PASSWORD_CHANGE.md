## Changing root password

- [ ] Use the .HASHER Sub-Program to generate the first and second hashes
- [ ] If you want certain daemon to be remain authorized, remotely change
its hkey.dat file to the first hash
- [ ] Go to cloudflare secrets and set the value of DAEMON_KEY to the first hash
- [ ] Edit .dev.vars to store DAEMON_KEY secret variable locally
- [ ] Build, test and deploy


>[!NOTE]
>Note that the second hash is no longer used since `v3.4.2`. Until then, the second
>hash was needed to be hardcoded in `daemon/main.cpp` file. The daemon will verify the
>password before starting. This would make sure unauthorized daemons do not consume
>server time. But it was removed, to make the daemon more light-weight.
>Now the daemon shuts down only if the server returns a `401 (Unauthorized)` HTTP
>status. This also makes it possible to update the password from the server side
