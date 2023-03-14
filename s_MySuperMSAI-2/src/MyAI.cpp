// ======================================================================
// FILE:        MyAI.cpp
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

#include "MyAI.hpp"

MyAI::MyAI ( int _rowDimension, int _colDimension, int _totalMines, int _agentX, int _agentY ) : Agent()
{
    // ======================================================================
    // YOUR CODE BEGINS
    // -2 = covered
    // value = uncovered
    // -1 = flag
    // ======================================================================
    if (_rowDimension != _colDimension)
    {
        isExpert = true;
    }
    else
        isExpert = false;

    if (!isExpert)
    {
        for (int i = 0; i < _rowDimension; ++i)
        {
            vector<int> v;
            for (int j = 0; j < _colDimension; ++j)
            {
                int temp = -2;
                v.push_back(temp);
            }
            board.push_back(v);
        }
    }

    numUncoveredTiles = (_rowDimension * _colDimension) - 1;
    numRows = _rowDimension;
    numCols = _colDimension;
    numMines = _totalMines;
    numFlaggedTiles = 0;
    prev_x = _agentX;
    prev_y = _agentY;
    prev_flag = false;
    
    // ======================================================================
    // YOUR CODE ENDS
    // ======================================================================
};

Agent::Action MyAI::getAction( int number )
{
    // ======================================================================
    // YOUR CODE BEGINS
    // ======================================================================
    int effectiveLabel = -1;
    int label = number;
    int numCoveredTiles = 0;

    covered_Frontier.clear();
    uncovered_Frontier.clear();
    
    //std::cout << "Label = " << label << std::endl;
    if (isExpert)
    {
        return{LEAVE, -1, -1};
    }

    // Assign -1 to board if previous location was flagged
    // Else location_val = label
    if (prev_flag == true)
    {
        board[prev_x][prev_y] = -1;
        prev_flag = false;
    }
    else
    {
        board[prev_x][prev_y] = label;
    }

    // Count number of covered tiles
    for (int i = 0; i < numRows; ++i)
    {
        for (int j = 0; j < numCols; ++j)
        {
            if (board[i][j] == -2)
                ++numCoveredTiles;
        }
    }

    // Check Finish condition
    if (numCoveredTiles == 0)
    {
        return {LEAVE,-1,-1};
    }

    if (toBeUncovered.size() > 0)
    {
        prev_x = toBeUncovered.back().x_val;
        prev_y = toBeUncovered.back().y_val;
        toBeUncovered.pop_back();
        return {UNCOVER, prev_x, prev_y};
    }
                 
    if (toBeFlagged.size() > 0)
    {
        prev_flag = true;
        prev_x = toBeFlagged.back().x_val;
        prev_y = toBeFlagged.back().y_val;
        toBeFlagged.pop_back();
        ++numFlaggedTiles;
        return {FLAG, prev_x, prev_y};
    }

    toBeUncovered.clear();
    toBeFlagged.clear();
    // Check rules of thumb
    for (int i = 0; i < numRows; ++i)
    {
        for (int j = 0; j < numCols; ++j)
        {
            // For all uncovered and unflagged
            if (board[i][j] != -2 && board[i][j] != -1)
            {
                //std::cout << "Found uncovered and unflagged tile at: " << i << ", " << j << std::endl;
                // Generate effecive label
                int markedNeighbors = numMarkedNeighbors(i, j);
                effectiveLabel = board[i][j] - markedNeighbors;
                 //std::cout << "Effective label: " << effectiveLabel << std::endl;
                // If effective label is zero, check if any neighbors are uncovered and unflagged
                if (effectiveLabel == 0 && numCoveredNeighbors(i, j) > 0)
                {
                    uncoveredNeighbor(i, j, false);
                    if (toBeUncovered.size() > 0)
                    {
                        prev_x = toBeUncovered.back().x_val;
                        prev_y = toBeUncovered.back().y_val;
                        toBeUncovered.pop_back();
                        //std::cout << "Uncovering " << prev_x << " , " << prev_y << std::endl;
                        return {UNCOVER, prev_x, prev_y};
                    }
                }
                // If the effective label is the same as the number of unmarked neighbors
                // then they all must be flagged
                else if (effectiveLabel == numCoveredNeighbors(i, j))
                {
                    uncoveredNeighbor(i, j, true);
                    if (toBeFlagged.size() > 0)
                    {
                        prev_flag = true;
                        prev_x = toBeFlagged.back().x_val;
                        prev_y = toBeFlagged.back().y_val;
                        toBeFlagged.pop_back();
                        ++numFlaggedTiles;
                        return {FLAG, prev_x, prev_y};
                    }
                }
            }
        }
    }

    // Build covered and uncovered frontiers
    //std::cout << "Building Frontiers" << std::endl;
    for (int i = 0; i < numRows; ++i)
    {
        for (int j = 0; j < numCols; ++j)
        {
            if (board[i][j] == -2)
            {
                if (covered_Frontier.size() <= 18)
                {
                    int numUncovered = numUncoveredNeighbors(i, j);
                    if (numUncovered > 0)
                    {
                        coords temp;
                        temp.x_val = i;
                        temp.y_val = j;
                        covered_Frontier.push_back(temp);
                    }
                }
            }
            else if (board[i][j] >= 0)
            {
                int numCovered = numCoveredNeighbors(i, j);
                if (numCovered > 0)
                {
                    coords temp;
                    temp.x_val = i;
                    temp.y_val = j;
                    uncovered_Frontier.push_back(temp);
                }
            }
        }
    }
    if (covered_Frontier.size() > 0)
    {
        bruteForceModelChecking();
    }

    if (toBeUncovered.size() > 0)
    {
        prev_x = toBeUncovered.back().x_val;
        prev_y = toBeUncovered.back().y_val;
        toBeUncovered.pop_back();
        return {UNCOVER, prev_x, prev_y};
    }

    if (toBeFlagged.size() > 0)
    {
        prev_flag = true;
        prev_x = toBeFlagged.back().x_val;
        prev_y = toBeFlagged.back().y_val;
        toBeFlagged.pop_back();
        ++numFlaggedTiles;
        return {FLAG, prev_x, prev_y};
    }

/*
    std::cout << "Covered Frontier" << std::endl;
    for (int i  = 0; i < covered_Frontier.size(); ++i)
    {
        std::cout << "X: " << covered_Frontier[i].x_val << " Y: " << covered_Frontier[i].y_val << "   ";
    }
    std::cout << std::endl;

    std::cout << "Uncovered Frontier" << std::endl;
    for (int i  = 0; i < uncovered_Frontier.size(); ++i)
    {
        std::cout << "X: " << uncovered_Frontier[i].x_val << " Y: " << uncovered_Frontier[i].y_val << "   ";
    }
*/
    //std::cout << "Choosing Random Assignment" << std::endl;
    int rand_x_val = rand() % numRows;
    int rand_y_val = rand() % numCols;
    // If nothing is found: generate random coordinates for a covered tile
    while (board[rand_x_val][rand_y_val] != -2)
    {
        rand_x_val = rand() % numRows;
        rand_y_val = rand() % numCols;
    }
    prev_x = rand_x_val;
    prev_y = rand_y_val;
    return {UNCOVER, prev_x, prev_y};

    //return{LEAVE, -1, -1};
    // ======================================================================
    // YOUR CODE ENDS
    // ======================================================================
}


