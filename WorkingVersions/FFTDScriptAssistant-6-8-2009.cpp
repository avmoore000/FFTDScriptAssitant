// Andrew Moore
// 06-08-2009
// Arctic Region Supercomputing Center

// This is a tool written to prepare data for Fast Fourier Transforms.

#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>

using namespace std;

void oneDimensionalTransform(int[],stringstream[]);  // Will be called if you are performing one dimensional Fourier Transforms
void twoDimensionalTransform(int[],stringstream[]); // Will be called if you are performing two dimensional Fourier Transforms
void fftTransform(int); // This function will perform the majority of the work with preparing the command line.
void instructions(); // Will display instructions about what the program is asking for.



int main()
{

	int choice = 0;  // Will allow you to choose which transform to perform

	while (choice != 4) // This will be the main menu loop
	{

		cout << "Fast Fourier Transform Assistant" << endl << endl;
		cout << "1. One dimensional Fast Fourier transforms." << endl;
		cout << "2. Two dimensional Fast Fourier transforms." << endl;
		cout << "3. Instructions" << endl;
		cout << "4. Quit" << endl;
		cout << "Please make a choice (1 - 4):  ";
		cin >> choice;

		switch (choice)
		{
			case 1:  // One dimensional transform
			case 2:  // Two dimensional transform
			{
				fftTransform(choice);
				break;
			}
			case 3: // Call up the instructions
			{
				instructions();
				break;
			}
			case 4: //Quit
			{
				cout << "Thank you for using this assistant.  Have a nice day." << endl;
				break;
			}
			default: //Invalid choice
				cout << "That is not a valid option, please make a choice 1 - 3." << endl << endl;
				break;
		}	
	}

	return 0;
}

void instructions () //Function Displays instructions about how to use the program.
{

	cout << "The following is an example of the input format: " << endl << endl
		<< "One Dimensional Fourier Transforms: \n"
		<< "\t c2c X1 X2 X3 X4 \n"
		<< "Where c2c is the FFT_Type, X1 is the number of fft's, \n"
		<< "X2 is the number of SPU's to use, X3 is the size of the fft's, \n"
		<< "and X4 is the hugepage_flag." << endl << endl;
	cout << "Two Dimensional Fourier Transforms: \n"
		<< "\t c2c X1 X2 X3 X4 X5 X6 X7 X8 X9 \n"
		<< "Where c2c is once again the FFT_Type, and the X1-X9 variables \n"
		<< "correspond to other numeric parameters of the transform." << endl << endl;

	return;
}

void fftTransform (int choice)
{
	string charParameter;						// Will hold the character parameter (such as 'c2c')
	string command;							// This string will contain the command line.
	string outputFilePrefix, outFileName;				// These will hold the prefix and the output file name.
	
	std::stringstream convert[8];	                                // Array for converting the integers array into strings for use in the command line.

	const char *sendCommand;					// Will contain the command line as a sequence of characters in a c-style string.

	char exit;							// Will allow you to nicely exit the program.

	int numericVariables[8];					// Holds the numeric parameters of the FFT.

	cout << "Please enter today's date (no /'s or \'s please!):  ";
	cin >> outputFilePrefix;

	cout << "Please enter the type of Fourier Transform to be performed (i.e. c2c):  ";
	cin >> charParameter;

	switch (choice)
	{
		case 1:
		{
	
  			oneDimensionalTransform(numericVariables,convert); // Set up the parameters for the one dimensional FFT
			command += "./FFT1D_sample " + charParameter + " ";
			
			for (int i = 0; i < 4; i++) // Add numeric parameters to the command line.
				command += convert[i].str() + " ";
			break;
		}
		case 2:
		{
			twoDimensionalTransform(numericVariables,convert); // Set up the parameters for the two dimensional FFT
			command += "./FFT2D_sample " + charParameter + " ";
	
			for (int i = 0; i < 8; i++) // Add numeric parameters to the command line.
				command += convert[i].str() + " ";
			break;
		}
	}

	command += "| tee " + outFileName + ".txt"; // This will cause the output to be put in the file you have specified.

	sendCommand = command.c_str();	// Create the character string to be used in the actual system call.

	cout << "The send command is equal to " << sendCommand << endl;	//Check to make sure the command was assembled correctly.

	while (true) // Run the command in a loop until the program is killed manually (going to fix this eventually)
	{
		system(sendCommand);
		sleep(1);	//Pause one second between loops.
	}
	
	return;
}

void oneDimensionalTransform(int number[], stringstream conv[])
{

	for (int i = 0; i < 4; i++)  //Loop will get all of the numeric parameters from the user (instead of using a bunch of cout-cin pairs).
	{
		cout << "Please enter the ";
			if (i == 0)
				cout << "number of FFT's:  ";
			else
				if (i == 1)
					cout << "SPU's to use:  ";
				else
					if (i == 2)
						cout << "size of the FFT's:  ";
				else
					if (i == 3)
						cout << "hugepage_flag:  ";
		cin >> number[i];
		
		conv[i] << number[i];  //Convert all of the integer parameters into string streams, convert them to strings and add them to the command variable.
	}

	return;
}

void twoDimensionalTransform (int number[], stringstream conv[])
{
	
	for (int i = 0; i < 8; i++)  //Loop will get all of the numeric parameters from the user (instead of using a bunch of cout-cin pairs).
	{
		cout << "Please enter the ";
			if (i == 0)
				cout << "second parameter:  ";
			else
				if (i == 1)
					cout << "third parameter:  ";
				else
					cout << (i+1) << "th parameter:  "; // Works for up to the eighth parameter.					

		cin >> number[i];
		
		conv[i] << number[i]; //Convert all of the integer parameters into string streams, convert them to strings and add them to the command variable.
	}
	
	return;
}



