// ======================================================================
// FILE:        MyAI.hpp
//
// AUTHOR:      Jian Li
//
// DESCRIPTION: This file contains your agent class, which you will
//              implement. You are responsible for implementing the
//              'getAction' function and any helper methods you feel you
//              need.
//
// NOTES:       - If you are having trouble understanding how the shell
//                works, look at the other parts of the code, as well as
//                the documentation.
//
//              - You are only allowed to make changes to this portion of
//                the code. Any changes to other portions of the code will
//                be lost when the tournament runs your code.
// ======================================================================

#ifndef MINE_SWEEPER_CPP_SHELL_MYAI_HPP
#define MINE_SWEEPER_CPP_SHELL_MYAI_HPP

#include "Agent.hpp"
#include <iostream> // temporary use
#include <cstdlib>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <cmath>

using namespace std;

class MyAI : public Agent
{
private:

    struct coords
    {
        int x_val;
        int y_val;
    };

    bool isExpert;
    vector<vector<int> > board;
    vector<coords> covered_Frontier;
    vector<coords> uncovered_Frontier;
    vector<coords> toBeUncovered;
    vector<coords> toBeFlagged;
    int numUncoveredTiles;
    int numFlaggedTiles;
    int numMines;
    int numRows;
    int numCols;
    int prev_x;
    int prev_y;
    bool prev_flag;

public:
    MyAI ( int _rowDimension, int _colDimension, int _totalMines, int _agentX, int _agentY );

    Action getAction ( int number ) override;

    // ======================================================================
    // YOUR CODE BEGINS
    // ======================================================================

    int numMarkedNeighbors(const int& xLoc, const int& yLoc);

    int numUnmarkedNeighbors(const int& xLoc,const int& yLoc);

    int numCoveredNeighbors(const int& xLoc, const int& yLoc);

    int numUncoveredNeighbors(const int& xLoc, const int& yLoc);

    void uncoveredNeighbor(const int& xLoc, const int& yLoc, bool flags); 

    bool isInBounds(const int& x, const int& y);

    void bruteForceModelChecking();

    vector<int> allBinary(int n, int length);

    bool constraintCheck(const vector<vector<int> >& sample);

    // ======================================================================
    // YOUR CODE ENDS
    // ======================================================================
};

#endif //MINE_SWEEPER_CPP_SHELL_MYAI_HPP
