// File name: GrandTrie.cpp
// Author: Kyle Gendreau
// Email: kyle.j.gendreau@vanderbilt.edu
// Date: 5/20/20
//
// Description:


#include "GrandTrie.h"
#include <map>
#include <cstdlib>

//region GNODE FUNCTS

GrandTrie::GNode::GNode() : val("?") {
    for (size_t i(0); i<ARR_CHAR_SIZE; ++i) { nArr[i] = nullptr; }
}

GrandTrie::GNode::GNode(std::string s) : val(s) {
    for (size_t i(0); i<ARR_CHAR_SIZE; ++i) { nArr[i] = nullptr; }
}

GrandTrie::GNode::GNode(const GrandTrie::GNode *og) : val(og->val) {

    //make a copy and swap vectors //todo will this work?
    std::vector<std::vector<size_t>> thing(og->fileInfo);
    std::swap(this->fileInfo, thing);

    // copy children
    for (size_t i(0); i<ARR_CHAR_SIZE; ++i) {
        if (og->nArr[i] == nullptr) { nArr[i] = nullptr; }
        else { nArr[i] = new GNode(og->nArr[i]); }
    }
}

GrandTrie::GNode::~GNode() {
    // recursive on children
    for (size_t i(0); i<ARR_CHAR_SIZE; ++i) {
        delete nArr[i];
        nArr[i] = nullptr;
    }
}

//endregion

void GrandTrie::initCharIndex() {
    size_t validCharCnt(0);

    // iterate through every elem in charIndex
    for (size_t i(0); i< (sizeof(charIndex)/sizeof(*charIndex)); ++i) {
        if (i == CHARS_USED[validCharCnt]) { charIndex[i] = validCharCnt; ++validCharCnt; }
        else { charIndex[i] = -1; }
    }
}

/**
 * deserialize --
 * deserializes standardized data from fname.txt
 * @param fname - file name
 */
std::string GrandTrie::delimitScnr(std::istream &scnr) {
    std::string s("");
    char c(scnr.get());
    while (c != ',') {
        s += c;
        c = scnr.get();
    }

    return s;
}

GrandTrie::GrandTrie() : head(new GNode) {
    initCharIndex();
}

//region BIG-3

GrandTrie::GrandTrie(const GrandTrie &og) : head(og.head) {
    initCharIndex();

    //todo this could be weird
    std::vector<std::tuple<std::string, int>> thing(og.fileDat);
    std::swap(thing, this->fileDat);
}

GrandTrie &GrandTrie::operator=(const GrandTrie &og) {
    if (this == &og) { return *this; }

    GrandTrie cpy(og);
    std::swap(this->head, cpy.head);
    std::swap(this->fileDat, cpy.fileDat);

    return *this;
}

GrandTrie::~GrandTrie() {
    delete head; //destructor should be recursive for all children
    head = nullptr;
}
//endregion

//region RECURSIVE

/**
 * recRewritArt --
 * recursively goes through TNode Trie and transitions its values into a radix version, placing
 * those values into GNode Trie.
 * @param leaf - GNode
 * @param wlkr - TNode
 * @param s - the string that will be added to GNode
 */
void GrandTrie::recRewritArt(GrandTrie::GNodePtr &leaf, const ArtAnalyzer::TNodePtr &wlkr,
                             std::string s) {

    if (s == "1" && wlkr->val == '1') {

    }

    // add the value
    s += wlkr->val;

    //count children
    size_t childCnt(0);
    for (size_t i(0); i<ARR_CHAR_SIZE; ++i) { if(wlkr->nArr[i] != nullptr) { ++childCnt; }}

    // if there is an empty file info
    if (wlkr->nodeFileInfo.size() == 0) {

        if (childCnt <= 1) {
            //add the one node's value (if there is a node)
            for (size_t i(0); i<ARR_CHAR_SIZE; ++i) {
                if(wlkr->nArr[i] != nullptr) {
                    recRewritArt(leaf, wlkr->nArr[i], s);
                }
            }
        } else {
            // assign this leaf value and move on
            leaf->val = s;
            s = "";

            // recurse on all children
            for (size_t i(0); i<ARR_CHAR_SIZE; ++i) {
                if(wlkr->nArr[i] != nullptr) {
                    leaf->nArr[i] = new GNode;
                    recRewritArt(leaf->nArr[i], wlkr->nArr[i], s);
                } else { leaf->nArr[i] = nullptr; }
            }
        }

    }
    else {

        // add all file info
        leaf->val = s;
        size_t fileNum;
        size_t fileWCnt;
        size_t fileIsTitle;
        for (size_t i(0); i<wlkr->nodeFileInfo.size(); ++i) {
            // get the file int
            fileNum = fnameToInt[std::get<0>(wlkr->nodeFileInfo.at(i))];
            // get the file word count
            fileWCnt = std::get<1>(wlkr->nodeFileInfo.at(i));
            // get if title word, 1 if true, 0 if false
            fileIsTitle = std::get<2>(wlkr->nodeFileInfo.at(i));

            // push value
            leaf->fileInfo.push_back({fileNum, fileWCnt, fileIsTitle});
        }

        // reset string and recurse on all children, if any. else makes them nullptr.
        s = "";
        for (size_t i(0); i<ARR_CHAR_SIZE; ++i) {
            if(wlkr->nArr[i] != nullptr) {
                leaf->nArr[i] = new GNode;
                recRewritArt(leaf->nArr[i], wlkr->nArr[i], s);
            } else { leaf->nArr[i] = nullptr; }
        }

    }
}