// ======================================================================
// YOUR CODE BEGINS
// ======================================================================
int MyAI::numMarkedNeighbors(const int& xLoc, const int& yLoc)
{
    int numMarkedNeighbors;
    if (isInBounds(xLoc - 1, yLoc - 1))
    {
        if (board[xLoc - 1][yLoc - 1] == -1) // Top Left
            ++numMarkedNeighbors;
    }
    if (isInBounds(xLoc - 1, yLoc))    // Middle Left
    {
        if (board[xLoc - 1][yLoc] == -1)
            ++numMarkedNeighbors;
    }
    if (isInBounds(xLoc - 1, yLoc + 1)) // Bottom Left
    {
        if (board[xLoc - 1][yLoc + 1] == -1)
            ++numMarkedNeighbors;
    }
    if (isInBounds(xLoc, yLoc - 1))    // Above
    {
        if (board[xLoc][yLoc - 1] == -1)
            ++numMarkedNeighbors;
    }
    if (isInBounds(xLoc, yLoc + 1))    // Below
    {
        if (board[xLoc][yLoc + 1] == -1)
            ++numMarkedNeighbors;
    }
    if (isInBounds(xLoc + 1, yLoc - 1)) // Top Right
    {
        if (board[xLoc + 1][yLoc - 1] == -1)
            ++numMarkedNeighbors;
    }
    if (isInBounds(xLoc + 1, yLoc))    // Middle Right
    {
        if (board[xLoc + 1][yLoc] == -1)
            ++numMarkedNeighbors;
    }
    if (isInBounds(xLoc + 1, yLoc + 1)) // Bottom Right
    {
        if (board[xLoc + 1][yLoc + 1] == -1)
            ++numMarkedNeighbors;
    }

    return numMarkedNeighbors;
}

