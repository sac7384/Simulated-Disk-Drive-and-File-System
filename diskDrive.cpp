#include <iostream>
using namespace std;

class diskDrive {
    //Disk drive simulated as a 2d array
    //256 blocks of 512 bytes each
    unsigned char drive[256][512];

public:
    //Initializes bitmap with blocks 0 and 1 occupied and everything else empty
    void InitializeBitmap() {
        //Sets first two bytes of bitmap to 1 and rest to 0 on startup
        drive[1][0] = '1'; drive[1][1] = '1';
        for (int i = 2; i < 256; i++) {
            drive[1][i] = '0';
        }
    }

    //Changes a value in the bitmap
    void UpdateBitmap(int blockIndex, unsigned char value) {
        //Updates a value in the bitmap
        drive[1][blockIndex] = value;
    }

    //Changes a single character in one block
    void WriteChar(int blockIndex, int charIndex, unsigned char byte) {
        drive[blockIndex][charIndex] = byte;
    }

    //Overwrites an entire block
    void WriteBlock(int blockIndex, unsigned char bytes[512]) {
        for (int i = 0; i < 512; i++) {
            if (bytes[i] == 0) {break;}
            drive[blockIndex][i] = bytes[i];
        }
    }

    //Overwrites a block, and adds in the chaining method's pointer at the end
    void WriteChainedBlock(int blockIndex, unsigned char bytes[509], int nextIndex) {
        for (int i = 0; i < 509; i++) {
            if (bytes[i] == 0) {break;}
            drive[blockIndex][i] = bytes[i];
        }
        if (nextIndex == -1) {return;}
        if (nextIndex > 99) {drive[blockIndex][509] = to_string(nextIndex)[0];
                             drive[blockIndex][510] = to_string(nextIndex)[1];}
        else if (nextIndex > 9)  {drive[blockIndex][510] = to_string(nextIndex)[0];}
        drive[blockIndex][511] = to_string(nextIndex % 10)[0];
    }

    //Deletes all data in a block
    void DeleteBlock(int blockIndex) {
        for (int i = 0; i < 512; i++) {
            drive[blockIndex][i] = 0;
        }
        UpdateBitmap(blockIndex, '0');
    }

    //Deletes a chained block and returns the index of the next block in the chain
    int DeleteChainedBlock(int blockIndex) {
        //Get nextIndex
        int nextIndex = -1;
        if (drive[blockIndex][511] != 0) {
            nextIndex = drive[blockIndex][511] - '0';
            if (drive[blockIndex][510] != 0) { nextIndex += 10 * (drive[blockIndex][510] - '0'); }
            if (drive[blockIndex][509] != 0) { nextIndex += 100 * (drive[blockIndex][509] - '0'); }
        }

        //Delete block and update bitmap
        DeleteBlock(blockIndex);

        return nextIndex;
    }

    //Returns a whole block
    unsigned char* Read(int blockIndex) {
        return drive[blockIndex];
    }
};