// Collations.cpp
#include "collations.h"

using namespace tinyxml2;

Collations Collations::from_xml(tinyxml2::XMLElement& element) {
    Collations collations;

    // Iterate over each "Collation" child element
    XMLElement* collationElement = element.FirstChildElement("Collation");
    while (collationElement) {
        // Get the collation string and add it to the vector
        std::string collation = collationElement->GetText();
        if (!collations.Collation) {
            collations.Collation = std::vector<std::string>{};
        }
        collations.Collation->push_back(collation);

        // Move to the next "Collation" sibling element
        collationElement = collationElement->NextSiblingElement("Collation");
    }

    return collations;
}
