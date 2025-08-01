#include <iostream>
#include "fileSystem.cpp"
using namespace std;

int main(int argCount, char* args[]) {
    //Initializing variables
    int input; string realFileName, simFileName; int blockNum;
    bool running = true;
    fileSystem fileSystem;
    if (!fileSystem.allocationType(args[1])) {running = false;}     //Tells fileSystem which file allocation method was chosen

    //User input loop
    while(running) {
        printf("\n1) Display a file\n");
        printf("2) Display the file table\n");
        printf("3) Display the free space bitmap\n");
        printf("4) Display a disk block\n");
        printf("5) Copy a file from the simulation to a file on the real system\n");
        printf("6) Copy a file from the real system to a file in the simulation\n");
        printf("7) Delete a file\n");
        printf("8) Exit\n");
        printf("\nChoice: ");
        cin >> input;

        //File system functions
        switch (input) {
            case 1:
                //Display file
                cout << "File name: ";
                cin >> simFileName;
                fileSystem.DisplayFile(simFileName);
                break;
            case 2:
                //Display file table
                fileSystem.DisplayFileTable();
                break;
            case 3:
                //Display bitmap
                fileSystem.DisplayBitmap();
                break;
            case 4:
                //Display a block
                cout << "Block number: ";
                cin >> blockNum;
                fileSystem.DisplayBlock(blockNum);
                break;
            case 5:
                //Copy from sim to real
                cout << "Copy from: ";
                cin >> simFileName;
                cout << "Copy to: ";
                cin >> realFileName;
                fileSystem.CopyToSystem(realFileName, simFileName);
                break;
            case 6:
                //Copy from real to sim
                cout << "Copy from: ";
                cin >> realFileName;
                cout << "Copy to: ";
                cin >> simFileName;
                fileSystem.CopyToSim(realFileName, simFileName);
                break;
            case 7:
                //Delete file
                cout << "File name: ";
                cin >> simFileName;
                fileSystem.DeleteFile(simFileName);
                break;
            case 8:
                //Exit
                running = false;
                break;
            default:
                break;
        }
    }//End of while loop
}
