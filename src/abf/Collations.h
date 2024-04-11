// Collations.h
#ifndef COLLATIONS_H
#define COLLATIONS_H

#include <vector>
#include <optional>
#include <string>
#include "../../third_party/tinyxml2/tinyxml2.h"

class Collations {
public:
    std::optional<std::vector<std::string>> Collation;

    static Collations from_xml(tinyxml2::XMLElement& element);
};

#endif // COLLATIONS_H
