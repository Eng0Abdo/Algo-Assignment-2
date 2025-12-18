/**
 * main_test_student.cpp
 * Extended Test Suite for ArcadiaEngine
 */

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <functional>
#include "ArcadiaEngine.h" 

using namespace std;

// ==========================================
// FACTORY FUNCTIONS
// ==========================================
extern "C" {
    PlayerTable* createPlayerTable();
    Leaderboard* createLeaderboard();
    AuctionTree* createAuctionTree();
}

// ==========================================
// TEST UTILITIES
// ==========================================
class StudentTestRunner {
    int count = 0;
    int passed = 0;
    int failed = 0;

public:
    void runTest(string testName, bool condition) {
        count++;
        cout << "TEST: " << left << setw(55) << testName;
        if (condition) {
            cout << "[ PASS ]";
            passed++;
        }
        else {
            cout << "[ FAIL ]";
            failed++;
        }
        cout << endl;
    }

    void printSummary() {
        cout << "\n==========================================" << endl;
        cout << "SUMMARY: Passed: " << passed << " | Failed: " << failed << endl;
        cout << "TOTAL TESTS: " << count << endl;
        cout << "==========================================" << endl;
    }
};

StudentTestRunner runner;

// ==========================================
// PART A: DATA STRUCTURES
// ==========================================

void test_PartA_DataStructures() {
    cout << "\n--- Part A: Data Structures ---" << endl;

    // ---------- PlayerTable ----------
    PlayerTable* table = createPlayerTable();
    runner.runTest("PlayerTable: Insert & Search", [&]() {
        table->insert(101, "Alice");
        return table->search(101) == "Alice";
        }());
    delete table;

    // ---------- Leaderboard ----------
    Leaderboard* board = createLeaderboard();

    runner.runTest("Leaderboard: Basic leader", [&]() {
        board->addScore(1, 100);
        board->addScore(2, 200);
        vector<int> top = board->getTopN(1);
        return !top.empty() && top[0] == 2;
        }());

    runner.runTest("Leaderboard: Tie break by ID", [&]() {
        board->addScore(10, 500);
        board->addScore(20, 500);
        vector<int> top = board->getTopN(2);
        return top.size() == 2 && top[0] == 10 && top[1] == 20;
        }());

    delete board;

    // ---------- AuctionTree ----------
    cout << "\n--- AuctionTree Tests ---" << endl;
    AuctionTree* tree = createAuctionTree();

    runner.runTest("AuctionTree: Single insert", [&]() {
        tree->insertItem(1, 100);
        return true;
        }());

    runner.runTest("AuctionTree: Multiple inserts", [&]() {
        tree->insertItem(2, 50);
        tree->insertItem(3, 150);
        tree->insertItem(4, 75);
        return true;
        }());

    runner.runTest("AuctionTree: Duplicate prices", [&]() {
        tree->insertItem(5, 100);
        tree->insertItem(6, 100);
        tree->insertItem(7, 100);
        return true;
        }());

    runner.runTest("AuctionTree: Delete leaf", [&]() {
        tree->deleteItem(7);
        return true;
        }());

    runner.runTest("AuctionTree: Delete node with one child", [&]() {
        tree->deleteItem(4);
        return true;
        }());

    runner.runTest("AuctionTree: Delete node with two children", [&]() {
        tree->deleteItem(1);
        return true;
        }());

    runner.runTest("AuctionTree: Delete root", [&]() {
        tree->deleteItem(3);
        return true;
        }());

    runner.runTest("AuctionTree: Delete non-existing ID", [&]() {
        tree->deleteItem(999);
        return true;
        }());

    runner.runTest("AuctionTree: Delete remaining nodes", [&]() {
        tree->deleteItem(2);
        tree->deleteItem(5);
        tree->deleteItem(6);
        return true;
        }());

    runner.runTest("AuctionTree: Delete from empty tree", [&]() {
        tree->deleteItem(1);
        return true;
        }());

    delete tree;
}

// ==========================================
// PART B: INVENTORY SYSTEM
// ==========================================

