// RockPileGameSimulation.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <algorithm>
#include <ctime>
#include <vector>

using namespace std;


class Game {
public:
	/** m -- upper bound on starting number of rocks */
	Game(unsigned int m) : m(m), maxStart(numToStr(m))
	{
		cwins = new bool[m + 1];
		pwins = new bool[m + 1];
		if (cwins == nullptr || cwins == nullptr) {
			run = false;
			cout << "There is not enough memory for the game." << endl;
			delete[] pwins; delete[] cwins;
			run = false;
		}
		// calculate strategy for computer player
		else modified = true;
	}

	~Game()
	{
		delete[] cwins;
		delete[] pwins;
	}

	/** Run the simulation. */
	void Run() {

		if (!run)
			return;

		// for random movements of computer
		srand(time(nullptr));
		
		while (run)
		{
			PrintMenu();
			ReadFromMenu();
			clear_screen();

			if (play)
				PlayGame();
		}

	}

private:

	#pragma region Game simulation

	/** Given vector v, returns x from v
	  * s.t. number of rocks on a pile % x == 0.
	  * if no such x exists, returns 0. */
	unsigned int div(const vector<unsigned int>& v) {
		for (unsigned int x : v) {
			if (rocks % x == 0)
				return x;
		}
		return 0;
	}

	/** Given set v, returns true if there exists x in v,
	* s.t. num is divisible by x (num is 'forbidden' for v)
	* num = 0 is always allowed regardless of v */
	bool isForbidden(unsigned int num, const vector<unsigned int>& v) {
		if (num == 0)
			return false;
		return any_of(v.cbegin(), v.cend(), [num](unsigned int x) { return num % x == 0; });
	}

	/** Calculate strategy for computer player. */
	void strategy() {
		cwins[0] = false;
		pwins[0] = false;
		unsigned int i = 1;
		while (i <= m)
		{
			cwins[i] = (!pwins[i - 1] && !isForbidden(i - 1, cf))
				|| (!pwins[i - 2] && !isForbidden(i - 2, cf))
				|| (!pwins[i - 3] && !isForbidden(i - 3, cf));

			pwins[i] = (!cwins[i - 1] && !isForbidden(i - 1, pf))
				|| (!cwins[i - 2] && !isForbidden(i - 2, pf))
				|| (!cwins[i - 3] && !isForbidden(i - 3, pf));

			++i;
		}
		modified = false;
	}

	/** Game simulation: alternates between player and computer. */
	void PlayGame()
	{
		// does strategy needs updating?
		if (modified)
			strategy();

		play = false;
		bool playersTurn = isFirstPlayer;

		while (rocks > 0) {
			if (playersTurn)
			{
				// has valid move been played?
				if (!PlayerPlays())
					continue;

				// has player won
				if (rocks == 0) {
					cout << "You have won!" << endl;
					enter_query();
				}
				// has player broken forbidden divisibility rule?
				else if (isForbidden(rocks, pf))
				{
					cout << "You have left " << rocks << " rocks on the pile." << endl;
					cout << rocks << " is divisible by " << div(pf) << ", which breaks forbidden divisibility rule." << endl;
					cout << "Computer has won.";
					enter_query();
					rocks = 0;
				}
			}
			else {
				ComputerPlays();
				// has computer won
				if (rocks == 0) {
					cout << "Computer has won." << endl;
					enter_query();
				}
				// has computer broken forbidden divisibility rule?
				else if (isForbidden(rocks, cf))
				{
					cout << "Computer has finished its move with " << rocks << " left." << endl;
					cout << rocks << " is divisible by " << div(cf) << ", which breaks forbidden divisibility rule." << endl;
					cout << "You have won!" << endl;
					enter_query();
					rocks = 0;
				}
			}
			// alternate turns
			playersTurn = !playersTurn;
		}
		clear_screen();
	}

	/** Player's turn
	  * returns: true if player played a valid move in their turn,
	  * false o/w */
	bool PlayerPlays()
	{
		PlayMenu();
		string x; getline(cin, x);

		// is it a regular move
		if (Move(x))
		{
			clear_screen();
			PrintRocksStatus();
			return true;
		}

		// is it a command to quit/restart
		if (Quit(x) || Restart(x))
		{
			rocks = 0;
			return false;
		}

		if (PrintF(x))
		{
			clear_screen();
			return false;
		}

		invalid_input();
		return false;
	}

