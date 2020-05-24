// File name: ArtAnalyzer.h
// Author: Kyle Gendreau
// Email: kyle.j.gendreau@vanderbilt.edu
// Date: 5/22/20
//
// Description:


#ifndef WRITANALYZER_ARTANALYZER_H
#define WRITANALYZER_ARTANALYZER_H

#include <cstdlib>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <tuple>
#include <stdexcept>
#include <iostream>
#include <fstream>

const char CHARS_USED[] = {'$', '%', '-', '.', '0', '1', '2', '3', '4', '5', '6', '7',
                           '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
                           'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
                           'y', 'z'};

const size_t ARR_CHAR_SIZE = sizeof(CHARS_USED);

const std::string COMM_WORDS_PATH = "contents/commonWords.txt";

typedef std::map<std::string, int> wordMap;

class ArtAnalyzer {
public:

    // Trie node structure
    struct TNode {
        char val;
        typedef std::vector<std::tuple<std::string, size_t, bool>> mNodeVec;
        mNodeVec nodeFileInfo;
        TNode* nArr[ARR_CHAR_SIZE];

        //constructors
        TNode();
        TNode(const char c);
        TNode(const TNode* &og);
        //destructor
        ~TNode();
    };
    typedef TNode* TNodePtr;

    TNodePtr head; // root TNode
    int charIndex[256]; // allows for easy direct access to TNodes
    std::set<std::string> commonWords; // easy access to overused words
    wordMap fileCnt; // maps file names to total words counted in file

    //region RECURSIVES

    /**
     * recAddWord --
     * private method adds string to lex, ignores nodes if already added, manages size_t values, word
     * should already be validated
     * @param fname - file word is from
     * @param isTitle - true if title word, false otherwise
     * @param leaf - current node being modified
     * @param s - string being added
     */
    void recAddWord(const std::string &fname, const bool &isTitle, TNodePtr &leaf, const
    std::string &s);

    /**
     * printWords --
     * recursively prints all words to ostream
     * @param out - stream being printed to
     * @param s - string from all previously visited TNodes
     * @param leaf - current TNode
     */
    static void recPrintWords(std::ostream &out, const std::string &s, TNodePtr &leaf);

    /**
     * printWords --
     * recursively prints all words from the specific fname to ostream
     * @param out - stream being printed to
     * @param s - string from all previously visited TNodes
     * @param leaf - current TNode
     */
    static void recPrintWords(std::ostream &out, const std::string &fname, const std::string &s,
            TNodePtr &leaf);
    //endregion

    //region INITIALIZERS

    void initCharIndex();
    /**
     * initCommonWords --
     * initializes all the common words into map
     * @param fname - file name being processed
     */
    void initCommonWords(const std::string &fname);
    //endregion

    // CONSTRUCTORS
    ArtAnalyzer();

    /**
 * validateWord --
 * checks if word is in commonWords
 * @param s - string being checked
 * @return - true if not in commonWords
 */
    bool validateWord(std::string &s);

    /**
     * addWord --
     * calls on recAddWord, adds word to Trie, storing fname, its count, and it it's a title word
     * or not
     * @param fname - file name
     * @param isTitle - true if title word, false otherwise
     * @param word - string being added
     * @return - true if valid word and added, false otherwise
     */
    bool addWord(const std::string &fname, const bool &isTitle, std::string &word);

    //region HANDLE CHAR FUNCTS

    /**
     * validateChar --
     * makes lowercase and verifies that c is indexed in charIndex and is a valid ascii char
     * @param c - char being inspected
     * @return - true if within index, false otherwise
     */
    bool validateChar(char &c);

    /**
      * handleBrackets --
      * this function should be called from within @parseDoc(). checks if curChar == '[', if so,
      * iterates
      * through next chars until it reaches ']' and dumps them so as not to add to the word.  This
      * is mostly to deal with apa style references, but can be used as a makeshift ignore
      * commmand in writings.
      *
      * if curChar is begin-bracket and the char directly after is also a begin-bracket, function
      * will get rid of all chars until it finds an end-bracket or it reaches EOF,
      *
      * else it will get rid of all chars until end-bracket or it reaches a newline.
      *
      * @param inF - istream source
      * @param sH - string holder, the current word
      * @param curChar - the current character
      * @return - true if it was a bracket, false otherwise
      */
    bool handleBrackets(std::istream &inF, const std::string &sH, char &curChar);

