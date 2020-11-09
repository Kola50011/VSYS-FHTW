# TWMailer

## Coding Conventions

* CamelCase
* Variables
    * start small
* Classes
    * start with a capital letter
* Macro Names
    * CAPITAL_WITH_UNDERSCORE
    + e.g. `#define THIS_IS_A_NUMBER 1`

## Building

```bash
mkdir build
cd build
cmake ..
make all
```

## Usage

```bash
./server
./client
```

## Encryption?

### At Rest

Encryption at Rest would be possible, if another parameter is added to the server startup. E.g. a private key, or a password.
This could then be used to encrypt / drecrypt every message that is stored inside the mailspooling directory.

### In Transit

Encryption in Transit could be implemented using TLS. This way, directly after opening the socket a TLS handshake would be performed, after which the communication could be encrypted.

### End to end

End to end encryption is technically possible, if you e.g. perform a Diffie–Hellman key exchange between the sender and receivers, after which the server would only act as a intermediary storage, without the ability to decrypt the files.