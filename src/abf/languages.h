#pragma once

#include <vector>
#include <optional>
#include <iostream>
#include <string>
#include "tinyxml2.h"

class Languages {
public:
    std::optional<std::vector<int>> Language;

    static Languages from_xml(tinyxml2::XMLElement& element);
};