/**
 * printWords --
 * recursively prints all words to ostream
 * @param out - stream being printed to
 * @param s - string from all previously visited TNodes
 * @param leaf - current TNode
 */
void GrandTrie::recPrintWords(std::ostream &out, const std::string &s, GrandTrie::GNodePtr &leaf) {
    if (leaf == nullptr) { return; } // base case

    // if there's file data at this node, print all info
    size_t numberOfFiles(leaf->fileInfo.size());
    if (numberOfFiles != 0) {
        out << s << leaf->val << " ";

        for (size_t i(0); i<numberOfFiles; ++i) {
            out << "[" << leaf->fileInfo[i][0] << ", " << leaf->fileInfo[i][1] << ", "
                << leaf->fileInfo[i][2] << "] ";
        }
        out << std::endl;
    }

    for (size_t i(0); i<ARR_CHAR_SIZE; ++i) {
        recPrintWords(out, s + leaf->val, leaf->nArr[i]);
    }
}

/**
 * printWords --
 * recursively prints all words from the specific fname to ostream
 * @param out - stream being printed to
 * @param s - string from all previously visited TNodes
 * @param leaf - current TNode
 */
void GrandTrie::recPrintWords(std::ostream &out, const std::string &fname, const std::string &s,
                              GrandTrie::GNodePtr &leaf) {
    if (leaf == nullptr) { return; } // base case

    // if there's file data, print all info
    size_t numberOfFiles(leaf->fileInfo.size());
    if (numberOfFiles != 0) {

        size_t fInt(0);
        wordMap::iterator it;
        for (size_t i(0); i<numberOfFiles; ++i) {

            fInt = leaf->fileInfo[i][0];
            it = fnameToInt.begin();

            for (size_t j(0); j<=fInt; ++j) { ++it; }

            if (it->first == fname) {
                out << s << leaf->val << " ";
                out << "[" << leaf->fileInfo[i][0] << ", "
                    << leaf->fileInfo[i][1] << ", "
                    << leaf->fileInfo[i][2] << "] "
                    << std::endl;
            }
        }

    }

    for (size_t i(0); i<ARR_CHAR_SIZE; ++i) {
        recPrintWords(out, fname, s + leaf->val, leaf->nArr[i]);
    }
}

/**
* recSerialize --
* recursively outstream words and their data values in standard serialized format
* @param nameRef - map for numbers referring to file names
* @param leaf - current node
* @param out - outstream
*/
void GrandTrie::recSerialize(GNodePtr &leaf, std::ostream &out) {
    if (leaf == nullptr) { return; } //edge case, should never happen
    out << leaf->val << "," << std::flush;


    // output number files and their data
    out << leaf->fileInfo.size() << "," << std::flush;
    for (size_t i(0); i<leaf->fileInfo.size(); ++i) {
        out << std::to_string(leaf->fileInfo[i][0]) << "," << std::flush; // file int
        out << std::to_string(leaf->fileInfo[i][1]) << "," << std::flush; // file word cnt
        out << std::to_string(leaf->fileInfo[i][2]) << "," << std::flush; // file is title word
    }

    // output how many children
    size_t fCnt(0);
    for (size_t i(0); i<ARR_CHAR_SIZE; ++i) {
        if (leaf->nArr[i] != nullptr) { ++fCnt; }
    }
    out << fCnt << "," << std::flush;

    // recursive call on all children
    for (size_t i(0); i<ARR_CHAR_SIZE; ++i) {
        if (leaf->nArr[i] != nullptr) { recSerialize(leaf->nArr[i], out); }
    }

}

/**
 * recDeserialize --
 * recursively read through the standard form of a serialized document and apply values
 * @param leaf - current TNode
 * @param scnr - instream
 */
void GrandTrie::recDeserialize(GrandTrie::GNodePtr &leaf, std::istream
    &scnr) {

    size_t numFiles(std::stoi(delimitScnr(scnr))); // get number of files with this word

    // create new file info if there are files
    size_t fInt, fWCnt, fIsTitle;
    for (size_t i(0); i<numFiles; ++i) {
        fInt = (size_t)std::stoi(delimitScnr(scnr));
        fWCnt = (size_t)std::stoi(delimitScnr(scnr));
        fIsTitle = (size_t)std::stoi(delimitScnr(scnr));

        leaf->fileInfo.push_back({fInt,fWCnt,fIsTitle});
    }

    // get the number of child nodes
    size_t numTNodes(std::stoi(delimitScnr(scnr)));

    // go recursive on those kids
    std::string nVal = "";
    for (size_t i(0); i<numTNodes; ++i) {
        nVal = delimitScnr(scnr);
        leaf->nArr[charIndex[nVal.at(0)]] = new GNode;
        leaf->nArr[charIndex[nVal.at(0)]]->val = nVal;
        recDeserialize(leaf->nArr[charIndex[nVal.at(0)]], scnr);
    }

}
//endregion

