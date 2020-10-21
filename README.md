# Directory Search Engine

## Description
### Objective

The objective for this project is to create a cpp program that searches through any directory of your choice and know the priorities of which files to look through based on relevance to your input. 

For illustration, imagine: you run the executable, you plug in the path of your foo/ directory, and you can then search through all of its immediate files (not just their names!) using a sentence.  

The reason for this project is because I have consistenty been finding myself downloading a number of pdfs and "*command f-ing*" a bunch of words to see if that file had what I wanted or not.  Additionally the thought of creating my own personal search engine sounds fascinating.

### Outline (as of 10/21/2020)

#### General
We'll need a class to hold standard information about each file, let's call that class DirFile.  Now, for searching and organizing, I toying with the idea of a radix trie: every string attached to every file will be in it, and at the end of the string will be a container of DirFile pointers for every DirFile that contains that word.  Our trie will use nodes that we can call RadNode.
We will output our data into a serialized file for saving our data, allowing for quicker reload time after the initial compilation/review of the files in our directory.  The file will keep track of the last compilation's observed files and the last time these files were edited: thus, if we run our compilation again, we will know if we need to go through new files/newly edited files or even delete old files that are no longer in our directory.

Potentially, there could be an option to go through all directories within our target directory (non-immediates included).

#### Specific Objects

##### **DirFile Class**
Holds standard information on every file.

###### DirFile Member Variables:
variable type and name | description
-----------------------|------------
std::string filename | the full filename, including the file type
? date ? lastModified | the date of the last modification prior to its observation/analysis of this program #todo: ## figure out a good variable type for dates
uint32_t wordCount | an unsigned integer of all the words in the file #todo: ## decide if this should only include valid trie words or any
? file type ? | specifically holds the file type
? title ? | the title of the file within the file itself, **not** the filename

###### DireFile Methods:
method signature | description
-----------------|------------
const &std::string getFilename() const; | returns filename
void setFilename(std::string fName); | assigns filename
const ? &date ? getLastModified() const; | returns lastModified
void setLastModified(? date ? lastDate); | assigns lastModified
const uint32_t getWordCount() const; | returns wordCount
const std::string getFileType() const; | returns the filetype (ex: .txt, .md, .pdf, etc)
void changeWordCnt(int32_t d); | changes the wordCount by d
void setWordCount(uint32_t num); | sets the wordCount as num
void incWC(uint32_t inc = 1); | increments the wordCount, default value is one
void decWC(uint32_t dec = 1); | decrements the wordCount, default value is one


##### **RadNode Template Class <T>**
#todo: ## structure will depend on final decision of trie structure, currently researching best potential options... open to suggestions.  I'm reading further into burst tries at the moment, see [article](https://people.eng.unimelb.edu.au/jzobel/fulltext/acmtois02.pdf)