==========
multi-luks
==========

About
=====

multi-luks is a shell script, which makes it possible to encrypt
multiple partitions with LUKS__ using the same password. It does this
by hashing the password with different hash algorithms multiple times
and passing intermediate results as passwords to the different LUKS
partitions. The script can be freely configured which hashing
algorithms to use, how many times to apply them and which salt to
use.

It has the advantage over decrypt_derived, that one does not have
access to all partitions if one can crack the master key of the first
partition in the chain. If one simply uses the same password for all
partitions one has access to all data if one breaks this one
password. With the different hash algorithms used in multi-luks, this
task becomes a bit harder.

__ http://code.google.com/p/cryptsetup/

Warning
=======

If you use this script you should have basic knowledge about
cryptography, hashing functions and LUKS. You should understand what
this script does and probably you should read and understand the whole
script before you actually use it.

Requirements
============

As multi-luks is a bash script you need version 4.0 of the bash or
later. In addition you need LUKS. If have tested the script with
version 1.1.0 of LUKS, but it should work with previous versions,
too. Of course you need to have the crypto algorithms you want to use
enabled in your kernel. To calculate the hash values multi-luks uses
openss. I have tested it with openssl version 0.9.8, but it should
work with previous versions, too.


Installation
============

You just need to copy the script ``bin/multi-luks`` somewhere to your
path and make it executable. By default multi-luks searchs in
``/etc/multi-luks`` for configuration files, but you can overwrite
this path at the command line. You can have multiple configuration
files in this directory if you have multiple disc arrays. You have to
specify the name of the configuration you want to work with when
invoking multi-luks from the command line.

You can find a documented sample configuration in the ``etc/``
directory of this repository.

multi-hash
==========

If you want to run the hash functions more the a dozen times, it will
take very long to calculate the hashes. This is because for every
single iteration the openssl executable is called.

I implemented the repeated hashing of one password with the same
algorithm and the same salt in C++. You can find the source code in
the ``src/`` directory. If you compile this program and put it in your
path, multi-luks will automatically find and use it. This will speed
up the hash calculations rapidly, if you use a big number of
repititions::

        $ # calling the batch function:
        $ time echo -n 'password' | multi-hash sha1 1000 'some salt'
        2f19781ec96834c7039e9b26a5833dc32f4b43f0

        real    0m4.873s
        user    0m16.600s
        sys     0m2.980s

        $ time echo -n 'password' | multi-hash sha1 10000 'some salt'
        6971b6aa73e6e5ac2c8fb6c3dcae32389f8e5668

        real    0m50.715s
        user    2m48.290s
        sys     0m31.780s

        $ # calling the compiled executable:
        $ time echo -n 'password' | ./multi-hash sha1 1000 'some salt'
        2f19781ec96834c7039e9b26a5833dc32f4b43f0

        real    0m0.005s
        user    0m0.000s
        sys     0m0.010s

        $ time echo -n 'password' | ./multi-hash sha1 10000 'some salt'
        6971b6aa73e6e5ac2c8fb6c3dcae32389f8e5668

        real    0m0.015s
        user    0m0.020s
        sys     0m0.000s

You can build the executable by simply running make in the ``src/``
directory. You will need ``g++`` and ``openssl`` installed. On Ubuntu
you need the development version of the ssl library: ``libssl-dev``.

Usage
=====

If you want to execute multi-luks you have to give it a command and a
config file. The command can be one of:

- format: Asks you twice for the password and runs ``cryptsetup
  luksFormat`` on all configured partitions.
- open: Asks you for your password and decrypts the configured
  partitions.
- close: Closes all configured partitions using ``cryptsetup
  luksClose``.
- backup: Backup the luks header, partition table (if available) and the
  corresponding configuration file to a bz2 compressed tar archive in the
  current working directory.

Optionally you can overwrite the default config path
(``/etc/multi-luks``) with the ``--config-path`` option.

If you invoke multi-luks without any parameters, it prints a short
help message and the available configurations::

    $ multi-luks
    Usage: multi-luks [--config-dir|-c <dir>] <command> <config-file>

    options:
        --config-dir: Specify the directory to search for config files

    commands:
        format: Set up the encryption as descriped in the config file
        open:   Open the devices as described in the config file
        close:  Close all given mappings
        backup: Backup all relevant information of a given config file

    config file:
        A file which describes which devices to encrypt and how to 
        generate the passphrases. The file has to be in /etc/multi-luks/

    available configurations:
        - sample
