// Languages.cpp
#include "Languages.h"

using namespace tinyxml2;

Languages Languages::from_xml(tinyxml2::XMLElement& element) {
    Languages languages;

    // Iterate over each "Language" child element
    XMLElement* languageElement = element.FirstChildElement("Language");
    while (languageElement) {
        // Parse the language int and add it to the vector
        int language = std::stoi(languageElement->GetText());
        if (!languages.Language) {
            languages.Language = std::vector<int>{};
        }
        languages.Language->push_back(language);

        // Move to the next "Language" sibling element
        languageElement = languageElement->NextSiblingElement("Language");
    }

    return languages;
}