void MyAI::uncoveredNeighbor(const int& xLoc, const int& yLoc, bool flag)
{
    if (isInBounds((xLoc - 1), (yLoc - 1)))
    {
        if (board[xLoc - 1][yLoc - 1] == -2) // Top Left
        {
            coords temp;
            temp.x_val = xLoc - 1;
            temp.y_val = yLoc - 1;
            if (!flag)
                toBeUncovered.push_back(temp);
            else
                toBeFlagged.push_back(temp);
        }
    }
    if (isInBounds((xLoc - 1), yLoc))    // Middle Left
    {
        if (board[xLoc - 1][yLoc] == -2)
        {
            coords temp;
            temp.x_val = xLoc - 1;
            temp.y_val = yLoc;
            if (!flag)
                toBeUncovered.push_back(temp);
            else
                toBeFlagged.push_back(temp);
        }
    }
    if (isInBounds((xLoc - 1), (yLoc + 1))) // Bottom Left
    {
        if (board[xLoc - 1][yLoc + 1] == -2)
        {
            coords temp;
            temp.x_val = xLoc - 1;
            temp.y_val = yLoc + 1;
            if (!flag)
                toBeUncovered.push_back(temp);
            else
                toBeFlagged.push_back(temp);
        }
    }
    if (isInBounds(xLoc, (yLoc - 1)))    // Above
    {
        if (board[xLoc][yLoc - 1] == -2)
        {
            coords temp;
            temp.x_val = xLoc;
            temp.y_val = yLoc - 1;
            if (!flag)
                toBeUncovered.push_back(temp);
            else
                toBeFlagged.push_back(temp);
        }
    }
    if (isInBounds(xLoc, (yLoc + 1)))    // Below
    {
        if (board[xLoc][yLoc + 1] == -2)
        {
            coords temp;
            temp.x_val = xLoc;
            temp.y_val = yLoc + 1;
            if (!flag)
                toBeUncovered.push_back(temp);
            else
                toBeFlagged.push_back(temp);
        }
    }
    if (isInBounds((xLoc + 1), (yLoc - 1))) // Top Right
    {
        if (board[xLoc + 1][yLoc - 1] == -2)
        {
            coords temp;
            temp.x_val = xLoc + 1;
            temp.y_val = yLoc - 1;
            if (!flag)
                toBeUncovered.push_back(temp);
            else
                toBeFlagged.push_back(temp);
        }
    }
    if (isInBounds((xLoc + 1), yLoc))    // Middle Right
    {
        if (board[xLoc + 1][yLoc] == -2)
        {
            coords temp;
            temp.x_val = xLoc + 1;
            temp.y_val = yLoc;
            if (!flag)
                toBeUncovered.push_back(temp);
            else
                toBeFlagged.push_back(temp);
        }
    }
    if (isInBounds((xLoc + 1), (yLoc + 1))) // Bottom Right
    {
        if (board[xLoc + 1][yLoc + 1] == -2)
        {
            coords temp;
            temp.x_val = xLoc + 1;
            temp.y_val = yLoc + 1;
            if (!flag)
                toBeUncovered.push_back(temp);
            else
                toBeFlagged.push_back(temp);
        }
    }
}