	/** Computer's turn. Wait for player's confirmation to continue when done. */
	void ComputerPlays()
	{
		unsigned int i = 1;
		vector<unsigned int> availables;
		while (i <= 3 && i <= rocks && pwins[rocks - i] && !isForbidden(rocks - i, cf)) {
			if (!isForbidden(rocks - i, cf))
				availables.push_back(i);
			++i;
		}
		// if losing, randomly play
		if (i > rocks || i > 3) {
			if (availables.size() == 0)
				i = 1;
			else {
				i = ((unsigned int)rand()) % availables.size();
				i = availables[i];
			}
		}
		rocks -= i;

		cout << "  ====== Computer's turn ======" << endl;
		cout << "Computer player takes " << i << " rock" << (i > 1 ? "s" : "") << " from the pile." << endl;
		
		PrintRocksStatus();
	}

	#pragma endregion

	#pragma region UI & Outputs

	// Helping output functions, game menus, etc.

	/** Print menu when playing the game. */
	void PlayMenu() {
		cout << "  ====== Player's turn ======" << endl;
		cout << "It is your turn. Number of remaining rocks: " << rocks << endl;
		cout << "Type:" << endl;
		cout << "\t * positive integer less or equal to " << (3 < rocks ? 3 : rocks) << ";" << endl;
		cout << "\t * \'f\' to see forbidden divisibility rules;" << endl;
		cout << "\t * \'r\' to leave current game;" << endl;
		cout << "\t * \'q\' to quit." << endl;

		cout << endl << "  >  ";
	}

	/** Print main menu. */
	void PrintMenu()
	{
		cout << "  ====== Main menu ======" << endl;
		cout << "You are currently selected to be " << (isFirstPlayer ? "first" : "second") << " player. Type:" << endl;
		cout << "\t * positive number representing starting rocks, to start the game;" << endl;
		cout << "\t\t(Upper bound for starting rocks: " << maxStart << ")" << endl;
		cout << "\t * \'clearPF\' to clear forbidden divisibilities for player;" << endl;
		cout << "\t * \'clearCF\' to clear forbidden divisibilities for computer;" << endl;
		cout << "\t * \'addPF\' to add forbidden divisibilities for player;" << endl;
		cout << "\t * \'addCF\' to add forbidden divisibilities for computer;" << endl;
		cout << "\t * \'f\' to print forbidden divisibilities for both player and computer;" << endl;
		cout << "\t * \'s\' to change to " << (isFirstPlayer ? "second" : "first") << " player;" << endl;
		cout << "\t * \'q\' to quit anytime." << endl;

		cout << endl << "  >  ";
	}

	/** x -- number
	* returns: string representing x */
	string numToStr(unsigned int x) {
		stringstream ss;
		ss << x;
		return ss.str();
	}

	/** Add forbidden divisibilities in container v, corresponding to fw. */
	void AddFW(const string& fw, vector<unsigned int>& v) {
		clear_screen();
		cout << "Add forbidden divisibilities for " << fw << "." << endl;
		cout << "Separate numbers with space:" << endl;

		string input; getline(cin, input);
		vector<string> parsed_nums;

		bool space = true;
		for (char c : input) {
			if (c == ' ')
				space = true;
			else if (space) {
				space = false;
				parsed_nums.push_back(string(1, c));
			}
			else
				parsed_nums[parsed_nums.size() - 1] += c;
		}

		vector<unsigned int> added;
		for (string p : parsed_nums) {
			if (!all_of(p.begin(), p.end(), isdigit))
				continue;
			if (p == "1" || p[0] == '0' || p.size() > maxStart.size()
				|| (p.size() == maxStart.size() && p >= maxStart))
				continue;

			unsigned int num;
			stringstream ss(p);
			ss >> num;

			if (any_of(v.begin(), v.end(), [num](unsigned int x) { return x == num; }))
				continue;
			modified = true;
			v.push_back(num);
			added.push_back(num);
		}

		clear_screen();
		if (added.empty())
			cout << "No new numbers were added to " << fw << "\'s forbidden divisibilities." << endl;
		else {
			cout << "Numbers successfully added to " << fw << "\'s forbidden divisibilities:";
			for (unsigned int a : added)
				cout << " " << a;
			cout << endl;
		}

		cout << "Press ENTER to continue..";
		enter_query();
	}

	/** Prints forbidden divisibilities for fw, stored in v. */
	void PrintFW(const string& fw, const vector<unsigned int>& v) {
		if (v.empty()) {
			cout << "- There are no forbidden divisibilities for " << fw << endl;
		}
		else {
			cout << "- Forbidden divisibilities for " << fw << " are:";
			for (unsigned int x : v)
				cout << " " << x;
			cout << endl;
		}
	}

	/** Print number of rocks currently remaining,
	* and wait for player's confirmation to continue. */
	void PrintRocksStatus()
	{
		if (rocks == 0)
			cout << "No rocks remain.";
		else
			cout << rocks << " rock" << (rocks > 1 ? "s" : "") << " remain"
			<< (rocks == 1 ? "s." : ".");

		cout << endl << "Press ENTER to continue.." << endl;
		enter_query();
		clear_screen();
	}

