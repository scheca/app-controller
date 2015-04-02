/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Sergio Checa Blanco
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
*/

#include "shell_parser_tree.h"

#include <iostream>

#define LOAD_CLI_CMD(cmdStr, text, func)                                \
    do {                                                                \
        cli::CliCmd* cliCmd = new cli::CliCmd{text, func};              \
        if (!commands.loadCommand(cmdStr, cliCmd)) delete cliCmd;       \
    } while(0);

static bool gStop = false;

static std::vector<std::string> tokenize(const std::string& input);

static void help(const std::string& args);
static void exit(const std::string& args);

int main()
{
    cli::CliCmdTree commands;

    LOAD_CLI_CMD("help", "Help", &::help);
    LOAD_CLI_CMD("quit", "Exit", (void(*)(const std::string&))&::exit);
    LOAD_CLI_CMD("exit", "Exit", (void(*)(const std::string&))&::exit);

    std::string line;

    while (!gStop && !std::cin.eof()) {
        std::cout << "shellapp> ";
        std::getline(std::cin, line);

        if (!line.empty()) {
            auto tokens = tokenize(line);
            for (auto const& completion : commands.execute(tokens[0]))
                std::cout << tokens[0] << completion << std::endl;
        }
    }

    return 0;
}

std::vector<std::string> tokenize(const std::string& input)
{
    std::vector<std::string> tokens;
    size_t len = input.length();
    size_t pos = 0;
    size_t next = 0;

    do {
        next = input.find_first_of(' ', pos);
        if (next-pos > 0 && pos < len)
            tokens.push_back(input.substr(pos, next-pos));
        pos = next + 1;
    } while (next != std::string::npos);

    return tokens;
}

//====================================================================
//
// Definition of command handlers
//
//====================================================================

void help(const std::string& args)
{
    std::cout << "Help" << std::endl;
}

void exit(const std::string& args)
{
    std::cout << "Bye!" << std::endl;
    gStop = true;
}
