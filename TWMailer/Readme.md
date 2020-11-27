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
./server -s ./Mails -p 8080
./client -a 127.0.0.1 -p 8080
```

## Docker
The server is also available as a minimal alpine docker image.

```bash
cd docker
docker-compose up --build
```