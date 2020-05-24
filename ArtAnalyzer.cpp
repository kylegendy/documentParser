// File name: ArtAnalyzer.cpp
// Author: Kyle Gendreau
// Email: kyle.j.gendreau@vanderbilt.edu
// Date: 5/22/20
//
// Description:


#include "ArtAnalyzer.h"

//region TNode

ArtAnalyzer::TNode::TNode() : val('?') {
    for (size_t i(0); i<ARR_CHAR_SIZE; ++i) { nArr[i] = nullptr; }
}

ArtAnalyzer::TNode::TNode(const char c) : val(c) {
    for (size_t i(0); i<ARR_CHAR_SIZE; ++i) { nArr[i] = nullptr; }
}

ArtAnalyzer::TNode::TNode(const ArtAnalyzer::TNode* &og) : val(og->val) {
    mNodeVec thing(og->nodeFileInfo);
    std::swap(this->nodeFileInfo, thing);

    for (size_t i(0); i<ARR_CHAR_SIZE; ++i) {
        if (og->nArr[i] == nullptr) { nArr[i] = nullptr; }
        else { nArr[i] = new TNode(*(og->nArr[i])); }
    }
}

ArtAnalyzer::TNode::~TNode() {
    for (size_t i(0); i<ARR_CHAR_SIZE; ++i) {
        delete nArr[i];
        nArr[i] = nullptr;
    }
}
//endregion

/**
 * openFile --
 * opens input and output files
 * @param infile and outfile -- the streams being using
 * @param fileName -- name of file being opened
 */
void ArtAnalyzer::openFile(std::ifstream &infile, const std::string &fname) {
    infile.open(fname.c_str());
    if (infile.fail()) {
        std::cout << "\n\nError opening input data file : " << fname << std::endl;
        exit(1);
    }
}
void ArtAnalyzer::openFile(std::ofstream &outfile, const std::string &fname) {
    outfile.open(fname.c_str());
    if (outfile.fail()) {
        std::cout << "\n\nError opening input data file : " << fname << std::endl;
        exit(1);
    }
}

//initializes values in charIndex
void ArtAnalyzer::initCharIndex() {
    size_t validCharCnt(0);

    // iterate through every elem in charIndex
    for (size_t i(0); i< (sizeof(charIndex)/sizeof(*charIndex)); ++i) {
        if (i == CHARS_USED[validCharCnt]) { charIndex[i] = validCharCnt; ++validCharCnt; }
        else { charIndex[i] = -1; }
    }
}

/**
 * initCommonWords --
 * initializes all the common words into map
 * @param fname - file name being processed
 */
void ArtAnalyzer::initCommonWords(const std::string &fname) {
    std::ifstream inF;
    openFile(inF, fname);

    std::string word("");
    while (inF >> word) {
        commonWords.insert(word);
    }
    inF.close();
}

//defailt constructor
ArtAnalyzer::ArtAnalyzer() : head(new TNode) {
    initCharIndex();
    initCommonWords(COMM_WORDS_PATH);
}

//region RECURSIVE FUNCTS

void ArtAnalyzer::recAddWord(const std::string &fname, const bool &isTitle,
        ArtAnalyzer::TNodePtr &leaf, const std::string &s) {

    if (s == "acts") {

    }

    if (s.length() == 0) { return; } // edge case, probs shouldn't happen

    size_t arrElem(charIndex[s.at(0)]); // find elem in array that is being modified

    // validate proper char input
    if (arrElem < 0 || arrElem >= ARR_CHAR_SIZE) {
        throw std::invalid_argument(&"<invalid char> recAddWord() : arrElem = " [ arrElem]);
    }

    // assign Trie node value
    if (leaf->nArr[arrElem] == nullptr) {
        leaf->nArr[arrElem] = new TNode;
        leaf->nArr[arrElem]->val = s.at(0);
    } else if (leaf->nArr[arrElem]->val != s.at(0)) { leaf->nArr[arrElem]->val = s.at(0); }

    TNodePtr ref = leaf->nArr[arrElem];
    // if at end of string
    if (s.length() == 1) {

        // check if filename has already been added
        bool fnd(false);
        size_t g, z(ref->nodeFileInfo.size());
        for(g = 0; g < ref->nodeFileInfo.size() && !fnd; ++g) {
            fnd = std::get<0>((ref->nodeFileInfo).at(g)) == fname;
            if (fnd) {
                std::get<1>((ref->nodeFileInfo).at(g)) += 1;
            }
        }

        if (!fnd) { ref->nodeFileInfo.push_back(std::make_tuple(fname, 1, isTitle)); }

    } else { recAddWord(fname, isTitle, ref, s.substr(1)); }
}

