#include <iostream>
#include <fstream>
#include <vector>
#include "diskDrive.cpp"
using namespace std;

class fileSystem {
    //Objects
    diskDrive disk;
    //Variables
    bool contiguous = false;    //File stays together, blocks are all next to each other
    bool chained = false;       //Linked list style, blocks are anywhere and point to next block
    bool indexed = false;       //File has starting block with indexes of all blocks in file

public:
    //Initializes the system with the inputted file allocation method
    bool allocationType(string method) {
        //Checks for which file allocation method should be used
        if (method == "contiguous") {contiguous = true;}
        else if (method == "chained") {chained = true;}
        else if (method == "indexed") {indexed = true;}
        else {printf("You did not provide a allowed file allocation method\n"); return false;}
        disk.InitializeBitmap();
        return true;
    }

    //Returns starting block of a file
    int FindStartingBlock(string fileName) {
        //Read in file table
        int fileAddress; int startingBlock;
        bool nameMatch;
        unsigned char* table;
        table = disk.Read(0);

        //Checks file table for a matching file name
        if (indexed) {
            for (int i = 0; i < 46; i++) {
                //First 8 characters = file name, next 3 char = starting block
                fileAddress = i * 11;
                nameMatch = true;
                if (table[fileAddress] == 0) {continue;}
                for (int j = 0; j < 8; j++) {   //Check if names match
                    if (j >= fileName.length()) {
                        if (table[fileAddress + j] != 0) {nameMatch = false;}
                        break;
                    }
                    if (table[fileAddress + j] != fileName[j]) {nameMatch = false;}
                }
                if (nameMatch) {    //If names matched, copy starting block value
                    startingBlock = table[fileAddress + 10] - '0';
                    if (table[fileAddress + 9] != 0) {startingBlock += 10 * (table[fileAddress + 9] - '0');}
                    if (table[fileAddress + 8] != 0) {startingBlock += 100 * (table[fileAddress + 8] - '0');}
                    return startingBlock;
                }
            }//End of file table search loop

            //Exit with error if there is no matching file
            cout << "Error: No file with given name was found\n";
            return -1;
        }//End of indexed file table search

        else {  //For contiguous and chained
            for (int i = 0; i < 39; i++) {
                //First 8 characters = file name, next 3 char = starting block, next 2 char = file length
                fileAddress = i * 13;
                nameMatch = true;
                if (table[fileAddress] == 0) {continue;}
                for (int j = 0; j < 8; j++) {   //Check if names match
                    if (j >= fileName.length()) {
                        if (table[fileAddress + j] != 0) {nameMatch = false;}
                        break;
                    }
                    if (table[fileAddress + j] != fileName[j]) {nameMatch = false;}
                }
                if (nameMatch) {    //If names matched, copy starting block value
                    startingBlock = table[fileAddress + 10] - '0';
                    if (table[fileAddress + 9] != 0) {startingBlock += 10 * (table[fileAddress + 9] - '0');}
                    if (table[fileAddress + 8] != 0) {startingBlock += 100 * (table[fileAddress + 8] - '0');}
                    return startingBlock;
                }
            }//End of file table search loop

            //Exit with error if there is no matching file
            cout << "Error: No file with given name was found\n";
            return -1;
        }//End of contiguous and chained file table search
    }//End of FineStartingBlock

    //Returns block length of a file
    int FindBlockLength(string fileName) {
        //Read in file table
        int fileAddress; int blockLength;
        bool nameMatch;
        unsigned char* table;
        table = disk.Read(0);

        for (int i = 0; i < 39; i++) {
            //First 8 characters = file name, next 3 char = starting block, next 2 char = file length
            fileAddress = i * 13;
            nameMatch = true;
            if (table[fileAddress] == 0) {continue;}
            for (int j = 0; j < 8; j++) {   //Check if names match
                if (j >= fileName.length()) {
                    if (table[fileAddress + j] != 0) {nameMatch = false;}
                    break;
                }
                if (table[fileAddress + j] != fileName[j]) {nameMatch = false;}
            }
            if (nameMatch) {    //If names matched, copy block length value
                blockLength = table[fileAddress + 12] - '0';
                if (table[fileAddress + 11] != 0) {blockLength += 10 * (table[fileAddress + 11] - '0');}
                return blockLength;
            }
        }//End of file table search loop

        //Exit if there is no matching file
        return -1;
    }//End of FindBlockLength

