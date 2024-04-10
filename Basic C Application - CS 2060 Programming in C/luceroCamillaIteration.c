/*
Camilla Lucero
4/1/2023
CS 2060-001 Deb Harding
Iteration

Problem Statement: A business wants to create a program to help organizations raise funds.
An organization can start a fundraiser so others can donate. There is a fee for every credit 
card transaction. Do so by tackling 3 customer problem statements: 1.) Set up a donation
page for an organization, 2.) Allow for incoming donations to be taken, monitored, and stored, and
3.) allow for admins of organizations to access their donation statistics.
*/

//Preprocessor Header Files
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <float.h>
#include <ctype.h>
#include <time.h>

//Constants
#define STRING_SIZE 80
#define DONATION_MINIMUM 0
#define CREDIT_CARD_PROCESSING 3.1 //percent values only
#define URL_BEGIN "https:donate.com/"
#define URL_END "?form=popup#."
#define FILE_PATH_BEGIN "C:\\Classes\\2022-2023\\CS 2060 - Programming In C\\"
#define FILE_PATH_END ".txt"
#define NUMBER_OF_INCORRECT_ADMIN_ATTEMPTS 3


//Structures
//Stores information that is needed to monitor a fundraiser for an organization
typedef struct organization {

	//String Data
	char organizationName[STRING_SIZE]; //stores name of organization
	char purpose[STRING_SIZE]; //stores purpose for creating fundraiser
	char organizerName[STRING_SIZE]; //stores the name of the organizer
	char organizerEmail[STRING_SIZE];//stores organizer's login email
	char organizerPassword[STRING_SIZE];//store organizer login password
	char URL[STRING_SIZE]; //stores URL for organzation's fundrasier once created
	char recieptFilePath[STRING_SIZE]; //stores file path for reciept text file.

	//Numerical Data
	double goalAmount; //stores goal for amount raised
	unsigned int numDonations; //stores total number of donations
	double newestDonation; // stores value for newest donation
	double currentAmount; //stores amount of current donations
	double currentAmountCreditFee; //stores amount of credit processing fees

	//Linked List Self-Referential Pointer
	struct Organization* nextOrgPtr;

	//File pointer for reciept
	FILE* reciept;

}Organization; //end struct 

//Functions
//Customer Modes
void setUpMode(Organization* organizationPtr, size_t stringSize, double donationMin, const char urlBegin[], const char urlEnd[], const char filePathBegin[], const char filePathEnd[]);
bool donationMode(Organization* organizationPtr, size_t stringSize, double donationMin, double creditProcessingPercent, int numberOfIncorrectAttempts);
bool adminMode(const Organization* organizationPtr, size_t stringSize, int numberOfIncorrectAttempts);
void displayMode(const Organization* organizationPtr, FILE* recieptFile, const char displayType[]);

//General Functions
void fgetsRemoveNewLine(char input[], size_t stringSize);
bool getValidNumericalInput(const char input[], double* returnVariable, size_t stringSize, double donationMin);
void getValidZip(double* returnVariable, size_t stringSize);
void concatenateURLAndFilePath(const char orgName[], const char stringBegin[], const char stringEnd[], char returnString[]);
double calculateProcessingFee(double creditProcessing, double input);
char validateYesOrNo();
void insertOrganization(Organization** headPtr, size_t stringSize, double donationMin, const char urlBegin[], const char urlEnd[], const char filePathBegin[], const char filePathEnd[]);
void deleteAllOrgs(Organization** headPtr);
bool determineAlphabeticalOrder(char string1[], char string2[]);
Organization* selectionMode(Organization* headPtr, size_t stringSize);
void displayList(const Organization* headPtr, const char displayType[]);