	/** Clear screen. */
	void clear_screen() {
		cout << flush;
		system("CLS");
	}

	/** Waits for player's permission to continue. */
	void enter_query()
	{
		string x;
		getline(cin, x);
	}

	/** Inform that invalid input has occurred. */
	void invalid_input() {
		cout << "Invalid input. Try again.";
		cout << endl << "Press ENTER to continue..." << endl;
		enter_query();
		clear_screen();
	}

	/** Read from main menu. */
	void ReadFromMenu() {
		string input;
		getline(cin, input);

		if (ClearF(input) || AddF(input) || PrintF(input) || ChangePlayer(input) || Quit(input) || StartingRocks(input))
			return;

		clear_screen();
		invalid_input();
	}

	#pragma endregion

	#pragma region Command parsings

	/** Parses for a valid player move.
	  * returns: true if it is a valid move by player,
	  * false o/w. */
	bool Move(const string& input) {
		if (input.size() == 1 && isdigit(input[0])) {
			unsigned int move = (unsigned int)(input[0] - '0');
			if (0 < move && move <= 3 && move <= rocks) {
				rocks -= move;
				return true;
			}
		}
		return false;
	}

	/** Parses for a command to change who's playing first.
	  * returns: true if it is a command to change who is playing first,
	  * false o/w. */
	bool ChangePlayer(const string& input) {
		if (input == "s" || input == "S") {
			isFirstPlayer = !isFirstPlayer;
			return true;
		}
		return false;
	}

	/** Parses for a command to clear forbidden divisibilities.
	  * returns: true if it is a command for clearing, false o/w. */
	bool ClearF(const string& input) {
		if (input == "clearPF") {
			if (!pf.empty())
			{
				pf.clear();
				modified = true;
			}
			return true;
		}
		if (input == "clearCF") {
			if (!cf.empty())
			{
				cf.clear();
				modified = true;
			}
			return true;
		}
		return false;
	}

	/** Parses for a command to add new forbidden divisibilities.
	  * returns: true if it is a command for adding, false o/w. */
	bool AddF(const string& input) {
		if (input == "addPF") {
			AddFW("player", pf);
			return true;
		}
		if (input == "addCF") {
			AddFW("computer", cf);
			return true;
		}
		return false;
	}

	/** Parses for a command to print forbidden divisibilities.
	  * returns: true if it is a print command, false o/w. */
	bool PrintF(const string& input) {
		if (input == "f" || input == "F") {
			clear_screen();
			PrintFW("player", pf);
			PrintFW("computer", cf);
			cout << "Press ENTER to continue.." << endl;
			enter_query();
			return true;
		}
		return false;
	}

	/** Parses for leave current game command.
	  * returns: true if it is a command to leave current game, 
	  * false o/w. */
	bool Restart(const string& input)
	{
		if (input == "r" || input == "R") {
			return true;
		}
		return false;
	}

	/** Parses for quit the program command.
	  * returns: true if it is a command to quit program,
	  * false o/w. */
	bool Quit(const string& input) {
		if (input == "q" || input == "Q") {
			run = false;
			return true;
		}
		return false;
	}

	/** Parses for starting rocks setup,
	  * and indicates that simulation can start.
	  * returns: true if it is game-start command,
	  * false o/w. */
	bool StartingRocks(const string& input) {
		if (input.size() > 0 && input[0] != '0'
			&& all_of(input.cbegin(), input.cend(), isdigit) &&
			(input.size() < maxStart.size() || input <= maxStart)) {

			stringstream x(input);
			x >> rocks; play = true;
			return true;
		}
		return false;
	}

	#pragma endregion

	#pragma region Variables 
	
	/** Number of rocks currently on the pile. */
	unsigned int rocks = 0;

	/** True if player is playing first, false o/w. */
	bool isFirstPlayer = true;

	/** True if player has not  requested to quit program,
	  * false o/w. */
	bool run = true;

	/** True if game simulation needs to start, false o/w. */
	bool play = false;

	/** True if strategy needs to be updated, o/w false. */
	bool modified = false;

	/** String representing upper bound on starting number of rocks on a pile. */
	const string maxStart;

	/** Upper bound on starting number of rocks on a pile. */
	const unsigned int m;

	/** Array for computer's strategy.
	  * For what numbers should computer be winning. */
	bool *cwins = nullptr;

	/** Array for computer's strategy.
	  * For what number should player be winning. */
	bool *pwins = nullptr;

	/** For all numbers x in pf, player must not end their turn
	  * on a number divisible by x. */
	vector<unsigned int> pf;

	/** For all numbers x in cf, computer must not end their turn
	  * on a number divisible by x. */
	vector<unsigned int> cf;

	#pragma endregion
};

int main()
{
	Game game(100);
	game.Run();
    return 0;
}

