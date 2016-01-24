#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <cstring>
#include <cctype>
#include "utilities.h"
using namespace std;

int manageOneRound(const char words[][MAXWORDLEN + 1], int nWords, int wordnum);
bool contains(const char words[][MAXWORDLEN + 1], int nWords, char target[]);

int main() {
	const int MAXWORDS = 9000;
	char secretWord[MAXWORDS][MAXWORDLEN + 1];

	int numRounds; // to be initialized later
	int numWords = loadWords(secretWord, MAXWORDS); // Reads from a text file using the loadWords function into an array of cstrings.
	if (numWords < 1) { // If no words were loaded into the array, the program prints a message and terminates.
		cout << "No words were loaded, so I can't play the game." << endl;
		return 1;
	}

	cout << "How many rounds do you want to play? ";
	cin >> numRounds; // Gets user input as to how many rounds of this game to play.

	cin.ignore(1000, '\n'); // cin with a number takes all of the numbers up to the newline character,
							// so we need to get rid of it to use getline later on.

	if (numRounds < 1) { // If the user doesn't input a positive number of rounds, the program prints a message and terminates.
		cout << "The number of rounds must be positive." << endl;
		return 1;
	}
	
	int i = 1, currentMax, currentMin, totalTries = 0; // totalTries will be used to compute the average
	while (i <= numRounds) {
		cout << endl << "Round " << i << endl;

		// Generates a random number from 0 to numWords - 1, the corresponding word will be the secret word.
		int secretNumber = randInt(0, numWords - 1);
		cout << "The secret word is " << strlen(secretWord[secretNumber]) << " letters long." << endl;

		// Runs through one round of the game and gets the user's score
		int numTries = manageOneRound(secretWord, numWords, secretNumber);
		
		if (numTries > 1)
			cout << "You got it in " << numTries << " tries." << endl;
		else
			cout << "You got it in 1 try." << endl;
		
		// Computing game statistics
		totalTries += numTries;
		if (i == 1) { // currentMax and currentMin should initially be equal to the score of the first round
			currentMax = numTries;
			currentMin = numTries;
		}
		else if (numTries > currentMax) // Later on, if there is a new max or min, the corresponding variables change appropriately. 
			currentMax = numTries;
		else if (numTries < currentMin)
			currentMin = numTries;
		
		// When printing out the average, we want to fixate a decimal point and two digits after the decimal point.
		cout.setf(ios::fixed);
		cout.setf(ios::showpoint);
		cout.precision(2); 
		
		cout << "Average: " << static_cast<double>(totalTries) / i; // i is conveniently the number of rounds up to that point,
		cout << ", minimum: " << currentMin;						// and to compute correctly we cast totalTries as a double.
		cout << ", maximum: " << currentMax << endl;
		
		i++;
	}
}

// Function to simulate one round of the game. It will return the score for that round.
int manageOneRound(const char words[][MAXWORDLEN + 1], int nWords, int wordnum) {
	if (nWords < 1 || wordnum < 0 || wordnum >= nWords) // From our main method this should never ever happen,
		return -1;										// but the project spec says that this must be so.

	int nTries = 1;
	while (true) { // We want to continue trying words until we eventually get the right one and break out of the loop.
		char secretWord[MAXWORDLEN + 1];
		strcpy(secretWord, words[wordnum]); // store a temporary copy of the secret word so we can work with it later on

		cout << "Probe word: ";
		char probeWord[100]; //
		cin.getline(probeWord, 100); // store up to 99 characters and a '\0' into our probe word array

		// We want to deal with all of the cases that the project spec says we should watch out for before counting rocks and pebbles.
		bool isValidProbeWord = true; // The probe word is valid until proven invalid.

		if (strlen(probeWord) < MINWORDLEN || strlen(probeWord) > MAXWORDLEN)
			isValidProbeWord = false; // If the probe word has less than 4 or more than 6 characters, it is invalid.
		for (int i = 0; probeWord[i] != '\0'; i++) {
			if (!islower(probeWord[i])) {
				isValidProbeWord = false; // If the probe word does not consist of entirely lower case letters, it is invalid.
			}
		}

		if (isValidProbeWord) { // If the probe word is considered valid, we must then check if it is contained in our text file.
			if (contains(words, nWords, probeWord) == false) { // We search for the word in the array that was passed in.
				cout << "I don't know that word." << endl;
				continue; // If this word is not a valid probe word or isn't in the text file,
			}			  // we want to move on to the next iteration of the loop.
		}
		else { // If the probe word is invalid, we print out this message instead.
			cout << "Your probe word must be a word of 4 to 6 lower case letters." << endl;
			continue;
		}

		// If the probe word is the secret word, then we return the score and exit the loop.
		if (strcmp(probeWord, secretWord) == 0)
			return nTries;

		// Our basis for computing rocks and pebbles is that once characters have been associated as rocks or pebbles,
		// we want to remove those characters from future consideration by replacing them with the 0 character.
		// The 0 character is rather abitrary, for we could use any character that wasn't a lower case letter.
		int nRocks = 0, nPebbles = 0;
		for (int i = 0; probeWord[i] != '\0'; i++) { // We want to look through each character in the probe word
			for (int j = 0; secretWord[j] != '\0'; j++) { // For each character in the probe word, we want to compare it to each character in the secret word.
				if (probeWord[i] == secretWord[j] && probeWord[i] != '0' && secretWord[j] != '0') { // We only care for characters in the probe word and secret word that match up and 
					if (i == j) {																	// are not already a rock or pebble
						nRocks++;			// If the indices match up, they form a rock!
						probeWord[i] = '0';	 // We must remove these two characters so they can't be part of another
						secretWord[j] = '0'; // rock or pebble.
					}
					else if (j >= strlen(probeWord) || i >= strlen(secretWord) || probeWord[j] != secretWord[j]) {
						nPebbles++;			 // If it's not a rock, then it is a pebble under two conditions.
						probeWord[i] = '0';	 // If the two words do not have the same length, then any characters that do not have a corresponding character in either probeWord or secretWord
						secretWord[j] = '0'; // cannot be rocks. If there is a corresponding character in the other word, then it is only a pebble if it would not otherwise be a rock.
					}	
				}
			}
		}

		cout << "Rocks: " << nRocks << ", Pebbles: " << nPebbles << endl;
		nTries++;
	}
}

// Function to check if a cstring is in an array of cstrings that is passed in.
// This function is really only used to see if a given probe word is in the text file that we initially read from.
bool contains(const char words[][MAXWORDLEN + 1], int nWords, char target[]) {
	for (int i = 0; i < nWords; i++) {
		if (strcmp(words[i], target) == 0)
			return true;
	}

	return false;
}