int main() {

	puts("Welcome to CS 2060 Fundrasing!\n");

	//Customer Mode 1 - Set Up Organizatons
	puts("Beginning Set Up\n----------------------------------------");
	Organization* headOrgPtr = NULL;// head ptr of linked list
	bool moreOrgToAdd = false;// boolean flag for loop below
	
	do {
		moreOrgToAdd = false; // intialize to false after each run
		//Insert org into linked list - create new org, initialize, and connect to list
		insertOrganization(&headOrgPtr,STRING_SIZE, DONATION_MINIMUM, URL_BEGIN,URL_END,FILE_PATH_BEGIN, FILE_PATH_END);
		puts("");

		puts("Would you like to add another organization?");
		char moreOrgComfirmation = validateYesOrNo();

		//determine type of input - if yes, continue asking, if no -exit
		if (moreOrgComfirmation == 'y') {
			moreOrgToAdd  = true;
		}
		puts("");

	} while (moreOrgToAdd);// while user wants to add more orgs
	puts("");
	puts("");


	//Customer Mode 2 & 3 - Donations & Admin Mode To Quit
	puts("Entering Donations Mode\n----------------------------------------");

	bool adminModeQuit = false; //Boolean Flag for if admin logs in a to quit

	do {

		//Find preferred selection
		Organization* userSelection = selectionMode(headOrgPtr, STRING_SIZE);
		puts("");

		//Send to donation mode
		adminModeQuit = donationMode(userSelection,STRING_SIZE,DONATION_MINIMUM,CREDIT_CARD_PROCESSING, NUMBER_OF_INCORRECT_ADMIN_ATTEMPTS);

		//Once sent to quit, display statistics
		if (adminModeQuit) {
			displayMode(headOrgPtr, NULL, "admin statistics");
		}

	} while (!adminModeQuit);

	deleteAllOrgs(&headOrgPtr);//Deletes all organizations in linked list
	return 0;
}//end main



//Gets user input and returns pointer to selected organzation. Parameters - headPtr or organization linked list and standard size of string. returns pointer to organzation struct
Organization* selectionMode(Organization* headPtr, size_t stringSize) {

	Organization* userSelection = NULL;//return variable 
	char tempString[STRING_SIZE] = ""; //temp string for user input
	bool orgFound = false; // boolean flag for do-while


	//While organization is not found...
	do {

		displayMode(headPtr,NULL, "printing org list");
		

		//Intialize for iteration
		Organization* currentOrg = headPtr;

		puts("Enter the Organization you would like to donate to (Case & Space Sensitive) : ");
		fgetsRemoveNewLine(tempString, stringSize);

		//Assuming HeadPtr is not null, continue and find that organzation.
		//While current pointer is not null and organization has not been found
		while (currentOrg != NULL && strcmp(tempString, currentOrg->organizationName) != 0) {

			//iterate through list
			currentOrg = currentOrg->nextOrgPtr;
		}

		//If currernt org is null, then org was not found
		if (currentOrg == NULL) {
			puts("This Organization Was Not Found...\n Returning To Selection.");
		}
		//Else, org was found and exit loop
		else {
			orgFound = true;
			userSelection = currentOrg;
		}

	} while (!orgFound);

	//Return found org
	return userSelection;
}


//Inserts an organization node into a linked list. Parameters- double pointer to headPtr, size of standard string, donation minimum, the beginning of a URL, end of a URL, beginning of a filePath, and end of a filePath Returns Nothing
void insertOrganization(Organization** headPtr, size_t stringSize, double donationMin, const char urlBegin[], const char urlEnd[], const char filePathBegin[], const char filePathEnd[]) {

	Organization* newOrgPtr = malloc(sizeof(Organization)); //allocate new memory on heap for new pet

	if (newOrgPtr != NULL) {

		//Initialize
		setUpMode(newOrgPtr,stringSize,donationMin,urlBegin,urlEnd, filePathBegin, filePathEnd);
		
		//Find Node's place in list.

		//Create new pets for iteration for comparison
		Organization* currentOrg = *headPtr;
		Organization* previousOrg = NULL;
		bool alphabeticalOrder = false;

		if (currentOrg != NULL) {
			alphabeticalOrder = determineAlphabeticalOrder(currentOrg->organizationName, newOrgPtr->organizationName);
		}

		//While not spot found, continue to iterate through list
		while (currentOrg != NULL && !alphabeticalOrder) {
			if (currentOrg != NULL) {
				alphabeticalOrder = determineAlphabeticalOrder(currentOrg->organizationName, newOrgPtr->organizationName);
			}

			previousOrg = currentOrg;
			currentOrg = currentOrg->nextOrgPtr;
		}

		//If previouspet is Null, then newpet is headptr
		if (previousOrg == NULL) {
			*headPtr = newOrgPtr;
		}
		//else Put pet into list at spot
		else
		{
			//Set previous nextpet to new pet
			previousOrg->nextOrgPtr = newOrgPtr;
		}

		//link the rest of the list to new pet
		newOrgPtr->nextOrgPtr = currentOrg;
	}

}//end insertNode