    /**
     * handleHyphens --
     * this function should be called from within @parseDoc(). checks if curChar == '-', if so:
     *
     *      check if next char is '.', '$', or some int.  if so:
     *          check if sH is currently empty, if so return false
     *          check if previous char is '%' or an int, if so then treat it as hyphenated word,
     *              see last check for reference of hyphenated word
     *
     *      check if sH is currently empty, if so, drop '-' and return true
     *
     *      check if next char is '-'. if so, treat as delimiter: drop hyphen, get
     *          rid of all directly following '-', make curChar a ' ' and return false
     *
     *      check if next char is a letter, if so, add current word without hyphen, set hyphWord
     *          == true to explain that the comp should add full word once reaching a delimiter,
     *          and also add the word after the hyphen, ONLY INCREMENT WORDMAP BY ONE! and then
     *          finally return false
     *
     * @param inF - istream source
     * @param hyphWord - true if it's a properly hyphenated word
     * @param sH - string holder, the current word
     * @param curChar - the current character
     * @return - true if used function, false if no change
     */
    bool handleHyphens(std::istream &inF, bool &hyphWord, const std::string &sH, char &curChar);

    //todo look at this, idk why we should even have it honestly
    /**
     * handleApostrophes --
     * this function should be called from within @parseDoc(). checks if curChar == '\'', if so:
     *      checks if sH length is 0, if so, returns true
     *
     *      checks if next char is anything but a letter, returns true
     *
     *      checks for contractions
     *
     *      else always return true
     *
     * @param inF - istream source
     * @param sH - string holder, the current word
     * @param curChar - the current character
     * @return - true if used function, false if no change
     */
    bool handleApostrophes(std::istream &inF, const std::string &sH, char &curChar);

    /**
     * handleNumSymb --
     * this function should be called from within @parseDoc(). checks if curChar == '$' or '%',
     * is $ an int must be after.  if % an int must be before.
     * @param inF - istream source
     * @param numSymb - true if it's a properly numSymb word, ie % is after an int, and $ before
     * @param sH - string holder, the current word
     * @param curChar - the current character
     * @return - true if used function, false if no change
     */
    bool handleNumSymbol(std::istream &inF, bool &numSymb, const std::string &sH, char &curChar);

    /**
     * handlePeriods --
     * this function should be called from within @parseDoc(). checks if curChar == '.', if so:
     *
     *      checks if previous char is a ' ', '$', or a digit and the next char is a digit
     *
     * @param inF - istream source
     * @param sH - string holder, the current word
     * @param curChar - the current character
     * @return - true if used function, false if no change
     */
    bool handlePeriods(std::istream &inF, const std::string &sH, char &curChar);
    //endregion

    /**
 * contCharHandle --
 * just holds all charHandle functions to make it easier
 * @param inF - istream source
 * @param sH - string holder, the current word
 * @param curChar - the current character
 * @param hyphWord - true if it's a properly hyphenated word
 * @param numSymb - true if it's a properly numSymb word, ie % is after an int, and $ before
 * @return - true if any of the functions returned true
 */
    bool contCharHandle(std::istream &inF, const std::string &sH, char &curChar, bool &hyphWord,
            bool &numSymb);

    /**
     * modWords --
     * modify and addwords with regards to hyphWord and numSymb
     * @param fname - name of file being read from
     * @param sH - string currently made
     * @param hyphWord - true if it's a hyphenated word and needs to be dealt with
     * @param numSymb - true if there's a number symbol that needs to be dealt with
     * @param isTitle - true if title word, false otherwise
     */
    void modWords(const std::string &fname, const std::string &sH, bool &hyphWord, bool &numSymb,
            bool isTitle);

    /**
     * parseDoc --
     * goes through the entire document, adding all valid words to Trie
     * @param fname - file name
     */
    void parseDoc(const std::string &fname);

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
     * openFile --
     * opens input and output files
     * @param infile and outfile -- the streams being using
     * @param fileName -- name of file being opened
     */
    static void openFile(std::ifstream &infile, const std::string &fname);
    static void openFile(std::ofstream &outfile, const std::string &fname);

};


#endif //WRITANALYZER_ARTANALYZER_H