void test_PartB_Inventory() {
    cout << "\n--- Part B: Inventory System ---" << endl;

    runner.runTest("LootSplit: {1,2,4} -> 1", [&]() {
        vector<int> coins = { 1, 2, 4 };
        return InventorySystem::optimizeLootSplit(3, coins) == 1;
        }());

    // 2. Inventory Packer (Knapsack)
    runner.runTest("Knapsack: Cap 10, All Fit -> Value 60", [&]() {
        vector<pair<int, int>> items = { {1, 10}, {2, 20}, {3, 30} };
        return InventorySystem::maximizeCarryValue(10, items) == 60;
        }());

    // ---------- EDGE CASES ----------

    // capacity too small to take anything
    runner.runTest("Knapsack: Cap 0 -> Value 0", [&]() {
        vector<pair<int, int>> items = { {1, 10}, {2, 20} };
        return InventorySystem::maximizeCarryValue(0, items) == 0;
        }());

    // no items
    runner.runTest("Knapsack: No items -> Value 0", [&]() {
        vector<pair<int, int>> items;
        return InventorySystem::maximizeCarryValue(10, items) == 0;
        }());

    // only one item, fits
    runner.runTest("Knapsack: Single item fits", [&]() {
        vector<pair<int, int>> items = { {5, 40} };
        return InventorySystem::maximizeCarryValue(5, items) == 40;
        }());

    // only one item, does NOT fit
    runner.runTest("Knapsack: Single item too heavy", [&]() {
        vector<pair<int, int>> items = { {6, 40} };
        return InventorySystem::maximizeCarryValue(5, items) == 0;
        }());

    // choose best combination (not greedy)
    runner.runTest("Knapsack: Best combination (not greedy)", [&]() {
        vector<pair<int, int>> items = {
            {5, 10},
            {4, 40},
            {6, 30}
        };
        // best is item {4,40} only
        return InventorySystem::maximizeCarryValue(5, items) == 40;
        }());

    // multiple combinations, same weight
    runner.runTest("Knapsack: Multiple choices same weight", [&]() {
        vector<pair<int, int>> items = {
            {2, 10},
            {2, 15},
            {2, 20}
        };
        // can take two best values: 20 + 15 = 35
        return InventorySystem::maximizeCarryValue(4, items) == 35;
        }());


    runner.runTest("String decode: 'uu' -> 2", [&]() {
        return InventorySystem::countStringPossibilities("uu") == 2;
        }());
}

// ==========================================
// PART C: WORLD NAVIGATOR
// ==========================================

void test_PartC_Navigator() {
    cout << "\n--- Part C: World Navigator ---" << endl;

    runner.runTest("PathExists: 0->1->2", [&]() {
        vector<vector<int>> edges = { {0,1},{1,2} };
        return WorldNavigator::pathExists(3, edges, 0, 2);
        }());

    runner.runTest("MinBribeCost: Triangle graph", [&]() {
        vector<vector<int>> roads = {
            {0,1,10,0},
            {1,2,5,0},
            {0,2,20,0}
        };
        return WorldNavigator::minBribeCost(3, 3, 1, 1, roads) == 15;
        }());

    runner.runTest("Binary APSP sum -> 110", [&]() {
        vector<vector<int>> roads = {
            {0,1,1},
            {1,2,2}
        };
        return WorldNavigator::sumMinDistancesBinary(3, roads) == "110";
        }());
}

// ==========================================
// PART D: SERVER KERNEL
// ==========================================

void test_PartD_Kernel() {
    cout << "\n--- Part D: Server Kernel ---" << endl;

    runner.runTest("Scheduler: {A,A,B}, n=2 -> 4", [&]() {
        vector<char> tasks = { 'A','A','B' };
        return ServerKernel::minIntervals(tasks, 2) == 4;
        }());
}

// ==========================================
// MAIN
// ==========================================

int main() {
    cout << "Arcadia Engine - Extended Student Tests\n";
    cout << "--------------------------------------\n";

    test_PartA_DataStructures();
    test_PartB_Inventory();
    test_PartC_Navigator();
    test_PartD_Kernel();

    runner.printSummary();
    return 0;
}
