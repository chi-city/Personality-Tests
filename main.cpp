// -----------------------------------------------------------------------

// Project 2 - Personality Quiz, main.cpp
//
//  Project Description: Build a personality quiz based
//  on the five factor model, which calculates a score based
//  on the user's answers and, then report a fictional
//  character who the user is most similar to.
//
// Author: Zaid Awaidah
// Date: 1 / 29 / 2022
// Class: CS 251, Spring 2022, UIC

// -----------------------------------------------------------------------

#include <set>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <cassert>
#include "driver.h"
using namespace std;

// -----------------------------------------------------------------------

void openQuesFile(set<Question>& questions);
void userOCEANFactors(string& line, Question& question);
void askQuestions(Question& questions, map<Question, int>& scores);
void displayNumOptions(int& userAnswer);
void loadPeopleInfo(int& testChoice, set<Person>& people);
void peopleOptions(int& tectChoice, set<Person>& people);
void loadPeople(string& fileName, set<Person>& people);
void peopleOCEANFactors(string& line, Person& people);

// -----------------------------------------------------------------------

int main() {
    int numQues = 0;
    int userTestChoices = -1;
    set<Person> people;
    set<Question> questions;
    set<Question> randomQuestions;
    map<Question, int> userAnswers;
    map<char, int> OCEANScores;
    cout << "Welcome to the Personality Quiz!\n\n";

    /* read in question file and
     * store questions in set
    */
    cout << "Choose number of questions: ";
    cin >> numQues;
    openQuesFile(questions);

    /* choose random question,
     * display question and get user answers
    */
    for (int i = 0; i < numQues; i++) {
        Question randomQues = randomQuestionFrom(questions);
        askQuestions(randomQues, userAnswers);
    }

    // calculate scores for user
    OCEANScores = scoresFrom(userAnswers);

    // display user's options for .people files
    while (userTestChoices != 0) {
        loadPeopleInfo(userTestChoices, people);

        // most similar person
        if (userTestChoices != 0) {
            Person similarPerson = {};
            similarPerson = mostSimilarTo(OCEANScores, people);
            cout << "You got " << similarPerson.name << "!\n";
            people.clear();
        }
    }

    cout << "Goodbye!\n";
    return 0;
}

// -----------------------------------------------------------------------

/* openQuesFile
 *
 *  opens the file and stores all the questions
 *  in set, referenced back to caller
 *
*/
void openQuesFile(set<Question>& questions) {
    ifstream inFile;
    inFile.open("questions.txt");
    assert(inFile.fail() == false);

    questions.clear();

    string line = "";
    while (getline(inFile, line, '\n')) {
        Question newQuestion;

        // index of period ('.')
        int idxPeriod = line.find('.');

        // get question
        string quesText = line.substr(0, idxPeriod + 1);
        newQuestion.questionText = quesText;

        // erase line to indx of space before factors
        line.erase(0, idxPeriod + 2);

        // get OCEAN factors
        userOCEANFactors(line, newQuestion);
        questions.insert(newQuestion);
    }
    inFile.close();
}

// -----------------------------------------------------------------------

/* setOceanFactors
 *
 *  openQuesFile helper function
 *
 *  Takes in a string and retrieves ocean factors.
 *  Sets object information
 * 
*/
void userOCEANFactors(string& line, Question& question) {
    for (unsigned int i = 0; i < line.size(); i++) {
        if (line.at(i) == ':') {
            char charFactor = line.at(i - 1);
            string num = line.substr(i + 1, line.find(" "));

            int intFactor = stoi(num);
            question.factors[charFactor] = intFactor;

        } else {
            continue;
        }
    }
    // erase line till newline
    line.erase(0, '\n');
}

// -----------------------------------------------------------------------

/* askQuestions
 *
 *  Displays the questions, and stores the user's answers
 *  and references back the map of the question and
 *  respective answer
 *
*/
void askQuestions(Question& questions, map<Question, int>& scores) {
    cout << "\nHow much do you agree with this statement?\n";
    int answer = 0;
    cout << "\"" << questions.questionText << "\"\n\n";
    displayNumOptions(answer);
    scores.emplace(questions, answer);
}

