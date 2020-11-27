# Documentation

## Multithreading

A thread is used for each new connection the server receives. With ".detach()" we allow them to execute independently from the main thread.

## Saving the messages

The messages get saved in a folder called "mails" in the specified server directory as a raw json file if the encryption is not enabled.
For each receiver a symlink to the message gets created in "users/<username>" and for the sender a symlink gets created in "users/<username>/sent".
When deleting a messing, just th symlink gets deleted to allow the sender and other receivers to still access the content.

## Ban list

When a user tries to connect or send a login request and he is banned to connection gets instantly closed from the server.
An IP counts as banned when it has three failed attempts and the last attempt is not older than 30 minutes.
If a user enters a wrong password for the first time his IP will be added to a json file with the current timestamp and the failed attempts.
For each additional attempt the failed attempts will be increased.

## Request handling

A class is used for each possible request type which contains a method get the keyword of the request, one that checks if the request is valid and the logic for processing the request.
In the requestProcessor.hpp the keyword of the incoming request gets checked against all allowed request keywords and the corresponding logic gets executed.

## Encryption

We use an external library called plusaes for encrypting the messages at rest. For that another parameter with the password is added to the server startup.
This password is used to encrypt / decrypt every message that is stored inside the mail spooling directory.
The encrypted data is saved as each byte converted to a number with a space as a separator.

Other possibilities not implemented in our project are:

### In Transit

Encryption in Transit could be implemented using TLS. This way, directly after opening the socket a TLS handshake would be performed, after which the communication could be encrypted.

### End to end

End to end encryption is technically possible, if you e.g. perform a Diffie–Hellman key exchange between the sender and receivers, after which the server would only act as a intermediary storage, without the ability to decrypt the files.

## Protocol

### Login
Request:
```
Login\n
<Username max. 8 characters>\n
<Password>\n
```

OK response
```
OK\n
```

Error response
```
ERR\n
```

### Send
Request:
```
SEND\n
<Receiver 0 max. 8 characters> <Receiver 1> .. <Receiver N>\n
<Subject max. 80 characters>\n
<Content>\n
.\n
```

OK response
```
OK\n
```

Error response
```
ERR\n
```

### List
Request:
```
LIST\n
```

OK response
```
OK\n
<Amount of messages>\n
<UUID4 message 0> <Subject message 0>\n
<UUID4 message 1> <Subject message 1>\n
...
<UUID4 message N> <Subject message N>\n
```

Error response
```
ERR\n
```

### List sent
Request:
```
LIST_SENT\n
```

OK response
```
OK\n
<Amount of messages>\n
<UUID4 message 0> <Subject message 0>\n
<UUID4 message 1> <Subject message 1>\n
...
<UUID4 message N> <Subject message N>\n
```

Error response
```
ERR\n
```

### Read
Request:
```
READ\n
<UUID4 message>\n
```

OK response
```
OK\n
<message sender>\n
<message receivers>\n
<message subject>\n
<message content>\n
```

Error response
```
ERR\n
```

### Delete
Request:
```
DEL\n
<UUID4 message>\n
```

OK response
```
OK\n
```

Error response
```
ERR\n
```