    //Searches for correct name in file table, then displays file
    void DisplayFile(string fileName) {
        int startingBlock = FindStartingBlock(fileName);
        int blockLength =  FindBlockLength(fileName);

        //Exit if no file was found
        if (startingBlock == -1) {return;}

        //Find file using file allocation method
        if (contiguous) {
            //Display file using starting block and block length
            for (int i = startingBlock; i < (startingBlock + blockLength); i++) {
                DisplayBlock(i);
            }
        }

        else if (chained) {
            while (startingBlock != -1) {
                //Display file by following chain of blocks
                //DisplayChainedBlock returns index of next block
                startingBlock = DisplayChainedBlock(startingBlock);
            }
        }

        else if (indexed) {
            unsigned char* bytes; int index = 0; int blockNumber;
            bytes = disk.Read(startingBlock);
            while (bytes[index + 2] != 0) {
                //Display file by following indexes in startingBlock
                blockNumber = bytes[index + 2] - '0';
                if (bytes[index + 1] != 0) {blockNumber += 10 * (bytes[index + 1] - '0');}
                if (bytes[index] != 0) {blockNumber += 100 * (bytes[index] - '0');}
                DisplayBlock(blockNumber);
                index += 3;
            }
        }
    }

    //Displays file table, line by line
    void DisplayFileTable() {
        int fileAddress;
        unsigned char* table;
        table = disk.Read(0);

        //Indexed file table
        if (indexed) {
            for (int i = 0; i < 46; i++) {
                //Starts from beginning of table entry and prints info
                //First 8 characters = file name, next 3 char = index block
                fileAddress = i * 11;
                if (table[fileAddress] == 0) { continue; } //Skips entry if no file there
                //Prints file name
                cout << table[fileAddress] << table[fileAddress + 1] << table[fileAddress + 2] << table[fileAddress + 3]
                     << table[fileAddress + 4] << table[fileAddress + 5] << table[fileAddress + 6]
                     << table[fileAddress + 7] << "\t";
                //Prints index block
                if (table[fileAddress + 8] != '0') { cout << table[fileAddress + 8]; }
                if (table[fileAddress + 9] != '0') { cout << table[fileAddress + 9]; }
                cout << table[fileAddress + 10] << "\n";
            }//End of for loop
        }
        //Contiguous and chained file tables
        else {
            for (int i = 0; i < 39; i++) {
                //Starts from beginning of table entry and prints info
                //First 8 characters = file name, next 3 char = starting block, next 2 char = file length
                fileAddress = i * 13;
                if (table[fileAddress] == 0) { continue; } //Skips entry if no file there
                //Prints file name
                cout << table[fileAddress] << table[fileAddress + 1] << table[fileAddress + 2] << table[fileAddress + 3]
                     << table[fileAddress + 4] << table[fileAddress + 5] << table[fileAddress + 6]
                     << table[fileAddress + 7] << "\t";
                //Prints starting block
                if (table[fileAddress + 8] != '0') { cout << table[fileAddress + 8]; }
                if (table[fileAddress + 9] != '0') { cout << table[fileAddress + 9]; }
                cout << table[fileAddress + 10] << "\t";
                //Prints file length
                if (table[fileAddress + 11] != '0') { cout << table[fileAddress + 11]; }
                cout << table[fileAddress + 12] << "\n";
            }//End of for loop
        }
    }//End of DisplayFileTable

    //Displays free space bitmap as 1s and 0s
    void DisplayBitmap() {
        unsigned char* bitmap;
        bitmap = disk.Read(1);
        for (int i = 0; i < 256; i++) {
            cout << bitmap[i];
            if ((i + 1) % 32 == 0) {cout << "\n";}
        }
    }

    //Displays all bytes in a block
    void DisplayBlock(int blockNum) {
        unsigned char* block;
        block = disk.Read(blockNum);
        for (int i = 0; i < 512; i++) {
            if (block[i] == 0) {continue;}
            cout << block[i];
        }
        cout << "\n";
    }

    //Displays all bytes in a chained block, also returns the index of the next block in the file
    int DisplayChainedBlock(int blockNum) {
        unsigned char* block;
        block = disk.Read(blockNum);
        for (int i = 0; i < 509; i++) {
            if (block[i] == 0) {continue;}
            cout << block[i];
        }
        cout << "\n";

        //Read and return nextIndex, makes displaying chained files easier
        if (block[511] == 0) {return -1;}
        int nextIndex = block[511] - '0';
        if (block[510] != 0) {nextIndex += 10 * (block[510] - '0');}
        if (block[509] != 0) {nextIndex += 100 * (block[509] - '0');}
        return nextIndex;
    }

