#include <iostream>
#include <fstream>
#include <PGE/String/String.h>

#include "INI.h"

INIFile::INIFile(const PGE::FilePath& filename) {
    name = filename;

    unsavedChanges = false;

    Section* currSection = nullptr;

    std::ifstream file(name.str().cstr());

    if (file.is_open()) {
        PGE::String currLine;
        while (!file.eof()) {
            std::string cppStr;
            getline(file, cppStr);
            currLine = PGE::String(cppStr);

            PGE::String::Iterator ch = currLine.charAt(0);
            if (ch != currLine.end()) {
                if (*ch == L'[') {
                    if (currSection != nullptr) {
                        sections.push_back(currSection);
                    }
                    currSection = new Section();
                    currLine = currLine.substr(1, currLine.length() - 2).trim();
                    currSection->names = currLine.split('|', true);
                } else if (*ch != L';') {
                    if (currSection != nullptr) {
                        std::vector<PGE::String> split = currLine.split('=', false);
                        if (split.size() >= 2) {
                            currSection->keys.push_back(split[0].trim());

                            // If the value itself had a '=' in it then recover those.
                            split.erase(split.begin());
                            PGE::String value = PGE::String::join(split, '=');
                            currSection->values.push_back(value.trim());
                        }
                    }
                }
            }
        }
        if (currSection != nullptr) {
            sections.push_back(currSection);
        }
    }
    file.close();
}

INIFile::~INIFile() {
    save();
    for (int i = 0; i < (int)sections.size(); i++) {
        delete sections[i];
    }
}

PGE::String INIFile::getValue(const PGE::String& section, const PGE::String& key, const PGE::String& defaultValue) {
    for (int i = 0; i < (int)sections.size(); i++) {
        for (int j = 0; j < (int)sections[i]->names.size(); j++) {
            if (section.equalsIgnoreCase(sections[i]->names[j])) {
                for (int k = 0; k < (int)sections[i]->keys.size(); k++) {
                    if (sections[i]->keys[k].equalsIgnoreCase(key)) {
                        return sections[i]->values[k];
                    }
                }
                return defaultValue;
            }
        }
    }
    return defaultValue;
}

void INIFile::setValue(const PGE::String& section, const PGE::String& key, const PGE::String& value) {
    for (int i = 0; i < (int)sections.size(); i++) {
        for (int j = 0; j < (int)sections[i]->names.size(); j++) {
            if (section.equalsIgnoreCase(sections[i]->names[j])) {
                for (int k = 0; k < (int)sections[i]->keys.size(); k++) {
                    if (sections[i]->keys[k].equalsIgnoreCase(key)) {
                        if (!sections[i]->values[k].equals(value)) {
                            sections[i]->values[k] = value;
                            unsavedChanges = true;
                        }
                        return;
                    }
                }

                // Key doesn't exist, make it.
                sections[i]->keys.push_back(key.toLower());
                sections[i]->values.push_back(value);
                unsavedChanges = true;
                return;
            }
        }
    }

    // Section doesn't exist, make one.
    Section* sec = new Section();
    sec->names.push_back(section);
    sec->keys.push_back(key.toLower());
    sec->values.push_back(value);

    sections.push_back(sec);
    unsavedChanges = true;
}

std::vector<INIFile::Section*> INIFile::getAllSections() {
    return sections;
}

void INIFile::save() {
    if (!unsavedChanges) {
        return;
    }

    std::ofstream f(name.str().cstr());

    for (int i = 0; i < (int)sections.size(); i++) {
        PGE::String secName = sections[i]->names[0];
        for (int j = 1; j < (int)sections[i]->names.size(); j++) {
            secName = secName + "|" + sections[i]->names[j];
        }
        f << "[" << secName << "]" << std::endl;
        for (int j = 0; j < (int)sections[i]->keys.size(); j++) {
            f << sections[i]->keys[j] << " = " << sections[i]->values[j] << std::endl;
        }
        f << std::endl;
    }

    f.close();
    unsavedChanges = false;
}

PGE::String INIFile::getString(const PGE::String& section, const PGE::String& key, const PGE::String& defaultValue) {
    return getValue(section, key, defaultValue);
}

bool INIFile::getBool(const PGE::String& section, const PGE::String& key, bool defaultValue) {
    PGE::String val = getValue(section, key, "");
    if (val == "true" || val == "false") {
        return val == "true";
    } else {
        return defaultValue;
    }
}

int INIFile::getInt(const PGE::String& section, const PGE::String& key, int defaultValue) {
    return getValue(section, key, PGE::String::from(defaultValue)).to<int>();
}

float INIFile::getFloat(const PGE::String& section, const PGE::String& key, float defaultValue) {
    return getValue(section, key, PGE::String::from(defaultValue)).to<float>();
}

void INIFile::setString(const PGE::String& section, const PGE::String& key, const PGE::String& value) {
    setValue(section, key, value);
}

void INIFile::setBool(const PGE::String& section, const PGE::String& key, bool value) {
    setValue(section, key, value ? "true" : "false");
}

void INIFile::setInt(const PGE::String& section, const PGE::String& key, int value) {
    setValue(section, key, PGE::String::from(value));
}

void INIFile::setFloat(const PGE::String& section, const PGE::String& key, float value) {
    setValue(section, key, PGE::String::from(value));
}