// -----------------------------------------------------------------------

/* displayNumOptions
 *
 *  askQuestions helper function
 *
 *  displays the user's options for choices,
 *  and references back the answer
*/
void displayNumOptions(int& userAnswer) {
    int input = 0;
    cout << "1. Strongly disagree\n";
    cout << "2. Disagree\n";
    cout << "3. Neutral\n";
    cout << "4. Agree\n";
    cout << "5. Strongly agree\n";
    cout << "\nEnter your answer here (1-5): ";
    cin >> input;

    userAnswer = input;
}

// -----------------------------------------------------------------------

/* loadPeopleInfo
 *
 *  displays the test choices for the user,
 *  calls helper functions to retrieve info,
 *  then references back the set of people
 * 
*/
void loadPeopleInfo(int& testChoice, set<Person>& people) {
    cout << "\n1. BabyAnimals\n";
    cout << "2. Brooklyn99\n";
    cout << "3. Disney\n";
    cout << "4. Hogwarts\n";
    cout << "5. MyersBriggs\n";
    cout << "6. SesameStreet\n";
    cout << "7. StarWars\n";
    cout << "8. Vegetables\n";
    cout << "9. mine\n";
    cout << "0. To end program.\n";
    cout << "\nChoose test number (1-9, or 0 to end): ";
    cin >> testChoice;

    peopleOptions(testChoice, people);
}

// -----------------------------------------------------------------------

/* peopleOptions
 *
 *  loadPeopleInfo helper function
 *
 *  Switch case stmt to determine file name
 * 
*/
void peopleOptions(int& tectChoice, set<Person>& people) {
    string fileName = "";

    // incase user changes choice
    switch (tectChoice) {
    case 1:
        fileName = "BabyAnimals.people";
        loadPeople(fileName, people);
        break;

    case 2:
        fileName = "Brooklyn99.people";
        loadPeople(fileName, people);
        break;

    case 3:
        fileName = "Disney.people";
        loadPeople(fileName, people);
        break;

    case 4:
        fileName = "Hogwarts.people";
        loadPeople(fileName, people);
        break;

    case 5:
        fileName = "MyersBriggs.people";
        loadPeople(fileName, people);
        break;

    case 6:
        fileName = "SesameStreet.people";
        loadPeople(fileName, people);
        break;

    case 7:
        fileName = "StarWars.people";
        loadPeople(fileName, people);
        break;

    case 8:
        fileName = "Vegetables.people";
        loadPeople(fileName, people);
        break;

    case 9:
        fileName = "mine.people";
        loadPeople(fileName, people);
        break;

    case 0:
        return;
    }
}

// -----------------------------------------------------------------------

/* loadPeople
 *
 *  loadPeopleInfo helper function
 *
 *  takes in the fileName, opens it and stores
 *  info appropriately, references back set of
 *  people.
 *  
*/
void loadPeople(string& fileName, set<Person>& people) {
    ifstream inFile;
    inFile.open(fileName);
    assert(inFile.fail() == false);

    string line = "";
    while (getline(inFile, line, '\n')) {
        Person newPerson;

        // index of period ('.')
        int idxPeriod = line.find('.');

        // get name
        string name = line.substr(0, idxPeriod);
        newPerson.name = name;

        // erase line to indx of space before factors
        line.erase(0, idxPeriod + 2);

        // get OCEAN factors
        peopleOCEANFactors(line, newPerson);
        people.insert(newPerson);
    }
    inFile.close();
}

// -----------------------------------------------------------------------

/* peopleOceanFactors
 *
 *  loadPeopleInfo helper function
 *
 *  Takes in a string and retrieves ocean factors.
 *  Sets person's score information
 *
*/
void peopleOCEANFactors(string& line, Person& people) {
    for (unsigned int i = 0; i < line.size(); i++) {
        if (line.at(i) == ':') {
            char charFactor = line.at(i - 1);
            string num = line.substr(i + 1, line.find(" "));

            int intFactor = stoi(num);
            people.scores[charFactor] = intFactor;
        } else if (line.at(i) == ' ') {
            continue;
        } else {
            continue;
        }
    }
    line.erase();
}

// -----------------------------------------------------------------------
