// -----------------------------------------------------------------------

// Project 2 - Personality Quiz, driver.h
//
//  driver.h is used as the processing framwork for
//  the personality quiz.
//
// Author: Zaid Awaidah
// Date: 1 / 29 / 2022
// Class: CS 251, Spring 2022, UIC

// -----------------------------------------------------------------------

#include <math.h>
#include <iomanip>
#include <limits>
#include <string>
#include <map>
#include <set>
#include "myrandom.h"
using namespace std;
constexpr double lowest_double = std::numeric_limits<double>::lowest();

// ------------------------------------------------------------

/* Type: Question
 *
 * Type representing a personality quiz question.
 */
struct Question {
    string questionText;  // Text of the question
    map<char, int> factors;   // Map from factors to +1 or -1
    friend bool operator< (const Question& lhs, const Question& rhs) {
        return lhs.questionText < rhs.questionText;
    }
    friend bool operator== (const Question& lhs, const Question& rhs) {
        return lhs.questionText == rhs.questionText;
    }
    friend bool operator!= (const Question& lhs, const Question& rhs) {
        return lhs.questionText != rhs.questionText;
    }
};

// ------------------------------------------------------------

/* Type: Person
 *
 * Type representing a person, used to represent people when determining
 * who's the closest match to the user.
 * 
 */
struct Person {
    string name;      // Name of the person
    map<char, int> scores;  // Map from factors to +1 or -1
    friend bool operator< (const Person& lhs,   const Person& rhs) {
        return lhs.name < rhs.name;
    }
    friend bool operator== (const Person& lhs, const Person& rhs) {
        return lhs.name == rhs.name;
    }
    friend bool operator!= (const Person& lhs, const Person& rhs) {
        return lhs.name != rhs.name;
    }
};

// ------------------------------------------------------------

/* randomElement
 *
 * This function selects, at random, a Question from the inputted questions set
 * and returns the question.  Note, this function does not remove the randomly
 * selected question from the set.
*/

// ------------------------------------------------------------

Question randomElement(set<Question>& questions) {
    int ind = randomInteger(0, (int)questions.size()-1);
    int i = 0;
    for (auto e : questions) {
        if (i == ind) {
            return e;
        }
        i++;
    }
    return {};
}

// ------------------------------------------------------------

/* randomQuestionFrom
 * 
 * This function chooses a random question from the set, removes it, and then
 * returns it back to the set.  
*/
Question randomQuestionFrom(set<Question>& questions) {
    // throw error if set is empty
    if (questions.size() == 0) {
        throw runtime_error("set is empty.");
    }

    // choose a random question
    Question randomQues = randomElement(questions);
    questions.erase(randomQues);
    return randomQues;
}

// ------------------------------------------------------------

/* scoresFrom
 * 
 *  this function takes in a map of answers to quiz questions and
 *  returns a map of their OCEAN scores.
 *  
 *  Computes score based on user answers:
 *   1: strongly disagree ( +ve x2 weight of factor)
 *   2. disagree (+ve x1 weight single factor)
 *   3. neutral (adds zero) 
 *   4. agree (-ve x1 weight single factor)
 *   5. strongly agree (-ve x2 weight of factor)
 * 
 *   *pattern for multiplier is user answer - 3;
 * 
 */
map<char, int> scoresFrom(map<Question, int>& answers) {
    int factorWeight = 0;
    // represents OCEAN or other factor scores
    map<char, int> factorScores = {};

    // loop thru answers
    for (auto& answer : answers) {
        // pattern*
        factorWeight = answer.second - 3;

        // loop thru answers key (Questions)
        for (auto& factor : answer.first.factors) {
            int score = factorWeight * factor.second;

            // if char is new
            if (factorScores.count(factor.first) == 0) {
                factorScores.emplace(factor.first, score);
            } else {
                // mapping updated score with char
                factorScores.at(factor.first)
                    = factorScores.at(factor.first) + score;
            }
        }
    }
    return factorScores;
}

// ------------------------------------------------------------
/* normalize
 * 
 *  takes an input of raw scores, and returns their normalized version
 *  normalized score is calculated by taking the square root of the sum
 *  of each score's value squared
 *
 *  normalize = ( ('O'^2) + ('C'^2) 
 *      + ('E'^2) + (A^2) + (N^2) ) ^  (1/2) 
 */
map<char, double> normalize(map<char, int>& scores) {
    double oceanScore = 0.0;
    double sumOfScores = 0.0;
    double normalizedScore = 0.0;
    double normalizedUserScore = 0.0;
    map<char, double> normalizedScores;

    // calculate "length of vector"
    for (auto& score : scores) {
        oceanScore = pow(score.second, 2);
        sumOfScores += oceanScore;
    }

    // normalize score & error checking divisor
    normalizedScore = sqrt(sumOfScores);

    if (normalizedScore == 0) {
        throw runtime_error("math error: dividing by zero");
    }

    /* divide each score.(int) then construt
    *  and insert new element in map
    */
    for (auto& score : scores) {
        normalizedUserScore = score.second / normalizedScore;
        normalizedScores.emplace(score.first, normalizedUserScore);
    }

    return normalizedScores;
}

// ------------------------------------------------------------

/* cosimeSimilarityOf
*
*   takes as input two sets of normalized score, 
*   then returns their cosine similarity using
*   the formula
* 
*   similarity = O1O2 + C1C2 + E1E2
*           + A1A2 + N1N2
* 
*/
double cosineSimilarityOf(const map<char, double>& lhs,
                          const map<char, double>& rhs) {
    double cosSimilarity = 0.0;
    double temp1 = 0.0;

    for (auto& factorFirst : lhs) {
        for (auto& factorsSecond : rhs) {
            // if char in map1 is found in both
            if (lhs.count(factorFirst.first) == rhs.count(factorFirst.first)) {
                temp1 = lhs.at(factorFirst.first) * rhs.at(factorFirst.first);
                cosSimilarity += temp1;
                break;
            } else {
                // break if chars are not same
                break;
            }
        }
    }
     return cosSimilarity;
}

// ------------------------------------------------------------

/* mostSimilarTo
*
*   Takes in user's raw OCEAN scors and a Set of fictional people.
*   Returns the Person whose scores have the highest cosine similarity
*   to the user's score.
* 
*/
Person mostSimilarTo(map<char, int>& scores, set<Person>& people) {
    Person similarPerson = {};
    double max = lowest_double;
    double similarityScore = 0.0;
    map<char, double> normalizedPeopleScores = {};
    map<char, double> normalizedUserScores = {};

    // set cannot be empty
    if (people.size() == 0) {
        throw runtime_error("set is empty.");
    }

    // normalize the user scores & error - check
    normalizedUserScores = normalize(scores);
    if (normalizedUserScores.size() == 0) {
        throw runtime_error("set is empty.");
    }

    for (auto person : people) {
        map<char, double> temp = normalize(person.scores);

        // throw error if person's score can't be normalized
        if (temp.size() == 0) {
            throw runtime_error("set is empty.");
        }
        similarityScore = cosineSimilarityOf(temp, normalizedUserScores);

        // find max
        if (max < similarityScore) {
            max = similarityScore;
            similarPerson = person;
        }
    }
    return similarPerson;
}

// ------------------------------------------------------------
