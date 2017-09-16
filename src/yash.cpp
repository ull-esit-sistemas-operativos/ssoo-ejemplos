// yash.cpp - Ejemplo de muy básico del funcionamiento de una shell

#include <cerrno>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

std::vector<std::string> split(const std::string& s)
{
    std::stringstream ss(s);
    std::vector<std::string> words;
    while(! ss.eof()) {
        std::string word;
        ss >> word;
        words.push_back(word);
    }
    
    return words;
}

void echo(const std::vector<std::string>& words)
{
    for (auto it = words.begin() + 1; it != words.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\n";
}

void run(const std::vector<std::string>& words)
{
    pid_t pid = fork();
    
    if (pid == 0) {
        size_t i = 0;
        char** argv = new char*[words.size() + 1];      // hueco extra para el NULL
        for(auto& word: words) {
            argv[i] = new char[word.size() + 1];        // hueco extra para el NULL
            word.copy(argv[i], word.size());
            argv[i][word.size()] = '\0';                // sting::copy() no añade el NULL
            ++i;
        }
        argv[i] = nullptr;                              // NULL al final del array de argumentos
        
        int result = execvp(words[0].c_str(), argv);
        if (result < 0) {
            std::cerr << "No se pudo ejecutar el comando: " << std::strerror(errno) << "\n";
            exit(0);                                    // terminar el proceso para no tener dos shell
        }
    }
    else if (pid > 0) {
        waitpid(pid, NULL, 0);
    }
    else {
        std::cerr << "No se pudo ejecutar el comando: " << std::strerror(errno) << "\n";
    }
}

void chdir(const std::vector<std::string>& words)
{
    chdir(words[1].c_str());
}

int main()
{
    while(! std::cin.eof()) {
        
        std::cout << "$> ";
        
        std::string line;
        std::getline(std::cin, line);
        
        auto words = split(line);
        
        if (words[0] == "exit") {
            break;
        }
        else if (words[0] == "echo") {
            echo(words);
        }
        else if (words[0] == "cd") {
            chdir(words);
        }
        else {
            run(words);
        }
    }
    
    return 0;
}
