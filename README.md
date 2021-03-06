Sendfile Shim
============

Used to fix weird systems that have a broken sendfile(2).


Usage:

- make
- put `sendfile-shim.so` somewhere that can be accessed by all users, e.g. /var/sendfile-shim/sendfile-shim.so
- echo "/var/sendfile-shim/sendfile-shim.so" > /etc/ld.so.preload