//sets up a fundraiser for an organization. Parameters - pointer to an organization structure, size of largest string, donation minimum, the beginning of a URL, and the end of the URL. Returns nothing
void setUpMode(Organization* organizationPtr, size_t stringSize, double donationMin, const char urlBegin[], const char urlEnd[], const char filePathBegin[], const char filePathEnd[]) {

	//Part 1 - Intialize Organization Values
	puts("Enter The Organization Name:");//Get organizationName
	fgetsRemoveNewLine(organizationPtr->organizationName, stringSize);
	puts("");

	puts("Enter The Purpose for Starting A Fundrasier:");//Get fundraiser purpose
	fgetsRemoveNewLine(organizationPtr->purpose, stringSize);
	puts("");

	puts("Enter Your First & Last Name:");// Get organizer name
	fgetsRemoveNewLine(organizationPtr->organizerName, stringSize);
	puts("");

	bool isValid = false; // Boolean flag for DoWhile Loop
	do {
		char tempInputString[STRING_SIZE] = { "" };// temp string to store input from user

		puts("Enter Your Fundraising Goal:");//Get fundraiser goal amount
		fgetsRemoveNewLine(tempInputString, stringSize);
		isValid = getValidNumericalInput(tempInputString ,&(organizationPtr->goalAmount), stringSize, donationMin);
		puts("");
	} while (!isValid); // while not valid input, repeat asking

	isValid = false;//Sets to false for email check

	//while email is not correct, continue prompting
	do {
		puts("Enter Your Email:");// Get organizer email
		fgetsRemoveNewLine(organizationPtr->organizerEmail, stringSize);
		
		printf("Is this email correct? Enter Y/N or y/n: %s\n", organizationPtr->organizerEmail);//Verify that Email is Correct
		char organizerComfirmation = validateYesOrNo();

		//determine type of input - if yes, print reciept, if no -exit
		if (organizerComfirmation == 'y') {
			isValid = true;
		}

	} while (!isValid);
	puts("");

	puts("Enter Your Password:");// Get organizer password
	fgetsRemoveNewLine(organizationPtr->organizerPassword, stringSize);
	puts("");

	//Initialize Final Values
	organizationPtr->currentAmount = 0;
	organizationPtr->currentAmountCreditFee = 0;
	organizationPtr->numDonations = 0;

	//Part 2 - Create URL & FilePath
	concatenateURLAndFilePath(organizationPtr->organizationName,urlBegin,urlEnd,organizationPtr->URL);//URL
	concatenateURLAndFilePath(organizationPtr->organizationName,filePathBegin, filePathEnd, organizationPtr->recieptFilePath);//FilePath

	//Open File and put into Organization
	FILE* reciept = fopen(organizationPtr->recieptFilePath,"w");

	fprintf(reciept, "%s Customer Reciepts\n----------------------------------------\n", organizationPtr->organizationName);

	organizationPtr->reciept = reciept;

	//Part 3 - Display
	displayMode(organizationPtr,NULL, "set up summary");// Call display mode to display the set up mode summary
}//end setUpMode



