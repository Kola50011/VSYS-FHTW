# Documentation

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

### Read
Request:
```
READ\n
<UUID4 message>\n
```

OK response
```
OK\n
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

### Quit
Request:
```
QUIT\n
```

OK response
```
OK\n
```

Error response
```
ERR\n
```