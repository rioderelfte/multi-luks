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

    config file:
        A file which describes which devices to encrypt and how to 
        generate the passphrases. The file has to be in etc/multi-luks//

    available configurations:
        - sample
