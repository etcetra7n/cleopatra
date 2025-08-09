# Hasher Sub-Program

Use this tool to generate the first and second hashes required for
changing the password of the daemon

### How to Use
Simply run `make_build_RELEASE.bat` and run `.HASHER.exe`. 
`.HASHER.exe` will prompt you to enter the password you wish to 
set, and it will generate the first and second hash. Use these 
hashes as specified in the protocol

### About generated hashes
The program generates two hashes. One is `argon2id`, with a 
prefixed salt string. The other is bcrypt hash, made on the
first hash

When user first use the software, the first hash is created and
is saved in something like `auth.dat` in the local PC. Each
time the software is launched this hash is verified using the
bcrypt hash, hardcoded into the code of the software

For server side verification, the first hash can be hardcoded
into the server. The key sent by the user is compared using
simple string comparison to verify authenticity
