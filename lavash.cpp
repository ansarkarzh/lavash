#include <unistd.h>

int main(int argc, char **argv, char **envv) {
    execvp("bash", argv);  /// Your code here
}