    //Copies a file from the simulation to the real system
    void CopyToSystem(string realFileName, string simFileName) {
        ofstream outfile(realFileName, ios::binary);
        vector<char> bytes;
        int startingBlock = FindStartingBlock(simFileName);
        int blockLength;
        if (!indexed) {blockLength = FindBlockLength(simFileName);}
        int blockSize = 512;
        if (chained) {blockSize = 509;}
        if (!indexed) {bytes.resize(blockSize * blockLength);}
        unsigned char* readBuffer;

        //Read data from simulation
        if (contiguous) {
            //Loops copy data from file blocks to char vector
            for (int i = 0; i < blockLength; i++) {
                readBuffer = disk.Read(startingBlock + i);
                for (int j = 0; j < blockSize; j++) {
                    bytes[(i * blockSize) + j] = readBuffer[j];
                }
            }
        }
        else if (chained) {
            //Loops copy data from file blocks to char vector
            for (int i = 0; i < blockLength; i++) {
                readBuffer = disk.Read(startingBlock);
                for (int j = 0; j < blockSize; j++) {
                    bytes[(i * blockSize) + j] = readBuffer[j];
                }
                //Sets startingBlock to the next index
                if (readBuffer[511] == 0) {break;}
                startingBlock = readBuffer[511] - '0';
                if (readBuffer[510] != 0) { startingBlock += 10 * (readBuffer[510] - '0'); }
                if (readBuffer[509] != 0) { startingBlock += 100 * (readBuffer[509] - '0'); }
            }
        }
        else if (indexed) {
            unsigned char* indexBlock; int indexList[20]; int index = 0; int blockNumber;
            indexBlock = disk.Read(startingBlock);
            while (indexBlock[index + 2] != 0) {
                //Stores block indexes in indexList
                blockNumber = indexBlock[index + 2] - '0';
                if (indexBlock[index + 1] != 0) {blockNumber += 10 * (indexBlock[index + 1] - '0');}
                if (indexBlock[index] != 0) {blockNumber += 100 * (indexBlock[index] - '0');}
                indexList[index / 3] = blockNumber;
                index += 3;
            }
            blockLength = index / 3;
            bytes.resize(blockSize * blockLength);

            for (int i = 0; i < blockLength; i++) {
                readBuffer = disk.Read(indexList[i]);
                for (int j = 0; j < blockSize; j++) {
                    bytes[(i * blockSize) + j] = readBuffer[j];
                }
            }
        }

        outfile.write(&bytes[0], blockSize * blockLength);
        cout << "File " << simFileName << " copied\n";
    }//End of CopyToSystem