void ArtAnalyzer::recPrintWords(std::ostream &out, const std::string &s,
        ArtAnalyzer::TNodePtr &leaf) {

    if (leaf == nullptr) { return; } // base case

    // if there's file data at this node, print all info
    size_t numberOfFiles(leaf->nodeFileInfo.size());
    if (numberOfFiles != 0) {
        out << s << leaf->val << " ";

        for (size_t i(0); i<numberOfFiles; ++i) {
            out << "[" << std::get<0>(leaf->nodeFileInfo.at(i)) << ", "
                    << std::get<1>(leaf->nodeFileInfo.at(i)) << ", "
                << std::get<2>(leaf->nodeFileInfo.at(i)) << "] ";
        }
        out << std::endl;
    }

    for (size_t i(0); i<ARR_CHAR_SIZE; ++i) {
        recPrintWords(out, s + leaf->val, leaf->nArr[i]);
    }
}

void ArtAnalyzer::recPrintWords(std::ostream &out, const std::string &fname, const std::string &s,
                                  ArtAnalyzer::TNodePtr &leaf) {

    if (leaf == nullptr) { return; } // base case

    // if there's LData at this node, print all info
    size_t numberOfFiles(leaf->nodeFileInfo.size());
    if (numberOfFiles != 0) {

        for (size_t i(0); i<numberOfFiles; ++i) {
            if (std::get<0>(leaf->nodeFileInfo.at(i)) == fname) {
                out << s << leaf->val << " ";
                out << "[" << std::get<0>(leaf->nodeFileInfo.at(i)) << ", "
                    << std::get<1>(leaf->nodeFileInfo.at(i)) << ", "
                    << std::get<2>(leaf->nodeFileInfo.at(i)) << "] "
                    << std::endl;
            }
        }

    }

    for (size_t i(0); i<ARR_CHAR_SIZE; ++i) {
        recPrintWords(out, fname, s + leaf->val, leaf->nArr[i]);
    }
}
//endregion

/**
 * validateWord --
 * checks if word is in commonWords
 * @param s - string being checked
 * @return - true if not in commonWords
 */
bool ArtAnalyzer::validateWord(std::string &s) {
    std::string nS("");

    // make SURE that the word is what you want
    for (size_t i(0); i<s.length(); ++i) {
        if (s.at(i) != '\n' && s.at(i) != '\r' && s.at(i) != '\t' && s.at(i) != ' ') {
            nS += s.at(i);
        }
    }

    std::swap(s,nS);
    return (commonWords.find(s) == commonWords.end());
}

/**
 * addWord --
 * calls on recAddWord, adds word to Trie, storing fname, its count, and it it's a title word
 * or not
 * @param fname - file name
 * @param isTitle - true if title word, false otherwise
 * @param word - string being added
 * @return - true if valid word and added, false otherwise
 */
bool ArtAnalyzer::addWord(const std::string &fname, const bool &isTitle, std::string &word) {
    if (validateWord(word)) {
        recAddWord(fname, isTitle, head, word);
        return true;
    }

    return false;
}

/**
 * printWords --
 * prints all words to ostream
 * @param out - ostream being printed to
 */
void ArtAnalyzer::printWords(std::ostream &out) {
    std::cout << "number of files: " << fileCnt.size() << std::endl;
    for (wordMap::iterator iter = fileCnt.begin(); iter != fileCnt.end(); ++iter) {
        out << iter->first << " : " << fileCnt[iter->first] << " valid words" << std::endl;
    }

    for (size_t i(0); i<ARR_CHAR_SIZE; ++i) {
        recPrintWords(out, "", head->nArr[i]);
    }
    out << std::endl;
}

/**
 * printWords --
 * prints all words in Trie from fname.txt to ostream
 * @param out - ostream being printed to
 * @param fname - file name
 */
void ArtAnalyzer::printWords(std::ostream &out, const std::string &fname) {

    out << fname << " : " << fileCnt[fname] << " valid words" << std::endl;

    for (size_t i(0); i<ARR_CHAR_SIZE; ++i) {
        recPrintWords(out, fname, "", head->nArr[i]);
    }
    out << std::endl;
}

//region CHAR HANDLE FUNCTS

