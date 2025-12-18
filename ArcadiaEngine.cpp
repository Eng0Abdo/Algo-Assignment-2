// ArcadiaEngine.cpp - STUDENT TEMPLATE
// TODO: Implement all the functions below according to the assignment requirements

#include "ArcadiaEngine.h"
#include <algorithm>
#include <queue>
#include <numeric>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <set>

using namespace std;

// =========================================================
// PART A: DATA STRUCTURES (Concrete Implementations)
// =========================================================

// --- 1. PlayerTable (Double Hashing) ---


class ConcretePlayerTable : public PlayerTable {
    private:
    struct Entry {
        int playerID;
        string name;
    };
    vector<Entry> table;
    vector<bool> occupied;
    
    int h1(int id){
        return id % 101;
    }
    
    int h2(int id){
        return 97 - (id % 97);
    }

public:
    ConcretePlayerTable() : table(101), occupied(101, false){} 


    void insert(int playerID, string name) override {
        for (int i = 0; i < 101; i++)
        {
            int index = (h1(playerID) + i * h2(playerID)) % 101;
            if (!occupied[index])
            {
                table[index].playerID = playerID;
                table[index].name = name;
                occupied[index] = true;
                return;
            }
        }
        cout << "Table is Full";
    }

    string search(int playerID) override {
        for (int i = 0; i < 101; i++)
        {
            int index = (h1(playerID) + i * h2(playerID)) % 101;
            if (occupied[index] && table[index].playerID == playerID) {
                return table[index].name;
            }
        }
        return "";
    }
};

// --- 2. Leaderboard (Skip List) ---

class ConcreteLeaderboard : public Leaderboard {
private:
    struct Node {
        int id;
        int score;
        vector<Node*> next;

        Node(int level, int i, int s) {
            id = i;
            score = s;
            next.resize(level + 1, nullptr);
        }
    };

    int MAX_LEVEL;
    int currentLevel;
    Node* head;

    // random level
    int getRandomLevel() {
        int level = 0;
        while ((rand() % 2) && level < MAX_LEVEL)
            level++;
        return level;
    }

    // comparison function
    bool comesBefore(int id1, int sc1, int id2, int sc2) {
        if (sc1 != sc2)
            return sc1 > sc2;   // score descending
        return id1 < id2;      // id ascending
    }


public:
    ConcreteLeaderboard() {
        MAX_LEVEL = 5;
        currentLevel = 0;
        head = new Node(MAX_LEVEL, -1, 1000000);
    }

    void addScore(int playerID, int score) override {
        vector<Node*> update(MAX_LEVEL + 1);
        Node* curr = head;

        for (int level = currentLevel; level >= 0; level--) {
            while (curr->next[level] &&
                   comesBefore(curr->next[level]->id,
                               curr->next[level]->score,
                               playerID, score)) {
                curr = curr->next[level];
            }
            update[level] = curr;
        }

        int newLevel = getRandomLevel();

        if (newLevel > currentLevel) {
            for (int i = currentLevel + 1; i <= newLevel; i++)
                update[i] = head;
            currentLevel = newLevel;
        }

        Node* newNode = new Node(newLevel, playerID, score);

        for (int i = 0; i <= newLevel; i++) {
            newNode->next[i] = update[i]->next[i];
            update[i]->next[i] = newNode;
        }
    }

    void removePlayer(int playerID) override {
        Node* curr = head;
        Node* target = nullptr;

        while (curr->next[0]) {
            if (curr->next[0]->id == playerID) {
                target = curr->next[0];
                break;
            }
            curr = curr->next[0];
        }

        if (!target) return;

        vector<Node*> update(MAX_LEVEL + 1);
        curr = head;

        for (int level = currentLevel; level >= 0; level--) {
            while (curr->next[level] &&
                   curr->next[level] != target) {
                curr = curr->next[level];
            }
            update[level] = curr;
        }

        for (int i = 0; i <= currentLevel; i++) {
            if (update[i]->next[i] == target)
                update[i]->next[i] = target->next[i];
        }

        delete target;

        while (currentLevel > 0 && head->next[currentLevel] == nullptr)
            currentLevel--;
    }

    vector<int> getTopN(int n) override {
        vector<int> result;
        Node* curr = head->next[0];

        while (curr && n--) {
            result.push_back(curr->id);
            curr = curr->next[0];
        }
        return result;
    }
};

// --- 3. AuctionTree (Red-Black Tree) ---

class ConcreteAuctionTree : public AuctionTree {
private:
    // TODO: Define your Red-Black Tree node structure
    // Hint: Each node needs: id, price, color, left, right, parent pointers

public:
    ConcreteAuctionTree() {
        // TODO: Initialize your Red-Black Tree
    }

    void insertItem(int itemID, int price) override {
        // TODO: Implement Red-Black Tree insertion
        // Remember to maintain RB-Tree properties with rotations and recoloring
    }

    void deleteItem(int itemID) override {
        // TODO: Implement Red-Black Tree deletion
        // This is complex - handle all cases carefully
    }
};

// =========================================================
// PART B: INVENTORY SYSTEM (Dynamic Programming)
// =========================================================