/**
 * rewritArt --
 * Rewrites the saved data from ArtAnalyzer object into new radix trie.  Overwrites current
 * data in trie
 * @param og -- ArtAnalyzer
 */
void GrandTrie::rewritArt(ArtAnalyzer &og) {
    size_t j(0);

    //load in file names and map to int val
    for ( wordMap::iterator iter = (og.fileCnt).begin(); iter != (og.fileCnt).end(); ++iter) {
        fileDat.push_back({iter->first,iter->second});

        fnameToInt[iter->first] = j;
        ++j;
    }

    // call recursive for loading Trie
    delete head;
    head = new GNode;
    for (size_t i(0); i<ARR_CHAR_SIZE; ++i) {
        if (og.head->nArr[i] != nullptr) {
            head->nArr[i] = new GNode;
            recRewritArt(head->nArr[i], og.head->nArr[i], "");
        }
    }

}

/**
 * printWords --
 * prints all words to ostream
 * @param out - ostream being printed to
 */
void GrandTrie::printWords(std::ostream &out) {
    out << "number of files: " << fileDat.size() << std::endl;
    for (size_t i(0); i<fileDat.size(); ++i) {
        out << std::get<0>(fileDat.at(i)) << " : "
            << std::get<1>(fileDat.at(i)) << " valid words" << std::endl;
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
void GrandTrie::printWords(std::ostream &out, const std::string &fname) {
    size_t fInt(0);
    bool fnd(false);
    for (fInt = 0; fInt<fileDat.size() && !fnd; ++fInt) {
        if (std::get<0>(fileDat.at(fInt)) == fname) { fnd = true; }
    }

    if (!fnd) {
        out << "no such file found" << std::endl;
        return;
    }

    out << fname << " : " << std::get<1>(fileDat.at(fInt)) << " valid words" << std::endl;

    for (size_t i(0); i<ARR_CHAR_SIZE; ++i) {
        recPrintWords(out, fname, "", head->nArr[i]);
    }
    out << std::endl;
}

/**
* serialize --
* serializes all data into standard format:
* format for Trie data: strVal,numFiles[,1fileInt,1fileWCnt,1fileIsTitle,...nfileIsTitle],
 * childCnt,[childDat]
* If numFiles is zero, skip all fileDat. If childCnt is zero, skip childDat, otherwise childDat is
* recursive call on all children.
* @param out - outstream
*/
void GrandTrie::serialize(std::string outName) {

    std::ofstream out;
    ArtAnalyzer::openFile(out, outName);

    out << fileDat.size() << "," << std::flush; // output number of different files

    // output all file names and respective total word count, comma separated
    for (size_t i(0); i<fileDat.size(); ++i) {
        out << std::get<0>(fileDat.at(i)) << "," << std::get<1>(fileDat.at(i)) << "," << std::flush;
    }
    out << "\n"  << std::flush;

    // output Trie data, starting with head node
    out << head->val << "," << 0 << "," << std::flush;
    // count all children that aren't null
    size_t childCnt(0);
    for (size_t i(0); i<ARR_CHAR_SIZE; ++i) {
        if (head->nArr[i] != nullptr) { ++childCnt; }
    }
    out << childCnt << "," << std::flush;

    // recursive call on all children
    for (size_t i(0); i<ARR_CHAR_SIZE; ++i) {
        if (head->nArr[i] != nullptr) { recSerialize(head->nArr[i], out); }
    }
    out << "\n";

    out.close();
}

/**
 * deserialize --
 * deserializes standardized data from fname.txt
 * @param fname - file name
 */
void GrandTrie::deserialize(const std::string &fname) {
    std::ifstream inF;
    ArtAnalyzer::openFile(inF, fname);

    GrandTrie empty;

    // get the files and their total word counts
    size_t numFiles(std::stoi(delimitScnr(inF))); // get number of files
    std::string ftitle("");
    for (size_t i(0); i<numFiles; ++i) {
        ftitle = delimitScnr(inF); // get file name
        empty.fnameToInt[ftitle];
        empty.fnameToInt[ftitle] = i;

        // assign file and word count values
        (empty.fileDat).push_back({ftitle,std::stoi(delimitScnr(inF))});
    }
    char nh(inF.get()); // get the newline char

    empty.head->val = delimitScnr(inF);
    recDeserialize(empty.head, inF);

    std::swap(this->head,empty.head);
    this->fileDat = empty.fileDat;
    this->fnameToInt = empty.fnameToInt;
}