    //Copies a file from the real system to the simulation
    void CopyToSim(string realFileName, string simFileName) {
        //Reads file into temporary character vector
        ifstream infile(realFileName, ios::binary);
        infile.seekg(0, ios::end);
        size_t length = infile.tellg();
        infile.seekg(0, ios::beg);
        vector<char> bytes;
        bytes.resize(length);
        infile.read(&bytes[0], length);

        //If new file name is not lowercase, exit with error
        for (char c: simFileName) {
            if (!islower(c)) {cout << "Error: File name must be lowercase\n"; return;}
        }

        //Calculate number of blocks necessary, error if too large
        int blockSize = 512;
        if (chained) {blockSize = 509;}     //Chained method has smaller blocks, last 3 bytes are needed for a pointer to the next block
        int blockLength = 1;
        while (length > blockSize) {
            length -= blockSize;
            blockLength++;
        }
        if (blockLength > 10) {cout << "Error: File cannot be larger than 10 blocks";}
        //Initializes temp array to 0
        unsigned char tempBlocks[10][blockSize];
        for (int i = 0; i < blockLength; i++) {
            for (int j = 0; j < blockSize; j++) {
                tempBlocks[i][j] = 0;
            }
        }

        //Moves data from vector to unsigned char arrays for each block
        for (int i = 0; i < blockLength; i++) {
            for (int j = 0; j < blockSize; j++) {
                if (i == (blockLength - 1) && j >= length) {break;}
                tempBlocks[i][j] = bytes[(i * blockSize) + j];
            }
        }

        //Write file to the simulation
        int startingBlock;
        unsigned char* bitmap;
        bitmap = disk.Read(1);
        if (contiguous) {
            //Check bitmap for large enough space for file
            int blockCounter = 0;
            for (int i = 2; i < 256; i++) {
                if (blockCounter == 0) {startingBlock = i;}

                if (bitmap[i] == '0') {blockCounter++;}
                else if (bitmap[i] == '1') {blockCounter = 0;}

                if (blockCounter >= blockLength) {break;}
            }

            //Out of memory error
            if (blockCounter < blockLength) {cout << "Error: Not enough memory on disk\n"; return;}

            //Copy file data to the correct blocks
            blockCounter = 0;
            for (int i = startingBlock; i < (startingBlock + blockLength); i++) {
                disk.WriteBlock(i, tempBlocks[blockCounter]);
                disk.UpdateBitmap(i, '1');
                blockCounter++;
            }
        }//End of contiguous file allocation

        else if (chained) {
            //Check bitmap for empty blocks
            int freeBlocks[blockLength];
            int blockCounter = 0;
            for (int i = 2; i < 256; i++) {
                if (bitmap[i] == '0') {
                    freeBlocks[blockCounter] = i;
                    blockCounter++;
                    if (blockCounter >= blockLength) {break;}
                }
            }

            //Out of memory error
            if (blockCounter < blockLength) {cout << "Error: Not enough memory on disk\n"; return;}

            //Use list of available blocks to allocate data
            startingBlock = freeBlocks[0];
            for (int i = 0; i < blockLength - 1; i++) {
                disk.WriteChainedBlock(freeBlocks[i], tempBlocks[i], freeBlocks[i + 1]);
                disk.UpdateBitmap(freeBlocks[i], '1');
            }
            disk.WriteChainedBlock(freeBlocks[blockLength - 1], tempBlocks[blockLength - 1], -1);
            disk.UpdateBitmap(freeBlocks[blockLength - 1], '1');


        }//End of chained file allocation
        else if (indexed) {
            //Check bitmap for empty blocks
            int freeBlocks[blockLength + 1];
            int blockCounter = 0;
            for (int i = 2; i < 256; i++) {
                if (bitmap[i] == '0') {
                    freeBlocks[blockCounter] = i;
                    blockCounter++;
                    if (blockCounter >= blockLength + 1) {break;}
                }
            }

            //Out of memory error
            if (blockCounter < blockLength + 1) {cout << "Error: Not enough memory on disk\n"; return;}

            //Use list of available blocks to allocate data
            //Starting block has indexes for all block locations, each index uses 3 char
            startingBlock = freeBlocks[0];
            disk.UpdateBitmap(freeBlocks[0], '1');
            for (int i = 0; i < blockLength; i++) {
                //Set index
                disk.WriteChar(startingBlock, (i * 3) + 2, to_string(freeBlocks[i + 1] % 10)[0]);
                if (freeBlocks[i + 1] > 9) {disk.WriteChar(startingBlock, (i * 3) + 1, to_string(freeBlocks[i + 1] % 100)[0]);}
                if (freeBlocks[i + 1] > 99) {disk.WriteChar(startingBlock, (i * 3), to_string(freeBlocks[i + 1])[0]);}
                //Copy data
                disk.WriteBlock(freeBlocks[i + 1], tempBlocks[i]);
                disk.UpdateBitmap(freeBlocks[i + 1], '1');
            }
        }//End of indexed file allocation

        //Add new file info to file table
        unsigned char* table;
        table = disk.Read(0);
        if (indexed) {
            for (int i = 0; i < 46; i++) {
                //Add info to first empty entry
                //First 8 characters = file name, next 3 char = starting block
                if (table[i * 11] == 0) {
                    //Name
                    for (int j = 0; j < 8; j++) {
                        if (j > simFileName.length()) {break;}
                        disk.WriteChar(0, (i * 11) + j, simFileName[j]);
                    }
                    //Starting block
                    if (startingBlock > 99) {
                        disk.WriteChar(0, (i * 11) + 8, to_string(startingBlock)[0]);
                        disk.WriteChar(0, (i * 11) + 9, to_string(startingBlock)[1]);
                    }
                    else if (startingBlock > 9) {disk.WriteChar(0, (i * 11) + 9, to_string(startingBlock)[0]);}
                    disk.WriteChar(0, (i * 11) + 10, to_string(startingBlock % 10)[0]);
                    break;
                }
            }
        }
        else {//Contiguous and chained tables
            for (int i = 0; i < 39; i++) {
                //Add info to first empty entry
                //First 8 characters = file name, next 3 char = starting block, next 2 char = file length
                if (table[i * 13] == 0) {
                    //Name
                    for (int j = 0; j < 8; j++) {
                        if (j > simFileName.length()) {break;}
                        disk.WriteChar(0, (i * 13) + j, simFileName[j]);
                    }
                    //Starting block
                    if (startingBlock > 99) {
                        disk.WriteChar(0, (i * 13) + 8, to_string(startingBlock)[0]);
                        disk.WriteChar(0, (i * 13) + 9, to_string(startingBlock)[1]);
                    }
                    else if (startingBlock > 9) {disk.WriteChar(0, (i * 13) + 9, to_string(startingBlock)[0]);}
                    disk.WriteChar(0, (i * 13) + 10, to_string(startingBlock % 10)[0]);
                    //File length
                    if (blockLength > 9) {disk.WriteChar(0, (i * 13) + 11, to_string(blockLength)[0]);}
                    disk.WriteChar(0, (i * 13) + 12, to_string(blockLength % 10)[0]);
                    break;
                }
            }
        }

        cout << "File " << realFileName << " copied\n";
    }//End of CopyToSim

