#include <exception>

struct FileNotFoundException : public std::exception {
    const char *what() const throw() {
        return "File was not found!";
    }
};