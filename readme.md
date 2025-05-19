# Traffic Monitor, Sizing
The issue was raised on May 15th, 2024, edited by Nick.


## Development Environment
Here are the necessary tools for developing and compiling the project on RHEL8.7:

g++ (g++ (GCC) 8.5.0 20210514 (Red Hat 8.5.0-15)) \
gcc (GCC) 8.5.0 20210514 (Red Hat 8.5.0-15) \
GNU Make 4.2.1 \
cmake version 3.20.2 \
jq-1.6 \
GNU Awk 4.2.1 \
git \
git@github.com:vrogier/ocilib.git 
./configure --with-oracle_headers_path=/usr/include/oracle/19.10/client64 --with-oracle_lib_path=/usr/lib/oracle/19.10/client64/lib
root
\
rpm -ql oracle-instantclient19.10-devel-19.10.0.0.0-1.x86_64


oracle client development
https://yum.oracle.com/repo/OracleLinux/OL8/oracle/instantclient21/x86_64/


## Running Phase
The current working directory is the project root. Users can use make to compile the codes
and run the execution, users can use the command `make`. If someone do not run the codes, please use
the command, `make build`.