/**
 * validateChar --
 * makes lowercase and verifies that c is indexed in charIndex and is a valid ascii char
 * @param c - char being inspected
 * @return - true if within index, false otherwise
 */
bool ArtAnalyzer::validateChar(char &c) {
    c = tolower(c);

    // first part checks if it's out of bounds for normal ascii characters, second part checks if
    // it's indexed in our charIndex array, ie if we consider it a valid ascii char
    if (static_cast<unsigned char>(c) > 127)  { return false; }

    return (charIndex[c] >= 0 && charIndex[c] < ARR_CHAR_SIZE);
}

/**
  * handleBrackets --
  * this function should be called from @parseDoc(). checks if curChar == '[', if so, iterates
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
bool ArtAnalyzer::handleBrackets(std::istream &inF, const std::string &sH, char &curChar) {
    if (curChar != '[') { return false; }

    // double bracket will continue to get rid of all chars until ']' or EOF, just a neat feature
    if (inF.peek() == '[') {

        while (curChar != ']' && curChar != EOF) {
            curChar = inF.get();
        }

    } else { // get rid of all chars until ']' or newline

        while (curChar != ']' && (curChar != '\n' || curChar != '\r')) {
            curChar = inF.get();
        }

    }

    return true;
}

/**
 * handleHyphens --
 * this function should be called from @parseDoc(). checks if curChar == '-', if not, return
 * false, otherwise:
 *
 *      check if next char is '-'. if so, treat as delimiter: drop hyphen, get
 *          rid of all directly following '-', make curChar a ' ' and return false
 *
 *      check if next char is '.', '$', some int, or some letter.  if so:
 *          check if sH is currently empty, if so:
 *              if next char is a letter return true, else false
 *
 *      check if sH is currently empty, if so, drop '-' and return true
 *
 * @param inF - istream source
 * @param sH - string holder, the current word
 * @param curChar - the current character
 * @return - true if used function, false if no change
 */
bool ArtAnalyzer::handleHyphens(std::istream &inF, bool &hyphWord, const std::string &sH,
        char &curChar) {
    if (curChar != '-') { return false; }

    else if (inF.peek() == '-') {
        while (inF.peek() == '-') { inF.get(); }
        curChar = ' ';
        return false;
    }

    else if (inF.peek() == '.' || inF.peek() == '$' || isdigit(inF.peek()) || isalpha(inF.peek())) {

        if (sH.length() == 0) {
            if (isalpha(inF.peek())) { return true; }
            return false;
        }

        else { // mark as hyphenated word
            hyphWord = true;
            return false;
        }
    }

    else if (sH.length() == 0) { return true; }

    return false;
}

/**
 * handleApostrophes --
 * this function should be called from within @parseDoc(). checks if curChar is '\'', is so:
 *
 *      psych, forget all below, just always return true
 *      checks if sH length is 0, if so, returns true
 *
 *      checks if next char is anything but a letter, returns true
 *
 *      checks for contractions // psych, we aren't dealing with that since most contracted words
 *      are in commonWords.txt anyways
 *
 *      else always return true
 *
 * @param inF - istream source
 * @param sH - string holder, the current word
 * @param curChar - the current character
 * @return - true if used function, false if no change
 */
bool ArtAnalyzer::handleApostrophes(std::istream &inF, const std::string &sH, char &curChar) {
    if (curChar != '\'') { return false; }
    else { return true; }
}

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
bool ArtAnalyzer::handleNumSymbol(std::istream &inF, bool &numSymb, const std::string &sH,
        char &curChar) {
    if (curChar != '%' && curChar != '$') { return false; }

    if (curChar == '%' && sH.length() != 0 && isdigit(sH.at(sH.length()-1))) {
        numSymb = true;
        return false;
    }

    if (curChar == '$' && (isdigit(inF.peek()) || inF.peek() == '.')) {
        numSymb = true;
        return false;
    }

    return true;
}

/**
 * handlePeriods --
 * this function should be called from within @parseDoc(). checks if curChar == '.', if so:
 *
 *      checks if previous char is a ' ', '$', '-' or a digit and the next char is a digit
 *
 * @param inF - istream source
 * @param sH - string holder, the current word
 * @param curChar - the current character
 * @return - true if used function, false if no change
 */