    //Deletes all blocks in a file
    void DeleteFile(string fileName) {
        int startingBlock = FindStartingBlock(fileName);

        //Exit if no file was found
        if (startingBlock == -1) {return;}

        //Finds and deletes blocks based on allocation method
        if (contiguous) {
            int blockLength = FindBlockLength(fileName);
            for (int i = startingBlock; i < (startingBlock + blockLength); i++) {
                disk.DeleteBlock(i);
            }
        }
        else if (chained) {
            while (startingBlock != -1) {
                startingBlock = disk.DeleteChainedBlock(startingBlock);
            }
        }
        else if (indexed) {
            unsigned char* bytes; int index = 0; int blockNumber;
            bytes = disk.Read(startingBlock);
            while (bytes[index + 2] != 0) {
                //Delete blocks by following indexes in startingBlock
                blockNumber = bytes[index + 2] - '0';
                if (bytes[index + 1] != 0) {blockNumber += 10 * (bytes[index + 1] - '0');}
                if (bytes[index] != 0) {blockNumber += 100 * (bytes[index] - '0');}
                disk.DeleteBlock(blockNumber);
                index += 3;
            }
            disk.DeleteBlock(startingBlock);
        }
        DeleteTableEntry(fileName);
        cout << "File " << fileName << " deleted\n";
    }//End of DeleteFile

    //Delete an entry in the file table
    void DeleteTableEntry(string fileName) {
        //Read in file table
        int fileAddress; bool nameMatch;
        unsigned char* table;
        table = disk.Read(0);

        //Checks file table for a matching file name
        if (indexed) {
            for (int i = 0; i < 46; i++) {
                //First 8 characters = file name, next 3 char = starting block
                fileAddress = i * 11;
                nameMatch = true;
                if (table[fileAddress] == 0) {continue;}
                for (int j = 0; j < 8; j++) {   //Check if names match
                    if (j >= fileName.length()) {
                        if (table[fileAddress + j] != 0) {nameMatch = false;}
                        break;
                    }
                    if (table[fileAddress + j] != fileName[j]) {nameMatch = false;}
                }
                if (nameMatch) {    //If names matched, delete table entry
                    for (int k = fileAddress; k < (fileAddress + 11); k++) {
                        disk.WriteChar(0, k, 0);
                    }
                    break;
                }
            }//End of file table search loop

            //Exit with error if there is no matching file
            if (!nameMatch) {cout << "Error: No file with given name was found\n";}
        }//End of indexed file table search

        else {  //For contiguous and chained
            for (int i = 0; i < 39; i++) {
                //First 8 characters = file name, next 3 char = starting block, next 2 char = file length
                fileAddress = i * 13;
                nameMatch = true;
                if (table[fileAddress] == 0) {continue;}
                for (int j = 0; j < 8; j++) {   //Check if names match
                    if (j >= fileName.length()) {
                        if (table[fileAddress + j] != 0) {nameMatch = false;}
                        break;
                    }
                    if (table[fileAddress + j] != fileName[j]) {nameMatch = false;}
                }
                if (nameMatch) {    //If names matched, delete table entry
                    for (int k = fileAddress; k < (fileAddress + 13); k++) {
                        disk.WriteChar(0, k, 0);
                    }
                    break;
                }
            }//End of file table search loop

            //Exit with error if there is no matching file
            if (!nameMatch) {cout << "Error: No file with given name was found\n";}
        }//End of contiguous and chained file table search
    }//End of DeleteTableEntry
};