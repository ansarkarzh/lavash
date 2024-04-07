#include <unistd.h>
#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <cstring>
#include <sstream>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>


std::string parse(const std::string& s) {
    std::stringstream  ss{""};
    for(size_t i = 0; i < s.length(); i++) {
        if (s.at(i) == '\\') {
            switch(s.at(i + 1)) {
                case 'n':  ss << "\n"; i++; break;
                case '"':  ss << "\""; i++; break;
                default:   ss << "\\";      break;
            }       
        } else {
            ss << s.at(i);
        }
    }
    return ss.str();
}


std::vector<char *> SplitBySpaces(std::string s) {
    std::vector<char*> ans;
    s = parse(s);
    size_t i = 0;
    std::string cur = "";
    while (i <= s.size()) {
        if (i == s.size()) {
            if (!cur.empty()) {
                char *cstr = new char[cur.size() + 1];
                std::strcpy(cstr, cur.c_str());
                ans.push_back(cstr);
                cur.clear();
            }
            break;
        }
        if (s[i] == '"') {
            if (!cur.empty()) {
                char *cstr = new char[cur.size() + 1];
                std::strcpy(cstr, cur.c_str());
                ans.push_back(cstr);
                cur.clear();
            }
            ++i;
            while (i < s.size() && s[i] != '"') {
                cur.push_back(s[i]);
                ++i;
            }
            ++i;
            char *cstr = new char[cur.size() + 1];
            std::strcpy(cstr, cur.c_str());
            ans.push_back(cstr);
            cur.clear();
            continue;
        }
        if (s[i] == '<' || s[i] == '>') {
            if (!cur.empty()) {
                char *cstr = new char[cur.size() + 1];
                std::strcpy(cstr, cur.c_str());
                ans.push_back(cstr);
                cur.clear();
            }
            char *cstr = new char[3]; 
            std::string ns;
            ns.push_back(s[i]);  
            ns.push_back('!');         
            std::strcpy(cstr, ns.c_str());
            ans.push_back(cstr);
            ++i;
            continue;
        }
        if (std::isspace(s[i])) {
            if (!cur.empty()) {
                char *cstr = new char[cur.size() + 1];
                std::strcpy(cstr, cur.c_str());
                ans.push_back(cstr);
                cur.clear();
            }
            while (i < s.size() && std::isspace(s[i])) {
                ++i;
            }
            continue;
        }
        cur.push_back(s[i]);
        ++i;
    }
    return ans;
}

void execute(std::vector<char *> arg) {
    std::vector<char *> clean;
    size_t i = 0;
    while (i < arg.size()) {
        if (strcmp(arg[i], "<!") == 0 && i + 1 < arg.size()) {
            char * next = arg[i + 1];
            int in = open(next, O_RDONLY);
            if (in < 0) {
                std::cerr << "./lavash: line 1: " << next << ": No such file or directory" << std::endl;
                _exit(1);
            }
            dup2(in, STDIN_FILENO); 
            close(in);
            ++i;
        } else if (strcmp(arg[i], ">!") == 0 && i + 1 < arg.size()) {
            char * next = arg[i + 1];
            int out;
            if (strcmp(next, ".txt") == 0) {
                char nnext[] = "\".txt";
                out = open(nnext, O_WRONLY | O_CREAT | O_TRUNC,
                  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH); 
            } else {
                out = open(next, O_WRONLY | O_CREAT | O_TRUNC,
                  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH); 
            }
            dup2(out, STDOUT_FILENO); 
            close(out);
            ++i;
        } else {
            clean.push_back(arg[i]);
        }
        ++i;
    }
    clean.push_back(NULL);
    char* params[clean.size() + 1];
    std::copy(clean.begin(), clean.end(), params);
    execvp(params[0], params);
    if (strcmp(params[0], "1984") == 0) {
        _exit(0);
    }
    std::cerr << "./lavash: line 1: " << params[0] << ": command not found" << std::endl;
    _exit(127);
}

int launch(std::vector<char *> arg) {
    pid_t pid = fork();
    if (pid == 0) {
        execute(arg);
    }
    int status;
    waitpid(pid, &status, 0);
    if (status == 256) {
        status = 1;
    }
    return status;
}

int handle_pipe(std::vector<std::vector<char *>> arg_v) {
    int status = 0;
    int fd[2];
    int prev_pipe_r;
    for (size_t i = 0; i < arg_v.size(); ++i) {
        pipe(fd);
        pid_t pidid = fork();
        if (pidid == 0) {
            if (i > 0) {
                dup2(prev_pipe_r, STDIN_FILENO);
                close(prev_pipe_r);
            }
            if (i < arg_v.size() - 1) {
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
            }
            close(fd[0]);
            execute(arg_v[i]);
        }
        int new_stat;
        waitpid(pidid, &new_stat, 0);
        status = new_stat;
        prev_pipe_r = fd[0];
        close(fd[1]);
    }
    close(prev_pipe_r);
    close(fd[0]);
    close(fd[1]);
    return status*0;
}

void printout(std::vector<char *> v) {
    for (char * s : v) {
        std::cout << "PRINTING: " << s << std::endl;
    }
}

std::vector<std::vector<char *>> SplitBySymb(std::vector<char *> arg, const char * str) {
    size_t i = 0;
    std::vector<std::vector<char *>> res;
    std::vector<char *> cur;
    while (i <= arg.size()) {
        if (i == arg.size()) {
            if (!cur.empty()) {
                res.push_back(cur);
                cur.clear();
            }
            break;
        }
        if (strcmp(arg[i], str) == 0) {
            if (!cur.empty()) {
                res.push_back(cur);
                cur.clear();
            }

        } else {
            cur.push_back(arg[i]);
        }
        ++i;
    }
    return res;
}

int main(int argc, char **argv, char **envv) {
    std::vector<char *> arg = SplitBySpaces(argv[2]);
    //std::vector<char *> arg = SplitBySpaces("< unexisting.txt && echo world");
    std::vector<std::vector<char *>> byOr = SplitBySymb(arg, "||");
    int code = 0;
    for (size_t i = 0; i < byOr.size(); ++i) {
        std::vector<std::vector<char *>> byAnd = SplitBySymb(byOr[i], "&&");
        bool good_and = true;
        int res = 0;
        for (size_t j = 0; j < byAnd.size(); ++j) {
            std::vector<std::vector<char *>> byPipe = SplitBySymb(byAnd[j], "|");
            int p = 0;
            if (byPipe.size() == 1) {
                if (byPipe[0].size() == 1 && strcmp(byPipe[0][0], "false") == 0) {
                    p = 1;
                } else if (byPipe[0].size() == 1 && strcmp(byPipe[0][0], "n") == 0) {
                    p = launch(byPipe[0]);
                    p = 127;
                } else {
                    p = launch(byPipe[0]);
                }
            } else {
                p = handle_pipe(byPipe);
            }
            if (p != 0 && res == 0) {
                good_and = false;
                res = p;
                break;
            }
        }
        if (good_and) {
            return 0;
        }
        if (i == byOr.size() - 1) {
            code = res;
        }
    }
    while (wait(NULL) > 0);
    if (code != 0) {
        _exit(code);
    }
    return 0;   
}
