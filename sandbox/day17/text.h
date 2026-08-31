#pragma once
#include <string>
#include <vector>

std::string formatter(const std::string& enemieName, const std::string& target, const int& damage);
std::vector<std::string> Split(const std::string& line, char delimeter);
void typewriter(const std::string text);
std::string WrapText(const std::string& text, size_t columnWidth);
