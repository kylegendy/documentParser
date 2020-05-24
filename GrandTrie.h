// File name: GrandTrie.h
// Author: Kyle Gendreau
// Email: kyle.j.gendreau@vanderbilt.edu
// Date: 5/20/20
//
// Description:


#ifndef WRITANALYZER_GRANDTRIE_H
#define WRITANALYZER_GRANDTRIE_H

#include "ArtAnalyzer.h"
#include <vector>

class GrandTrie {
//private: //todo redo this
public:

    //region Trie node structure
    struct GNode {
        std::string val; //string value
        // first elem is file int, second is word cnt, third is for isTitle
        std::vector< std::vector<size_t> > fileInfo;
        GNode* nArr[ARR_CHAR_SIZE];

        //constructors
        GNode();
        GNode(std::string s);
        GNode(const GNode* og);
        //destructor
        ~GNode();
    };
    typedef GNode* GNodePtr;

    //endregion

    GNodePtr head; // initial node

    std::vector< std::tuple<std::string, int> > fileDat; // file name, word count

    wordMap fnameToInt;

    int charIndex[256];

    //region INITIALIZERS

    void initCharIndex();
    //endregion

//public:

    GrandTrie();

    //region BIG-3

    GrandTrie(const GrandTrie &og);

    GrandTrie &operator=(const GrandTrie &og);

    ~GrandTrie();
    //endregion

    //region RECURSIVES

    void recRewritArt(GNodePtr &leaf, const ArtAnalyzer::TNodePtr &wlkr, std::string s);

    /**
 * recSerialize --
 * recursively outstream words and their data values in standard serialized format
 * @param nameRef - map for numbers referring to file names
 * @param leaf - current node
 * @param out - outstream
 */
    void recSerialize(GNodePtr &leaf, std::ostream &out);

    /**
     * printWords --
     * recursively prints all words to ostream
     * @param out - stream being printed to
     * @param s - string from all previously visited TNodes
     * @param leaf - current TNode
     */
    void recPrintWords(std::ostream &out, const std::string &s, GNodePtr &leaf);

    /**
     * printWords --
     * recursively prints all words from the specific fname to ostream
     * @param out - stream being printed to
     * @param s - string from all previously visited TNodes
     * @param leaf - current TNode
     */
    void recPrintWords(std::ostream &out, const std::string &fname, const std::string &s,
            GNodePtr &leaf);

    /**
     * recDeserialize --
     * recursively read through the standard form of a serialized document and apply values
     * @param leaf - current TNode
     * @param scnr - instream
     */
    void recDeserialize(GNodePtr &leaf, std::istream &scnr);
    //endregion

    /**
     * rewritArt --
     * Rewrites the saved data from ArtAnalyzer object into new radix trie.  Overwrites current
     * data in trie
     * @param og -- ArtAnalyzer
     */
    void rewritArt(ArtAnalyzer &og);

    /**
     * printWords --
     * prints all words to ostream
     * @param out - ostream being printed to
     */
    void printWords(std::ostream &out);

    /**
     * printWords --
     * prints all words in Trie from fname.txt to ostream
     * @param out - ostream being printed to
     * @param fname - file name
     */
    void printWords(std::ostream &out, const std::string &fname);

    /**
 * serialize --
 * serializes all data into standard format
 * @param out - outstream
 */
    void serialize(std::string outName);

    /**
 * delimitScnr --
 * takes in new chars until it comes across a comma or a parenthesis
 * @param scnr - in-stream
 * @return - the string being delimited including the last character, ie a ',' or a ')' or '('
 */
    static std::string delimitScnr(std::istream &scnr);

    /**
     * deserialize --
     * deserializes standardized data from fname.txt
     * @param fname - file name
     */
    void deserialize(const std::string &fname);

};


#endif //WRITANALYZER_GRANDTRIE_H