bool ArtAnalyzer::handlePeriods(std::istream &inF, const std::string &sH, char &curChar) {
    if (curChar != '.') { return false; }

    else if (sH.length() != 0) {
        char cc = sH.at(sH.length()-1);
        if (cc == ' ' || cc == '$' || cc == '-') { return false; }
    }

    else if (isdigit(inF.peek())) { return false; }

    return true;
}
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
bool ArtAnalyzer::contCharHandle(std::istream &inF, const std::string &sH, char &curChar,
        bool &hyphWord, bool &numSymb) {
    if (curChar == ' ') { return false; }
    bool pH(false);

    // handle brackets
    if (!pH) { pH = handleBrackets(inF, sH, curChar); }

    // handle hyphens
    if (!pH) { pH = handleHyphens(inF, hyphWord, sH, curChar); }

    // handle apostrophes
    if (!pH) { pH = handleApostrophes(inF,sH,curChar); }

    // handle $ and %
    if (!pH) { pH = handleNumSymbol(inF, numSymb, sH, curChar); }

    // handle periods
    if (!pH) { pH = handlePeriods(inF, sH, curChar); }

    // validate char
    return pH;
}

/**
 * modWords --
 * modify and addwords with regards to hyphWord and numSymb
 * @param fname - name of file being read from
 * @param sH - string currently made
 * @param hyphWord - true if it's a hyphenated word and needs to be dealt with
 * @param numSymb - true if there's a number symbol that needs to be dealt with
 * @param isTitle - true if title word, false otherwise
 */
void ArtAnalyzer::modWords(const std::string &fname, const std::string &sH, bool &hyphWord,
        bool &numSymb, bool isTitle) {

    std::string sub("");
    std::string subT("");
    char pH(' ');

    // start with hyphenated words
    if (hyphWord) {

        for (size_t i(0); i<=sH.length(); ++i) {

            if (i == sH.length() || sH.at(i) == '-') {

                if (numSymb) {
                    addWord(fname, isTitle, sub);
                    for (size_t j(0); j <= sub.length(); ++j) {

                        if (j < sub.length() && sub.at(j) != '%' && sub.at(j) != '$') {
                            subT += sub.at(j);
                        }
                        else if (subT.length() != 0) {
                            addWord(fname, isTitle, subT);
                            subT = "";
                            sub = "";
                        }

                    }
                }
                else {
                    addWord(fname, isTitle, sub);
                    sub = "";
                }

            }
            else { sub += sH.at(i); }
        }

    }

        // then check for JUST number symbol words
    else if (numSymb) {
        for (size_t i(0); i<=sH.length(); ++i) {

            if (i < sH.length() && sH.at(i) != '%' && sH.at(i) != '$') {
                sub += sH.at(i);
            }
            else if (sub.length() != 0) {
                addWord(fname, isTitle, sub);
                sub = "";
            }
        }
    }

    hyphWord = false;
    numSymb = false;
}

/**
 * parseDoc --
 * goes through the entire document, adding all valid words to Trie
 * @param fname - file name
 */
void ArtAnalyzer::parseDoc(const std::string &fname) {
    std::ifstream inF;
    openFile(inF, fname);
    char curChar(' ');
    std::string sH("");
    bool hyphWord(false);
    bool numSymb(false);
    bool pH(false);

    // handle title - guess this means you can't have a long title :)
    while (curChar != '\n' && curChar != '\r') {
        curChar = inF.get();

        pH = contCharHandle(inF,sH,curChar,hyphWord,numSymb);

        if (!pH && (curChar == ' ' || curChar == '\r' || curChar == '\n') && sH.length() != 0) {
            modWords(fname, sH, hyphWord, numSymb, true);
            if (addWord(fname, true, sH)) {
                fileCnt[fname];
                fileCnt[fname] = fileCnt[fname] + 1;
            }
            sH = "";
        }
        else if (!pH && validateChar(curChar)) {
            sH += curChar;
        }
    }

    // reset vars
    hyphWord = false;
    numSymb = false;
    pH = false;

    // for the rest of the text
    while (curChar != EOF && sH != "bibliography") {
        curChar = inF.get();

        pH = contCharHandle(inF,sH,curChar,hyphWord,numSymb);

        if (!pH && (curChar == ' ' || curChar == '\r' || curChar == '\n' || curChar == EOF) &&
        sH.length() != 0) {

            modWords(fname, sH, hyphWord, numSymb, false);
            if (addWord(fname, false, sH)) {
                fileCnt[fname];
                fileCnt[fname] = fileCnt[fname] + 1;
            }

            sH = "";
        }
        else if (!pH && validateChar(curChar)) {
            sH += curChar;
        }
    }

    inF.close();
}