//Completes a donation for an organization. Parameters - a pointer to an organization struct, a size_t for string size, a double for the donation minimum, and a double for credit card processing fee percent, returns nothing
bool donationMode(Organization* organizationPtr, size_t stringSize, double donationMin, double creditProcessingPercent, int numberOfIncorrectAttempts) {

	//Admin Mode Boolean Flags
	bool isAdmin = false;// boolean flag for if user is an admin
	bool adminModeSucceed = false;// boolean flag for if user entered the right credentials in admin Mode

	//Display fundraiser information
		displayMode(organizationPtr,NULL, "fundraiser information");

		//Part 1 - Recieve Valid Donation
		//Variables
		double newDonation = 0;// storage for new donation
		double creditProcessingFee = 0;// storage for credit card processing for donation
		double customerZip = 0;// stores current customer's zip
		char customerName[STRING_SIZE] = { "" };// Stores current customer's name
		bool isValid = false;// boolean flag for do while

		//Get Donation Amount until Valid
		do {
			char tempString[STRING_SIZE] = { "" };// temp string to store input from user
			puts("Please enter the amount you would like to donate: ");
			fgetsRemoveNewLine(tempString, stringSize);

			//if q/Q is entered, then assume user is admin
			if (strcmp("q", tempString) == 0 || strcmp("Q", tempString) == 0) {
				isValid = true;
				isAdmin = true;
				puts("");
			}
			else {
				//if not admin, then proceed with donation mode
				isValid = getValidNumericalInput(tempString, &newDonation, stringSize, donationMin);
				puts("");
			}

		} while (!isValid);// Once valid, proceed

		//Set to false for further checks
		isValid = false;

		//If admin, put into admin mode to enter credentials
		if (isAdmin) {
			adminModeSucceed = adminMode(organizationPtr, stringSize, numberOfIncorrectAttempts);

			//If adminMode doesnt succeed, set isAdmin back to false
			if (!adminModeSucceed) {
				puts("---------------------------------------\n Returning to Selecion Mode");
			}

		}
		//If not admin, continue asking information & complete donation
		else {
			puts("Please Enter Your First & Last Name: ");// Get customer first & last name
			fgetsRemoveNewLine(customerName, stringSize);
			puts("");

			puts("Please Enter Your Five Digit Zip Code: ");//Get customer ZIP code
			getValidZip(&customerZip, stringSize);
			puts("");

			creditProcessingFee = calculateProcessingFee(creditProcessingPercent, newDonation);//calculate & prompt about processing fee
			printf("----------------------------------------\nThank you for your donation! There is a %.1lf %% credit card processing fee of $%.2lf. $%.2lf will be donated.\n",
				creditProcessingPercent, creditProcessingFee, newDonation);

			//Now that donation is complete, put donation info into organization
			organizationPtr->newestDonation = newDonation;
			organizationPtr->currentAmount += newDonation;
			organizationPtr->currentAmountCreditFee += creditProcessingFee;
			organizationPtr->numDonations++;

		
			//Request for Receipt
			puts("Would you like a reciept?");
			char customerRecieptChoice = validateYesOrNo();
			puts("");

			//determine type of input - if yes, print reciept, if no -exit
			if (customerRecieptChoice == 'y') {
					isValid = true;
					displayMode(organizationPtr,organizationPtr->reciept, "reciept");
			}
			else if (customerRecieptChoice == 'n') {
					isValid = true;
					puts("Understandable, Have a great day!");
			}

		}
	
	return adminModeSucceed;

}//end donationMode


