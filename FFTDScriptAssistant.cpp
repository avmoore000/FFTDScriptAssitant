#include "/import/home/u1/uaf/amoore/Working/Headers/standardIncludes.h"
#include "/import/home/u1/uaf/amoore/Working/Headers/sanityChecks.h"
//#include <signal.h>	//Will be used to catch the signal interrupt in order to kill the while loop in fftTransform

/* To Do list 

	1.   Get the signal handling to work so it doesn't go into an infinite loop anymore.
	2.   Implement user specification of script locations
	3.   Design a GUI interface to wrap it in (OpenGL, OSG or QT4)
*/


using namespace std;

void oneDimensionalTransform(int[],stringstream[]);  // Will be called if you are performing one dimensional Fourier Transforms
void twoDimensionalTransform(int[],stringstream[]); // Will be called if you are performing two dimensional Fourier Transforms
void fftTransform(int); // This function will perform the majority of the work with preparing the command line.
void instructions(); // Will display instructions about what the program is asking for.
void mainMenu();	//This will display a menu containing the user options associated with the script.
void testFunction();

/**************************************************Flags for the Sanity Check **************************************************************/

#define MAIN_MENU 0
#define FFT_1D    1
#define FFT_2D    2


int main()  // Starts up the main menu
{
	system("clear");
	
	mainMenu();
	
	return 0;
}

void mainMenu () // Displays the main menu and lets the user decide what to do
{

	string tempChoice = "/0";  // Temporarily holds choice to prevent ugly crashes.
	string fftPath = "/0";  // Will hold the user specified path for the FFT program.
	int choice = 0; // Used for the real choice.
	int sanity = 0;

	while (choice != 4) // This will be the main menu loop
	{

		cout << setw(56) << "Fast Fourier Transform Assistant" << endl << endl;
		cout << "\t1. One dimensional Fast Fourier transforms." << endl;
		cout << "\t2. Two dimensional Fast Fourier transforms." << endl;
		cout << "\t3. Instructions" << endl;
		cout << "\t4. Quit" << endl;
		cout << "\tPlease make a choice (1 - 4):  ";
		cout << "\n\t5. Test Function" << endl;	//Sample function made for testing.
		cin >> tempChoice;

		choice = atoi(tempChoice.c_str());

			switch (choice)
			{
				case 1:  // One dimensional transform
				{
					system("clear");
					cout << "Please enter the path for the one dimensional Fourier Transform code:  ";
					cin >> fftPath;
					sanity = fftSanityCheck(MAIN_MENU,choice,fftPath);	//Make sure the program exists in the specified directory

					if (sanity == 0)  // Everything is cool, no insanity here.
					{
						fftTransform(choice);
					}
				}
				case 2:  // Two dimensional transform
				{
					system("clear");
					cout << "Please enter the path for the two dimensional Fourier Transform code:  ";
					cin >> fftPath;
					sanity = fftSanityCheck(MAIN_MENU,choice,fftPath); // Make sure the program exists in the specified directory
					
					if (sanity == 0)	// Everything is cool, no insanity here.
					{
						fftTransform(choice);	
					}
					break;
				}
				case 3: // Call up the instructions
				{
					system("clear");
					instructions();
					break;
				}
				case 4: //Quit
				{
					system("clear");
					cout << "Thank you for using this assistant.  Have a nice day." << endl << endl;
					break;
				}
				case 5: //Test Function
				{
					cout << "Please enter the path to the HelloWorld program:  ";
					cin >> fftPath;

					sanity = fftSanityCheck(MAIN_MENU,choice,fftPath);

					if (sanity == 0)
						system("./HelloWorld >& test.txt");

					break;
				}
				default: //Invalid choice, display error message.
					cout << "\nThat is not a valid option, please make a choice 1 - 4." << endl << endl;
					cout << "Press Enter to continue.";
					cin.ignore();
					cin.get();
					system("clear");
					break;
			}	
	}

	return;
}

