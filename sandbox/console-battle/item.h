#pragma once
#include "types.h"
#include <unordered_map>
#include <string>
ItemEffect StringToItemEffect(std::string& effect);
std::unordered_map<std::string, Item> LoadItems(const std::string& path);