//Displays a wide range of summary types. Parameters- a pointer to an organization structure and a string holding the displaytype. returns nothing
void displayMode(const Organization* organizationPtr,FILE* recieptFile, const char displayType[]) {

	puts("-------------------------------------------------------------------------------");

	//If displayType is set up summary then display
	if (strcmp("set up summary", displayType) == 0) {
		printf("Thank you %s! The URL to raise funds for %s is %s\n", organizationPtr->organizerName,
			organizationPtr->organizationName, organizationPtr->URL);
	}
	//else if displayType is fundraiser info, display
	else if (strcmp("fundraiser information", displayType) == 0) {

		printf("%s\nMake A Difference By Your Donation!\nOrganization Name: %s\nFundraising Purpose: %s\nWe have raised $%.2lf\n\n",
			organizationPtr->URL,organizationPtr->organizationName, organizationPtr->purpose, organizationPtr->currentAmount);

		//Figure out if goal has been reached, and display corresponding message
		if (organizationPtr->goalAmount <= organizationPtr->currentAmount) {
			//If goal has been met
			puts("We have met our goal, however donations are still appreciated!");
		}
		else {
			//If goal has not been met, display percentage and goal amount
			printf("We are %d%% towards our goal amount of $%.2lf\n",
				(int)((organizationPtr->currentAmount/organizationPtr->goalAmount) * 100), organizationPtr->goalAmount);
		} 
	}
	//else if displayType is receipt, display
	else if (strcmp("reciept", displayType) == 0) {
		puts("Printing Reciept\n----------------------------------------");

		time_t currentTime = 0;// variable to display time
		time(&currentTime);

		printf("Organization Name: %s\nAmount Donated: $%.2lf\nDate of Donation: %s\n",
			organizationPtr->organizationName, organizationPtr->newestDonation, ctime(&currentTime));

		fprintf(recieptFile,"Organization Name: %s\nAmount Donated: $%.2lf\nDate of Donation: %s\n",
			organizationPtr->organizationName, organizationPtr->newestDonation, ctime(&currentTime));
	}
	//else if displayTpe is admin statistics, display
	else if (strcmp("admin statistics", displayType) == 0 ) {
		puts("Printing Admin Statistics\n----------------------------------------");

		displayList(organizationPtr, displayType);
	
	}
	//else if displayType is printing org list, displat
	else if (strcmp("printing org list", displayType) == 0) {
		puts("Organizations Hosted on CS 2060 Fundraising -\n\nOrganization \tGoal Amount\tCurrent Amount Raised");

		displayList(organizationPtr, displayType);
	}

	puts("\n-------------------------------------------------------------------------------\n");

}//end displayMode


void displayList(const Organization* headPtr,const char displayType[]) {

	//Set current org for iteration
	Organization* currentOrg = headPtr;

	if ((strcmp("printing org list", displayType) == 0)) {

		//while not a end of list..
		while (currentOrg != NULL) {
			//print details & iterate
			printf("%s\t$ %.2f\t$ %.2f\n", currentOrg->organizationName, currentOrg->goalAmount, currentOrg->currentAmount);

			currentOrg = currentOrg->nextOrgPtr;
		}

	}
	else if ((strcmp("admin statistics", displayType) == 0)) {

		//while not a end of list..
		while (currentOrg != NULL) {
			//print details & iterate
			printf("Organization Name: %s\nNumber of Donations: %d\nAmount Raised: $%.2lf\nAmount Paid For Credit Card Processing: $%.2lf\n",
				currentOrg->organizationName, currentOrg->numDonations, currentOrg->currentAmount, currentOrg->currentAmountCreditFee);

			puts("");
		
			currentOrg = currentOrg->nextOrgPtr;
		}

	}
}


