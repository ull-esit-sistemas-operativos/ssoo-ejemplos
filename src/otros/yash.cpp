// yash.cpp - Ejemplo de muy básico del funcionamiento de una shell
//
//      g++ -o yash yash.cpp
//

#include <cerrno>
#include <cstring>
#include <fstream>
#include <iostream>
#include <print>
#include <sstream>
#include <vector>

#include <glob.h>
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
        std::print( "{} ", *it);
    }
    std::print("\n");
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
            std::println(stderr, "No se pudo ejecutar el comando: {}", std::strerror(errno));
            exit(0);                                    // terminar el proceso para no tener dos shell
        }
    }
    else if (pid > 0) {
        waitpid(pid, nullptr, 0);
    }
    else {
        std::println(stderr, "No se pudo ejecutar el comando: {}", std::strerror(errno));
    }
}

void chdir(const std::vector<std::string>& words)
{
    chdir(words[1].c_str());
}

std::vector<std::string> expand_wildcards(const std::vector<std::string>& words)
{
    std::vector<std::string> expanded_words;
    for(auto& word: words) {
        glob_t pglob;
        glob(word.c_str(), GLOB_TILDE | GLOB_NOCHECK, nullptr, &pglob);
        if (pglob.gl_pathc) {
            for (char** p = pglob.gl_pathv; *p != nullptr; ++p) {
                expanded_words.push_back(*p);
            }
        }
        globfree(&pglob);
    }
    
    return expanded_words;
}

int main(int argc, char* argv[])
{
    bool interative_mode = argc > 1 ? false : true;
    
    std::ifstream ifs;
    if(! interative_mode) {                             // paso 7
        ifs.open(argv[1]);
        std::cin.rdbuf(ifs.rdbuf());
    }
    
    while(! std::cin.eof()) {
        
        if(interative_mode) {
            std::print( "$> " );
        }
        
        std::string line;
        std::getline(std::cin, line);
        
        if (line.empty() || line[0] == '#') {           // paso 4
            continue;
        }
            
        auto words = split(line);                       // paso 1
        words = expand_wildcards(words);                // paso 8

        size_t pos = words[0].find_first_of('=');         
        if (pos != std::string::npos) {                 // paso 6
          auto name = words[0].substr(0, pos);
          auto value = words[0].substr(pos + 1);
          // con putenv() nos ahorraríamos dividir words[0] pero al volver se queda
          // con un puntero a la cadena para usarlo internamente.
          //  => la cadena words[0] no se puede liberar
          //  => habría que reservar un buffer dinámicamente y copiar words[0] en él.
          setenv(name.c_str(), value.c_str(), 1/* overwrite */);
        }
        else if (words[0] == "exit") {                  // paso 2
            break;
        }
        else if (words[0] == "echo") {                  // paso 3
            echo(words);
        }
        else if (words[0] == "cd") {                    // paso 5
            chdir(words);
        }
        else {
            run(words);                                 // paso 4
        }
    }
    
    return EXIT_SUCCESS;
}
