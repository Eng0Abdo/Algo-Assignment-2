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

    struct Node {
        int ID;
        int price;
        char color;
        Node* left;
        Node* right;
        Node* parent;

        Node(int id, int p) {
            ID = id;
            price = p;
            color = 'R';
            left = nullptr;
            right = nullptr;
            parent = nullptr;
        }
    };

    Node* root;
    Node* NIL;

    // comparison
    bool lessThan(int p1, int id1, int p2, int id2) {
        if (p1 != p2) return p1 < p2;
        return id1 < id2;
    }

    // Rotations

    void leftRotate(Node* x) {
        Node* y = x->right;
        x->right = y->left;

        if (y->left != NIL) {
            y->left->parent = x;
        }
        y->parent = x->parent;

        if (x->parent == nullptr) {
            root = y;
        }
        else if (x == x->parent->left) {
            x->parent->left = y;
        }
        else {
            x->parent->right = y;
        }
        y->left = x;
        x->parent = y;
    }

    void rightRotate(Node* x) {
        Node* y = x->left;
        x->left = y->right;

        if (y->right != NIL) {
            y->right->parent = x;
        }
        y->parent = x->parent;
        if (x->parent == nullptr) {
            root = y;
        }
        else if (x == x->parent->right) {
            x->parent->right = y;
        }
        else {
            x->parent->left = y;
        }
        y->right = x;
        x->parent = y;
    }

    // Insert

    void fixInsert(Node* x) {
        while (x->parent != nullptr &&
            x->parent->parent != nullptr &&
            x->parent->color == 'R') {

            if (x->parent == x->parent->parent->left) {
                Node* z = x->parent->parent->right;

                if (z->color == 'R') {
                    x->parent->color = 'B';
                    z->color = 'B';
                    x->parent->parent->color = 'R';
                    x = x->parent->parent;
                }
                else {
                    if (x == x->parent->right) {
                        x = x->parent;
                        leftRotate(x);
                    }
                    x->parent->color = 'B';
                    x->parent->parent->color = 'R';
                    rightRotate(x->parent->parent);
                }
            }
            else {
                Node* z = x->parent->parent->left;
                if (z->color == 'R') {
                    x->parent->color = 'B';
                    z->color = 'B';
                    x->parent->parent->color = 'R';
                    x = x->parent->parent;
                }
                else {
                    if (x == x->parent->left) {
                        x = x->parent;
                        rightRotate(x);
                    }
                    x->parent->color = 'B';
                    x->parent->parent->color = 'R';
                    leftRotate(x->parent->parent);
                }
            }
        }
        root->color = 'B';
    }

    Node* search_ID(Node* node, int id) {
        if (node == NIL) {
            return NIL;
        }
        if (node->ID == id) {
            return node;
        }
        Node* leftResult = search_ID(node->left, id);
        if (leftResult != NIL) return leftResult;
        return search_ID(node->right, id);
    }

    // Delete

    Node* minimum(Node* node) {
        while (node->left != NIL) {
            node = node->left;
        }
        return node;
    }

    void transplant(Node* x, Node* y) {
        if (x->parent == nullptr) {
            root = y;
        }
        else if (x == x->parent->left) {
            x->parent->left = y;
        }
        else {
            x->parent->right = y;
        }
        y->parent = x->parent;
    }

    void fixDelete(Node* x) {
        while (x != root && x->color == 'B') {
            if (x == x->parent->left) {
                Node* a = x->parent->right;
                if (a->color == 'R') {
                    a->color = 'B';
                    x->parent->color = 'R';
                    leftRotate(x->parent);
                    a = x->parent->right;
                }
                if (a->left->color == 'B' && a->right->color == 'B') {
                    a->color = 'R';
                    x = x->parent;
                }
                else {
                    if (a->right->color == 'B') {
                        a->left->color = 'B';
                        a->color = 'R';
                        rightRotate(a);
                        a = x->parent->right;
                    }
                    a->color = x->parent->color;
                    x->parent->color = 'B';
                    a->right->color = 'B';
                    leftRotate(x->parent);
                    x = root;
                }
            }
            else {
                Node* a = x->parent->left;
                if (a->color == 'R') {
                    a->color = 'B';
                    x->parent->color = 'R';
                    rightRotate(x->parent);
                    a = x->parent->left;
                }

                if (a->left->color == 'B' && a->right->color == 'B') {
                    a->color = 'R';
                    x = x->parent;
                }
                else {
                    if (a->left->color == 'B') {
                        a->right->color = 'B';
                        a->color = 'R';
                        leftRotate(a);
                        a = x->parent->left;
                    }
                    a->color = x->parent->color;
                    x->parent->color = 'B';
                    a->left->color = 'B';
                    rightRotate(x->parent);
                    x = root;
                }
            }
        }
        x->color = 'B';
    }

    void deleteTree(Node* node) {
        if (node == NIL) return;
        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    }