int MyAI::numUnmarkedNeighbors(const int& xLoc, const int& yLoc)
{
    int numUnmarkedNeighbors;
    if (isInBounds(xLoc - 1, yLoc - 1))
    {
        if (board[xLoc - 1][yLoc - 1] != -1) // Top Left
            ++numUnmarkedNeighbors;
    }
    if (isInBounds(xLoc - 1, yLoc))    // Middle Left
    {
        if (board[xLoc - 1][yLoc] != -1)
            ++numUnmarkedNeighbors;
    }
    if (isInBounds(xLoc - 1, yLoc + 1)) // Bottom Left
    {
        if (board[xLoc - 1][yLoc + 1] != -1)
            ++numUnmarkedNeighbors;
    }
    if (isInBounds(xLoc, yLoc - 1))    // Above
    {
        if (board[xLoc][yLoc - 1] != -1)
            ++numUnmarkedNeighbors;
    }
    if (isInBounds(xLoc, yLoc + 1))    // Below
    {
        if (board[xLoc][yLoc + 1] != -1)
            ++numUnmarkedNeighbors;
    }
    if (isInBounds(xLoc + 1, yLoc - 1)) // Top Right
    {
        if (board[xLoc + 1][yLoc - 1] != -1)
            ++numUnmarkedNeighbors;
    }
    if (isInBounds(xLoc + 1, yLoc))    // Middle Right
    {
        if (board[xLoc + 1][yLoc] != -1)
            ++numUnmarkedNeighbors;
    }
    if (isInBounds(xLoc + 1, yLoc + 1)) // Bottom Right
    {
        if (board[xLoc + 1][yLoc + 1] != -1)
            ++numUnmarkedNeighbors;
    }
    return numUnmarkedNeighbors;
}

int MyAI::numCoveredNeighbors(const int& xLoc, const int& yLoc)
{
    int numCoveredNeighbors;
    if (isInBounds(xLoc - 1, yLoc - 1))
    {
        if (board[xLoc - 1][yLoc - 1] == -2) // Top Left
            ++numCoveredNeighbors;
    }
    if (isInBounds(xLoc - 1, yLoc))    // Middle Left
    {
        if (board[xLoc - 1][yLoc] == -2)
            ++numCoveredNeighbors;
    }
    if (isInBounds(xLoc - 1, yLoc + 1)) // Bottom Left
    {
        if (board[xLoc - 1][yLoc + 1] == -2)
            ++numCoveredNeighbors;
    }
    if (isInBounds(xLoc, yLoc - 1))    // Above
    {
        if (board[xLoc][yLoc - 1] == -2)
            ++numCoveredNeighbors;
    }
    if (isInBounds(xLoc, yLoc + 1))    // Below
    {
        if (board[xLoc][yLoc + 1] == -2)
            ++numCoveredNeighbors;
    }
    if (isInBounds(xLoc + 1, yLoc - 1)) // Top Right
    {
        if (board[xLoc + 1][yLoc - 1] == -2)
            ++numCoveredNeighbors;
    }
    if (isInBounds(xLoc + 1, yLoc))    // Middle Right
    {
        if (board[xLoc + 1][yLoc] == -2)
            ++numCoveredNeighbors;
    }
    if (isInBounds(xLoc + 1, yLoc + 1)) // Bottom Right
    {
        if (board[xLoc + 1][yLoc + 1] == -2)
            ++numCoveredNeighbors;
    }
    return numCoveredNeighbors;
}

