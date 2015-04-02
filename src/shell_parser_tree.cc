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

#include <vector>
#include <stack>
#include <iostream>

#include "shell_parser_tree.h"

namespace cli
{

class CliCmdNode
{
private:
    CliCmdNode(char character);
    ~CliCmdNode();

    std::vector<CliCmdNode*> m_kids;
    char m_character;
    CliCmd* m_command;

    friend class CliCmdTree;
};

CliCmdNode::CliCmdNode(char character) :
    m_kids(std::vector<CliCmdNode*>()),
    m_character(character),
    m_command(nullptr)
{
}

CliCmdNode::~CliCmdNode()
{
    std::vector<CliCmdNode*>::iterator n, end = m_kids.end();
    for (n = m_kids.begin(); n != end; ++n) {
        delete *n;
    }
    if (m_command)
        delete m_command;
}


CliCmdTree::CliCmdTree() : m_root(nullptr)
{
}

CliCmdTree::~CliCmdTree()
{
    if (m_root)
        delete m_root;
}

bool CliCmdTree::loadCommand(const std::string& cmdStr, CliCmd* const cmd)
{
    if (!m_root) {
        m_root = new CliCmdNode(0);
    }
    char const* ptr = cmdStr.c_str();
    CliCmdNode *n = m_root;
    while (*ptr != 0) {
        std::vector<CliCmdNode*>::iterator m = n->m_kids.begin();
        std::vector<CliCmdNode*>::iterator end = n->m_kids.end();
        while (m != end && *ptr != (*m)->m_character)
            ++m;
        if (end == m) {
            CliCmdNode *newNode = new CliCmdNode(*ptr);
            n->m_kids.push_back(newNode);
            n = newNode;
        }
        else
            n = *m;
        ++ptr;
    } //while
    if (nullptr == n->m_command) {
        n->m_command = cmd;
        return true;
    }
    return false;
}

std::vector<std::string> CliCmdTree::execute(const std::string& cmdStr) const
{
    if (!m_root || cmdStr.length() == 0)
        return {};

    char const* ptr = cmdStr.c_str();
    CliCmdNode *n = m_root;
    while (*ptr != 0) {
        std::vector<CliCmdNode*>::iterator m = n->m_kids.begin();
        std::vector<CliCmdNode*>::iterator end = n->m_kids.end();
        while (m != end && *ptr != (*m)->m_character)
            ++m;
        if (end == m)
            return {};
        else {
            n = *m;
            ++ptr;
        }
    }
    if (n->m_command)
        n->m_command->m_exec("");
    else
        return autocomplete(n);

    return {};
}

std::vector<std::string> CliCmdTree::autocomplete(CliCmdNode *root) const
{
    std::vector<std::string> completions;
    std::string command = "";
    std::stack<CliCmdNode*> branch;
    for (auto const& m : root->m_kids)
        branch.push(m);
    while (!branch.empty()) {
        CliCmdNode* n = branch.top();
        branch.pop();
        command.push_back(n->m_character);
        if (n->m_command)
            completions.push_back(command);
        if (!n->m_kids.empty())
            for (auto const& m : n->m_kids)
                branch.push(m);
        else
            command.clear();
    }

    return completions;
}

} // namespace cli
