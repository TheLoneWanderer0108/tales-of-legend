#pragma once
#include "types.h"
#include <unordered_map>

std::unordered_map<int, SkillDef> loadedSkill(const std::string& path);
ElementType StringToElement(std::string& element);
Target StringToTarget(std::string& target);