int MyAI::numUncoveredNeighbors(const int& xLoc,const int& yLoc)
{
    int numUncoveredNeighbors;
    if (isInBounds(xLoc - 1, yLoc - 1))
    {
        if (board[xLoc - 1][yLoc - 1] >= 0) // Top Left
            ++numUncoveredNeighbors;
    }
    if (isInBounds(xLoc - 1, yLoc))    // Middle Left
    {
        if (board[xLoc - 1][yLoc] >= 0)
            ++numUncoveredNeighbors;
    }
    if (isInBounds(xLoc - 1, yLoc + 1)) // Bottom Left
    {
        if (board[xLoc - 1][yLoc + 1] >= 0)
            ++numUncoveredNeighbors;
    }
    if (isInBounds(xLoc, yLoc - 1))    // Above
    {
        if (board[xLoc][yLoc - 1] >= 0)
            ++numUncoveredNeighbors;
    }
    if (isInBounds(xLoc, yLoc + 1))    // Below
    {
        if (board[xLoc][yLoc + 1] >= 0)
            ++numUncoveredNeighbors;
    }
    if (isInBounds(xLoc + 1, yLoc - 1)) // Top Right
    {
        if (board[xLoc + 1][yLoc - 1] >= 0)
            ++numUncoveredNeighbors;
    }
    if (isInBounds(xLoc + 1, yLoc))    // Middle Right
    {
        if (board[xLoc + 1][yLoc] >= 0)
            ++numUncoveredNeighbors;
    }
    if (isInBounds(xLoc + 1, yLoc + 1)) // Bottom Right
    {
        if (board[xLoc + 1][yLoc + 1] >= 0)
            ++numUncoveredNeighbors;
    }
    return numUncoveredNeighbors;
}

bool MyAI::isInBounds(const int& x, const int& y)
{
    if (x >= 0 && x < numRows)
    {
        if (y >= 0 && y < numCols)
        {
            return true;
        }
    }
    return false;
}

void MyAI::bruteForceModelChecking()
{
    int numFrontierNodes = covered_Frontier.size();

    vector<vector<int> > combs;

    int control = 1<<numFrontierNodes;

    // std::cout << "Num Combinations: " << control << std::endl;
    // Get all combinations of 0's and 1's
    for (int i = 0; i < control; ++i)
    {
        combs.push_back(allBinary(i, numFrontierNodes));
    }
    // Print all combinations
    /*
    for (int i = 0; i < control; ++i)
    {
        std::cout << "Combination " << i + 1 << ": " << std::endl;
        for (int j = 0; j < numFrontierNodes; ++j)
        {
            std::cout << combs[i][j] << " ";
        }
        std::cout << std::endl;
    }
    */
    // std::cout << "All combinations Found" << std::endl;
    vector<vector<int> > test_board = board;

    vector<float> probability_Safe;
    vector<float> probability_Flag;
    float numValidAssignments = 0.0;

    for (int i = 0; i < numFrontierNodes; ++i)
    {
        probability_Safe.push_back(0.0);
        probability_Flag.push_back(0.0);
    }

    // std::cout << "Allocated safe and flagged vectors" << std::endl;
    
    for (int i = 0; i < control; ++i)
    {
        test_board = board;

        for (int j = 0; j < numFrontierNodes; ++j)
        {
            int test_x = covered_Frontier[j].x_val;
            int test_y = covered_Frontier[j].y_val;
            if (combs[i][j] == 1)
            {
                test_board[test_x][test_y] = -1;
            }
            else
            {
                test_board[test_x][test_y] = -2;
            }
        }

        if (constraintCheck(test_board) == true)
        {
            // std::cout << "Valid Assignment Found" << std::endl;
            numValidAssignments += 1.0;
            for (int k = 0; k < numFrontierNodes; ++k)
            {
                // std::cout << combs[i][k];
                if (combs[i][k] == 0)
                {
                    probability_Safe[k] += 1.0;
                }
                else if (combs[i][k] == 1)
                {
                    probability_Flag[k] += 1.0;
                }
            }
            // std::cout << std::endl;
        }
    }

    // std::cout << "Evaluated all combinations" << std::endl;

    for (int i = 0; i < numFrontierNodes; ++i)
    {
        if (numValidAssignments > 0)
        {
            probability_Safe[i] /= numValidAssignments;
            probability_Flag[i] /= numValidAssignments;
        }
    }

    // std::cout << "Number of valid assignments: " << numValidAssignments << std::endl;
    /*
    // std::cout << "Safe Prob. Vector:" << std::endl;
    for (int i = 0; i < numFrontierNodes; ++i)
    {
        std::cout << probability_Safe[i] << " ";
    }
    // std::cout << std::endl;
    // std::cout << "Flag Prob. Vector:" << std::endl;
    for (int i = 0; i < numFrontierNodes; ++i)
    {
        // std::cout << probability_Flag[i] << " ";
    }
    std::cout << std::endl;
    */
    for (int i = 0; i < numFrontierNodes; ++i)
    {
        coords temp;
        temp.x_val = covered_Frontier[i].x_val;
        temp.y_val = covered_Frontier[i].y_val;

        if (probability_Safe[i] == 1.0)
        {
            toBeUncovered.push_back(temp);
        }
        if (probability_Flag[i] == 1.0)
        {
            toBeFlagged.push_back(temp);
        }
    }

    
    if (toBeUncovered.size() == 0)
    {
        for (int i = 0; i < numFrontierNodes; ++i)
        {
            coords temp;
            temp.x_val = covered_Frontier[i].x_val;
            temp.y_val = covered_Frontier[i].y_val;

            if (probability_Safe[i] >= 0.85)
            {
                toBeUncovered.push_back(temp);
            }
        }
    }
    if (toBeFlagged.size() == 0)
    {
        for (int i = 0; i < numFrontierNodes; ++i)
        {
            coords temp;
            temp.x_val = covered_Frontier[i].x_val;
            temp.y_val = covered_Frontier[i].y_val;

            if (probability_Flag[i] >= 0.85)
            {
                toBeFlagged.push_back(temp);
            }
        }
    }


    if (toBeUncovered.size() == 0)
    {
        for (int i = 0; i < numFrontierNodes; ++i)
        {
            coords temp;
            temp.x_val = covered_Frontier[i].x_val;
            temp.y_val = covered_Frontier[i].y_val;

            if (probability_Safe[i] >= 0.60)
            {
                toBeUncovered.push_back(temp);
            }
        }
    }
    if (toBeFlagged.size() == 0)
    {
        for (int i = 0; i < numFrontierNodes; ++i)
        {
            coords temp;
            temp.x_val = covered_Frontier[i].x_val;
            temp.y_val = covered_Frontier[i].y_val;

            if (probability_Flag[i] >= 0.60)
            {
                toBeFlagged.push_back(temp);
            }
        }
    }
}