void instructions () //Function Displays instructions about how to use the program.
{

	// General instructions

	cout << "\n\nBefore using this application, be sure that the executable is in the same\n"
		<< "directory as the scripts you are trying to run, or an error will occur." << endl << endl;

	//Instructions for the one dimensional Fourier Transforms

	cout << "\tThe following is an example of the input format: " << endl << endl
		<< "\t\tOne Dimensional Fourier Transforms: \n\n\n"
		<< "\t\t\t ./FFT1D_sample c2c X1 X2 X3 X4 \n\n"
		<< "\t\t\tWhere:\n\t\t\t\t-c2c is the FFT_Type (not neccasarily c2c)\n\t\t\t\t-X1 is the number of FFT's\n"
		<< "\t\t\t\t-X2 is the number of SPU's to use\n\t\t\t\t-X3 is the size of the FFT's\n"
		<< "\t\t\t\t-X4 is the hugepage_flag." << endl << endl << endl;
	
	//Instructions for the two dimensional Fourier Transforms.

	cout << "\t\tTwo Dimensional >&Fourier Transforms: \n\n\n"
		<< "\t\t\t ./FFT2D_sample X1 X2 X3 X4 X5 X6 X7 X8 X9 \n\n"
		<< "\t\t\tWhere:\n\t\t\t\t-X1 is the tile_flag\n\t\t\t\t-X2 is the "
		<< "inplace_flag\n\t\t\t\t-X3 is the log_size_x\n\t\t\t\t-X4"
		<< " is the log_size_y\n\t\t\t\t-X5 is ncycles, how many cycles"
		<< " you wish to run\n\t\t\t\t-X6 is the log2_nspus, the number"
		<< " of SPU's as a log base 2, options are 3 or 4 \n\t\t\t\t"
		<< "-X7 is the numa_flag\n\t\t\t\t-X8 is the largepage_flag"
		<< endl << endl;

	cout << "Press Enter to continue.";
	cin.ignore();
	cin.get();
	

	system("clear");

	return;
}

void fftTransform (int choice)  // Sets up the command that will start the tests.
{
	string charParameter;						// Will hold the character parameter (such as 'c2c')
	string command;							// This string will contain the command line.
	string outputFilePrefix, outFileName;				// These will hold the prefix and the output file name.
	string timeStamp;						// This will let us add a time stamp to the output file.
	string systemMessage;
	
	std::stringstream convert[8];	                                // Array for converting the integers array into strings for use in the command line.

	//char exit;							// Will allow you to nicely exit if the FFTD script is not in the proper directory.

	int numericVariables[8];					// Holds the numeric parameters of the FFT.

	system("clear");

	timeStamp = "date 1>> ";

	cout << "Please enter today's date (no /'s or \'s please!):  ";
	cin >> outputFilePrefix;

	switch (choice)
	{
		case 1:
		{
			cout << "Please enter the type of Fourier Transform to be performed (i.e. c2c):  ";
			cin >> charParameter;

  			oneDimensionalTransform(numericVariables,convert); // Set up the parameters for the one dimensional FFT
			outFileName = "FFT1D-" + outputFilePrefix;  // The name of the file that will be storing the output.
			timeStamp += outFileName + ".txt && ";
			command += timeStamp + "./FFT1D_sample " + charParameter + " ";
			
			for (int i = 0; i < 4; i++) // Add numeric parameters to the command line.
				command += convert[i].str() + " ";
			break;
		}
		case 2:
		{
			twoDimensionalTransform(numericVariables,convert); // Set up the parameters for the two dimensional FFT
			outFileName = "FFT2D-" + outputFilePrefix; // The name of the file that will be storing the output.
			timeStamp += outFileName + ".txt && ";
			command += timeStamp + "./FFT2D_sample " + charParameter + " ";
			
			for (int i = 0; i < 8; i++) // Add numeric parameters to the command line.
				command += convert[i].str() + " ";
			break;
		}
	}

	command += " 2>> " + outFileName + ".txt"; // This will cause the output to be put in the file you have specified.

	/**********************************************************Start of file Header ****************************************************************/
	systemMessage = "printf 'This run began on ' 1>> " + outFileName + ".txt";
	system(systemMessage.c_str());
	systemMessage = "date 1>>" + outFileName + ".txt && printf 'Using the command:\n\n' 1>> " + outFileName + ".txt && printf '\n\n'";
	system(systemMessage.c_str());
	systemMessage = "printf '" + command + "\n\n' 1>> " + outFileName + ".txt";
	system(systemMessage.c_str());

	systemMessage = "printf 'BEGIN'";
	system(systemMessage.c_str());
	/*********************************************************************************************************************************************/

	system("clear");

//	cout << "The current command is:\n\n\t" << command << endl << endl << endl;	//Check to make sure the command was assembled correctly.
	
	while (true) // Run the command in a loop until the program is killed manually (going to fix this eventually)
	{
		//cout << "Is looping" << endl; //Test to make sure we're in the loop.

		system(command.c_str()); //Send the command as a system call.
		sleep(1);	//Pause one second between loops.	
	}	
/****************************************************************End of file Header ******************************************************************/
	systemMessage = "printf 'End Time:'  1>> " + outFileName + ".txt";
	system(systemMessage.c_str());
	systemMessage = "date 1>> " + outFileName + ".txt";
	system(systemMessage.c_str());

	systemMessage = "printf 'END'";
	system(systemMessage.c_str());

/******************************************************************************************************************************************************/

	return;
}

