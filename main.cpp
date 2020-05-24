#include <iostream>
#include <string>
#include <filesystem>
#include "GrandTrie.h"
#include "ArtAnalyzer.h"

int main() {

    ArtAnalyzer pooch;
    GrandTrie tested;

    std::string path("contents/articles");
    std::string entryPath("");
    for (const auto & entry : std::filesystem::directory_iterator(path)) {
        if (entry.path() != path + "/.DS_Store") {
            entryPath = entry.path();
            pooch.parseDoc(entryPath);
        }
    }

    //pooch.printWords(std::cout);

    tested.rewritArt(pooch);

    std::string on("newOutName.txt");
    tested.serialize(on);

    GrandTrie deser;
    deser.deserialize(on);

    tested.printWords(std::cout);
    std::cout << "\n\n\n";
    deser.printWords(std::cout);

    return 0;
}