vector<int> MyAI::allBinary(int n, int length)
{
    vector<int> temp;
    int bit = 1 << length - 1;
    while (bit) {
        temp.push_back(n & bit ? 1 : 0);
        bit >>= 1;
    }
    return temp;
}

bool MyAI::constraintCheck(const vector<vector<int> >& sample)
{
    for (int i = 0; i < uncovered_Frontier.size(); ++i)
    {
        int _x = uncovered_Frontier[i].x_val;
        int _y = uncovered_Frontier[i].y_val;
        
        int numFlaggedNeigh = 0;

        if (isInBounds(_x - 1, _y - 1))
        {
            if (sample[_x - 1][_y - 1] == -1) // Top Left
                ++numFlaggedNeigh;
        }
        if (isInBounds(_x - 1, _y))    // Middle Left
        {
            if (sample[_x - 1][_y] == -1)
                ++numFlaggedNeigh;
        }
        if (isInBounds(_x - 1, _y + 1)) // Bottom Left
        {
            if (sample[_x - 1][_y + 1] == -1)
                ++numFlaggedNeigh;
        }
        if (isInBounds(_x, _y - 1))    // Above
        {
            if (sample[_x][_y - 1] == -1)
                ++numFlaggedNeigh;
        }
        if (isInBounds(_x, _y + 1))    // Below
        {
            if (sample[_x][_y + 1] == -1)
                ++numFlaggedNeigh;
        }
        if (isInBounds(_x + 1, _y - 1)) // Top Right
        {
            if (sample[_x + 1][_y - 1] == -1)
                ++numFlaggedNeigh;
        }
        if (isInBounds(_x + 1, _y))    // Middle Right
        {
            if (sample[_x + 1][_y] == -1)
                ++numFlaggedNeigh;
        }
        if (isInBounds(_x + 1, _y + 1)) // Bottom Right
        {
            if (sample[_x + 1][_y + 1] == -1)
                ++numFlaggedNeigh;
        }

        int effective_label;

        if (sample[_x][_y] >= 0)
        {
            effective_label = sample[_x][_y] - numFlaggedNeigh;
        }
        else
            effective_label = 0;

        if (effective_label != 0)
        {
            return false;
        }
    }
    return true;
}

// ======================================================================
// YOUR CODE ENDS
// ======================================================================