void oneDimensionalTransform(int number[], stringstream conv[])  // Gets the parameters for a one dimensional Fourier Transform.
{
	int temp = 0;
	int valid = 0;

	for (int i = 0; i < 4; i++)  //Loop will get all of the numeric parameters from the user (instead of using a bunch of cout-cin pairs).
	{
		valid = 0;

		while (valid == 0)
		{
			cout << "Please enter the ";

			switch (i)	// Switch on i to get the parameters for the script
			{
				case 0:
				{
					cout << "number of FFT's:  ";
					break;
				}
				case 1:
				{
					cout << "SPU's to use:  ";
					break;
				}
				case 2:
				{
					cout << "size of the FFT's:  ";
					break;
				}
				case 3:
				{
					cout <<"hugepage_flag:  ";
					break;
				}
				default:
				{
					cout << "ERROR! Loop index out of range!";
					break;
				}
			}
			valid = fftSanityCheck(FFT_1D,0,""); //Check to make sure the input is an integer.
		}
		number[i] = valid;
		conv[i] << number[i];  //Convert all of the integer parameters into string streams.
	}

	return;
}

void twoDimensionalTransform (int number[], stringstream conv[])  //Gets the parameters for a two dimensional Fourier Transform.
{
	int valid = 0;

	for (int i = 0; i < 8; i++)  //Loop will get all of the numeric parameters from the user (instead of using a bunch of cout-cin pairs).
	{
		valid = 0;

		while (valid == 0)
		{
			cout << "Please enter the ";
			
			switch (i) //Switch on i to get the parameters for the script
			{
				case 0:
				{
					cout << "tile_flag:  ";
					break;
				}
				case 1:
				{
					cout << "inplace_flag:  ";
					break;
				}
				case 2:
				{
					cout << "log_size_x:  ";
					break;
				}
				case 3:
				{
					cout << "log_size_y:  ";
					break;
				}
				case 4:
				{
					cout << "ncycles:  ";
					break;
				}
				case 5:
				{
					cout << "log2_nspus:  ";
					break;
				}
				case 6:
				{
					cout << "numa_flag:  ";
					break;
				}
				case 7:
				{
					cout << "largepage_flag:  ";
					break;
				}
				default:
				{
					cout << "ERROR! Loop index out of range!";
					break;
				}
			}

			valid = fftSanityCheck(FFT_2D,0,"");	//Check to make sure the input is an integer.
		}					
		
		number[i] = valid;
		
		conv[i] << number[i]; //Convert all of the integer parameters into string streams, convert them to strings and add them to the command variable.
	}
	
	return;
}