bool adminMode(const Organization* organizationPtr, size_t stringSize, int numberOfIncorrectAttempts) {

	puts("Entering Admin Mode\n----------------------------------------");

	unsigned int incorrectCredentialCounter = 0;// counter to keep track of how many incorrect attempts were made
	bool correctCredential = false;// boolean flag to store whether a credential was correct or not
	bool returnFlag = false;// boolean flage to be returned. Shows whether function suceeded or not

	//Prompt and verify email
	do {
		char tempString[STRING_SIZE] = { "" };
		puts("Enter Your Email:");// Get organizer email
		fgetsRemoveNewLine(tempString, stringSize);
		puts("");

		//Check if this entered email is equal to one stored.
		if (strcmp(tempString,organizationPtr->organizerEmail) == 0) {
			correctCredential = true;
		}
		//if not, increment counter and repeat
		else {
			puts("Incorrect Credential.");
			incorrectCredentialCounter++;
		}

	} while (!correctCredential && incorrectCredentialCounter < numberOfIncorrectAttempts);// continue repeating until credential is correct and counter is less to 2

	//If incorrect twice, send back to Donations mode
	if (incorrectCredentialCounter >= numberOfIncorrectAttempts) {

		printf("%d incorrect email attempts... Returning to Donations Mode\n\n\n", numberOfIncorrectAttempts);
		returnFlag = false;
	}
	//Else, proceed to check password
	else {
		incorrectCredentialCounter = 0;// set back to zero for further checks
		correctCredential = false; //set back to false for further checks

		//Prompt and verify password
		do {
			char tempString[STRING_SIZE] = { "" };
			puts("Enter Your Password:");// Get organizer password
			fgetsRemoveNewLine(tempString, stringSize);
			puts("");

			//Check if this entered password is equal to one stored.
			if (strcmp(tempString, organizationPtr->organizerPassword) == 0) {
				correctCredential = true;
			}
			//if not, increment counter and repeat
			else {
				puts("Incorrect Credential.");
				incorrectCredentialCounter++;
			}

		} while (!correctCredential && incorrectCredentialCounter < numberOfIncorrectAttempts);// continue repeating until credential is correct and counter is less to 2

		//If incorrect twice send back to Donations Mode
		if (incorrectCredentialCounter >= numberOfIncorrectAttempts) {
			printf("%d incorrect password attempts... Returning to Donations Mode\n\n\n", numberOfIncorrectAttempts);
			returnFlag = false;

		}
		//If correct, send to display mode to display statistics
		else {
			returnFlag = true;
		}

	}

	return returnFlag;

}//end adminMode


//Gets a string from standard input and removes newline from the end. Parameters - character array, size of input. Returns - nothing
void fgetsRemoveNewLine(char input[], size_t stringSize) {

	//Get string
	fgets(input, stringSize, stdin);

	//Find  & remove newline character
	bool removedNewLine = false;
	size_t size = 0;
	
	//While the newline hasn't been removed, keep going until found. Once found, remove it.
	while(!removedNewLine) {
		if (input[size] == '\n') {

			input[size] = '\0';
			input[size + 1]--;
			removedNewLine = true;
		}
		size++;
	}
}//end fgetsRemoveNewLine 


//gets a string from standard input and converts to a double. Parameters - a sring with input, pointer to the return double, size of the largest string, and the donation minimum. Returns boolean on whether succeeded
bool getValidNumericalInput(const char input[], double* returnVariable, size_t stringSize, double donationMin) {

	//Variables
	bool returnBoolean = false;// boolean variable for function return
	char* end; // A pointer to a char named end, end of input string
	errno = 0;// An errno int. Used by the system to indicate a type of error and is defaulted to 0

	//Tests if strtod suceeded and prints error if not
	double test = strtod(input, &end);//double that stores the return from strtod. 

		//Checks if input is not an numerical value.
		if (end == input) {
			printf("Not a decimal number\n");
		}
	//else, checks if there are more inputs past the null character.
		else if ('\0' != *end) {
			printf("Extra characters at end of input\n");
		}
	//else, checks if value is in range of a long
		else if ((DBL_MIN == test || DBL_MAX == test) && ERANGE == errno) {
			printf("Entered Number is too large\n");
		}
	//else, checks if bigger than donation minimum
		else if (test <= donationMin) {
			printf("Entered Number is to small\n");
		}
	//Else, the donation is valid.
		else {
			*returnVariable = test;
			returnBoolean = true;
		}

	return returnBoolean;
}// end getValidNumericalInput


//Gets input from user and verifies that it contains 5 digits. Parameters - unsigned int pointer for return variable, largest string size. Returns boolean.
void getValidZip(double* returnVariable, size_t stringSize) {
	
	bool isValid = false;// boolean flag for dowhile
	double input = 0;//stores user input
	char stringInput[STRING_SIZE] = { "" };

	do {
		fgetsRemoveNewLine(stringInput, stringSize);
		isValid = getValidNumericalInput(stringInput, &input, stringSize, 0);//get valid numerical input for the zip. 0 is hard coded to occupy range check

		//Check if number is valid
		if (isValid == true) {
			isValid = false; //set isValid to false again so it can be used for further checks		
			
			if(input <= 9999 || input >= 100000){
				puts("Your Entered Zip Has Too Many or Not Enough Digits. Please Enter A 5 Digit Zip Code");
			}
			else {
				isValid = true;
				*returnVariable = input;
			}

		}
	} while (!isValid);

}//end getValidZip