int InventorySystem::optimizeLootSplit(int n, vector<int>& coins) {
    int total = 0;
    for (int c : coins)
        total += c;

    int target = total / 2;

    vector<bool> dp(target + 1, false);
    dp[0] = true;

    for (int coin : coins) {
        for (int s = target; s >= coin; s--) {
            if (dp[s - coin])
                dp[s] = true;
        }
    }
    int bestSplit = 0;
    for (int s = target; s >= 0; s--) {
        if (dp[s]) {
            bestSplit = s;
            break;
        }
    }
    int diff =  total - 2 * bestSplit;

    return diff;
}

int InventorySystem::maximizeCarryValue(int capacity, vector<pair<int, int>>& items) {
    // TODO: Implement 0/1 Knapsack using DP
    // items = {weight, value} pairs
    // Return maximum value achievable within capacity
    return 0;
}

long long InventorySystem::countStringPossibilities(string s) {
    // TODO: Implement string decoding DP
    // Rules: "uu" can be decoded as "w" or "uu"
    //        "nn" can be decoded as "m" or "nn"
    // Count total possible decodings
    return 0;
}

// =========================================================
// PART C: WORLD NAVIGATOR (Graphs)
// =========================================================

bool WorldNavigator::pathExists(int n, vector<vector<int>>& edges, int source, int dest) {
    // TODO: Implement path existence check using BFS or DFS
    // edges are bidirectional
    return false;
}
struct DSU {

    vector<int> parent, rank;

    DSU(int n) {
        parent.resize(n);
        rank.resize(n, 0);
        for (int i = 0; i < n; i++){
            parent[i]=i;
    }

}

    int find(int x) {

        if (parent[x] != x){
            parent[x] = find(parent[x]);
    }
        return parent[x];
}

    bool unite(int x, int y) {
        x = find(x);
        y = find(y);
        if (x == y) return false;

        if (rank[x] < rank[y])
            parent[x] = y;
        else if (rank[x] > rank[y])
            parent[y] = x;
        else {

            parent[y] = x;
            rank[x]++;
        }
        return true;
    }
};
long long WorldNavigator::minBribeCost(
        int n, int m,long long goldRate, long long silverRate,vector<vector<int>>& roadData){

        vector<tuple<long long, int, int>> edges;

        for (auto& r : roadData) {
            int u = r[0];
            int v = r[1];
            long long cost = (long long)r[2]*goldRate + (long long)r[3]*silverRate;
            edges.push_back({cost, u, v});
}

        sort(edges.begin(), edges.end());

        DSU dsu(n);
        long long totalCost = 0;
        int usedEdges = 0;
        for (auto& e : edges) {
            long long cost;
            int u, v;
            tie(cost, u, v) = e;

            if (dsu.unite(u, v)) {
                totalCost += cost;
                usedEdges++;
            }
        }
        if (usedEdges != n - 1)
            return -1;

        return totalCost;
}
string WorldNavigator::sumMinDistancesBinary(int n, vector<vector<int>>& roads) {
    // TODO: Implement All-Pairs Shortest Path (Floyd-Warshall)
    // Sum all shortest distances between unique pairs (i < j)
    // Return the sum as a binary string
    // Hint: Handle large numbers carefully
    const long long INF = 1e18;
    vector<vector<long long>> res(n, vector<long long>(n));
    long long sum = 0;
    for (int f = 0; f < n; f++)
    {
        for (int t = 0; t < n; t++)
        {
            res[f][t] = (f == t) ? 0 : INF;        
        }   
    }
    
    for (vector<int> v:roads){
        int from  = v[0];
        int to  = v[1];
        int weight = v[2];
        res[from][to] = min(res[from][to], (long long)weight);
    }

    for (int k = 0; k < n; k++)
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {   
                if (res[i][k] < INF && res[k][j] < INF){ 
                    res[i][j] = min(res[i][j], res[i][k] + res[k][j]);
                }
            }
        }
    }
    
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {   
            if (res[i][j] < INF){ 
                sum+=res[i][j];
            }
        }
    }

    string binary = "";
    while (sum > 0) {
        binary = to_string(sum % 2) + binary;
        sum = sum / 2;
    }
    
    if (binary!="") return binary;
    
    return "0";
}

// =========================================================
// PART D: SERVER KERNEL (Greedy)
// =========================================================

int ServerKernel::minIntervals(vector<char>& tasks, int n) {

    vector<int> freq(26, 0);

    for (char t : tasks) {
        freq[t - 'A']++;
}

    int maxFreq = 0;
    for (int f : freq) {
        if (f > maxFreq)
            maxFreq = f;
    }

    int countMax = 0;
    for (int f : freq) {
        if (f == maxFreq)
        countMax++;
}

    int totalTasks = tasks.size();
    int intervals = (maxFreq - 1) * (n + 1) + countMax;

    if (intervals < totalTasks)
        return totalTasks;

    return intervals;
}

// =========================================================
// FACTORY FUNCTIONS (Required for Testing)
// =========================================================

extern "C" {
    PlayerTable* createPlayerTable() { 
        return new ConcretePlayerTable(); 
    }

    Leaderboard* createLeaderboard() { 
        return new ConcreteLeaderboard(); 
    }

    AuctionTree* createAuctionTree() { 
        return new ConcreteAuctionTree(); 
    }
}