public:
    ConcreteAuctionTree() {
        NIL = new Node(0, 0);
        NIL->color = 'B';

        NIL->left = NIL;
        NIL->right = NIL;
        NIL->parent = nullptr;

        root = NIL;
    }

    ~ConcreteAuctionTree() {
        deleteTree(root);
        delete NIL;
    }

    void insertItem(int itemID, int price) override {
        Node* node = new Node(itemID, price);
        node->left = node->right = NIL;
        Node* parent = nullptr;
        Node* curr = root;

        while (curr != NIL) {
            parent = curr;
            if (lessThan(price, itemID, curr->price, curr->ID)) {
                curr = curr->left;
            }
            else {
                curr = curr->right;
            }
        }
        node->parent = parent;

        if (parent == nullptr) {
            root = node;
        }
        else if (lessThan(price, itemID, parent->price, parent->ID)) {
            parent->left = node;
        }
        else {
            parent->right = node;
        }

        fixInsert(node);
    }

    void deleteItem(int itemID) override {
        Node* z = search_ID(root, itemID);
        if (z == NIL) return;

        Node* y = z;
        Node* x;
        char originalColor = y->color;

        if (z->left == NIL) {
            x = z->right;
            transplant(z, z->right);
        }
        else if (z->right == NIL) {
            x = z->left;
            transplant(z, z->left);
        }
        else {
            y = minimum(z->right);
            originalColor = y->color;
            x = y->right;

            if (y->parent == z) {
                x->parent = y;
            }
            else {
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }

            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }

        delete z;
        if (originalColor == 'B') fixDelete(x);
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
    vector<vector<int>> DP(items.size() + 1, vector<int>(capacity + 1, 0));

    for (int i = 1; i <= items.size(); i++) {
        int weight = items[i - 1].first;
        int value = items[i - 1].second;

        for (int j = 0; j <= capacity; j++) {
            if (weight <= j) {
                DP[i][j] = (DP[i - 1][j] > DP[i - 1][j - weight] + value ? DP[i - 1][j] : DP[i - 1][j - weight] + value);
            }
            else {
                DP[i][j] = DP[i - 1][j];
            }
        }
    }

    return DP[items.size()][capacity];
}

long long InventorySystem::countStringPossibilities(string s) {
    // TODO: Implement string decoding DP
    // Rules: "uu" can be decoded as "w" or "uu"
    //        "nn" can be decoded as "m" or "nn"
    // Count total possible decodings
    
    const int MOD = 1e9 + 7;
    int n = s.size();

    vector<long long> dp(n + 1, 0);
    dp[0] = 1;

    for (int i = 1; i <= n; i++) {


        dp[i] = dp[i - 1];

        if (i >= 2) {
            if ((s[i - 2] == 'u' && s[i - 1] == 'u') ||
                (s[i - 2] == 'n' && s[i - 1] == 'n')) {

                dp[i] = (dp[i] + dp[i - 2]) % MOD;
            }
        }
    }

    return dp[n];
}

// =========================================================
// PART C: WORLD NAVIGATOR (Graphs)
// =========================================================

bool WorldNavigator::pathExists(int n, vector<vector<int>>& edges, int source, int dest) {
    // TODO: Implement path existence check using BFS or DFS
    // edges are bidirectional

    int maxIndex = max(source, dest);
    for (auto& e : edges) {
        maxIndex = max(maxIndex, max(e[0], e[1]));
    }
    
    if (n <= maxIndex) {
        return false;
    }
    
    vector<vector<int>> graph(n);
    for (auto& e : edges) {
        graph[e[0]].push_back(e[1]);
        graph[e[1]].push_back(e[0]); 
    }
    
    vector<bool> visited(n, false);
    queue<int> q;

    q.push(source);
    visited[source] = true;

    while (!q.empty()) {
        int current = q.front();
        q.pop();

        if (current == dest)
            return true;

        for (int neighbor : graph[current]) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                q.push(neighbor);
            }
        }
    }
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