//Determines which string is in "greater" alphabetically. Returns a boolean, takes two strings
bool determineAlphabeticalOrder(char string1[], char string2[]) {
	bool returnBool = false;// return varaible

	//Copying and changing each string to lower case for comparison
	size_t string1Size = strlen(string1);
	char string1Copy[STRING_SIZE] = " ";
	strcpy(string1Copy, string1);


	for (size_t i = 0; i < string1Size; i++) {
		string1Copy[i] = tolower(string1Copy[i]);
	}

	size_t string2Size = strlen(string2);
	char string2Copy[STRING_SIZE] = " ";
	strcpy(string2Copy, string2);

	for (size_t i = 0; i < string2Size; i++) {
		string2Copy[i] = tolower(string2Copy[i]);
	}

	//Compare Strings
	//if string 1 is greater than or equal string 2, then 2 is earlier in the alphabet
	if (strcmp(string1Copy, string2Copy) > 0) {
		returnBool = true;
	}

	return returnBool;
}//end determineAlphabeticalOrder


//Calculates a processing fee based on a percent. Parameters- a double for the input, and the credit processing percent, returns a double
double calculateProcessingFee(double creditProcessing, double input) {
	double processingFee = input * (creditProcessing / 100);

	return processingFee;
}//end calculateProcessingFee



//creates a concatenated string of 3 pieces for an organization. Parameters - string of organization's name, the beginning of the string, end of the string, and the variable for the final input to
//to be stored. Returns Nothing
void concatenateURLAndFilePath(const char orgName[], const char stringBegin[], const char stringEnd[], char returnString[]) {

	char orgNameWithDashes[STRING_SIZE];//stores organization name with dashes instead of spaces
	
	//Copy orgname over to new variable for editting
	strcpy(orgNameWithDashes, orgName);

	//Find each dash and replace with space
	for (size_t size = 0; size < strlen(orgNameWithDashes); size++) {
		if (orgNameWithDashes[size] == ' ') {
			orgNameWithDashes[size] = '-';
		}
		else if (isupper(orgNameWithDashes[size]) != 0) {
			orgNameWithDashes[size] = tolower(orgNameWithDashes[size]);
		}
	}

	//Concatenate URL together
	strcpy(returnString,stringBegin);//Copy Beginning into URL string
	strcat(returnString,orgNameWithDashes);//Put organization name after beginning
	strcat(returnString,stringEnd);//put end after organization name

}// end createURL


//Validates whether a y or a n was recieved. No parameters, returns a char. 
char validateYesOrNo() {
	//Validates a yes or no answer. Returns char of y or n 
	
		char validYesNo;
		do {
			puts("Please enter (y)es or (n)o:");
			validYesNo = getchar();
			while (getchar() != '\n');

			validYesNo = tolower(validYesNo);

			if (validYesNo != 'y' && validYesNo != 'n') {
				puts("Not valid input!");

			}

		} while (validYesNo != 'y' && validYesNo != 'n');

		return  validYesNo;
} //End validateYesNo

//Frees all memory from Organization nodes. Parameters - double Org pointer to the headPet in a linked list.
void deleteAllOrgs(Organization** headPtr) {
	Organization* currentPtr = *headPtr;//Org pointer to iterate through list
	Organization* nextPtr = NULL;//Temp Org to handle freeing of memory

	//iterate through and remove pets
	while (currentPtr != NULL) {
		nextPtr = currentPtr->nextOrgPtr;
		fclose(currentPtr->reciept); // also close file
		free(currentPtr);
		currentPtr = nextPtr;
	}

	//free the last org held in tempPet;
	*headPtr = NULL;

}//end deleteAllOrgs