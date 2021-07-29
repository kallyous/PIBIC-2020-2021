#include <stdio.h>
#include <vector>
#include <set>
#include <ilcplex/ilocplex.h>
using namespace std;


int main(int argc, char* argv[])
{
    IloEnv env;
    IloModel maximumBalancedBicliqueProblem(env, "Maximum Balanced Biclique Problem");
    IloCplex cplex(maximumBalancedBicliqueProblem);

    bool hasBound = false;
    int bound;
    if(argc > 1) {
        hasBound = true;
        bound = atoi(argv[1]);
        cout << "Using bound " << bound << "\n";
    }


    // Statement Data:
    int n, e;
    scanf("%d %d", &n, &e);
    cout << "Instance with " << n << " vertices and " << e << " edges\n";

    vector<int> weights(n);
    int w;
    for (int i = 0; i < n; i ++) {
        scanf("%d" , &w);
        weights[i] = w;
    }


    set<pair<int, int>> edges;
    set<pair<int, int>> no_edges;
    int u, v;
    for (int i = 0; i < e; i ++)
    {
        scanf("%d %d", &u, &v);
        if(v < u) swap(u,v);
        edges.insert({u-1, v-1}); // Vertices number start at 1
    }
    for (int i = 0; i <n; i ++)
        for (int j = i+1; j <n; j ++)
            if (! edges.count({i, j}))
                no_edges.insert({i, j});
            cerr << "No edges = " << no_edges.size() << endl;


            // Decision Variables:
            IloBoolVarArray x(env, n), y(env, n);


            // Restrictions:
            IloExpr xSum(env), ySum(env);
            IloExpr weigxSum(env), weigySum(env);
            for (int i = 0; i < n; i ++) {
                xSum += x[i];
                weigxSum += x[i] * weights[i];
            }

            for (int i = 0; i < n; i ++) {
                ySum += y[i];
                weigySum += y[i] * weights[i];
            }

            // Objective Function:
            maximumBalancedBicliqueProblem.add(IloMaximize(env, weigxSum + weigySum));

            // Balanced:
            maximumBalancedBicliqueProblem.add(xSum == ySum);

            // (5) For each vertex v, v can be in only one part
            for (int i = 0; i < n; i ++) {
                maximumBalancedBicliqueProblem.add(x[i] + y[i] <= 1); // (5)
            }

            // (6) (7) For each edge uv, u and v can't be in the same part
            for (auto edge: edges) {
                maximumBalancedBicliqueProblem.add(x[edge.first] + x[edge.second] <= 1); // (6)
                maximumBalancedBicliqueProblem.add(y[edge.first] + y[edge.second] <= 1); // (7)
            }


            // (8) (9) For each nonedge uv, u and v can't be in diff parts
            for (auto edge: no_edges) {
                maximumBalancedBicliqueProblem.add(x[edge.first] + y[edge.second] <= 1); // (8)
                maximumBalancedBicliqueProblem.add(x[edge.second] + y[edge.first] <= 1); // (9)
            }

            if(hasBound)
                maximumBalancedBicliqueProblem.add(weigxSum + weigySum >= bound);

            // Get Solution:
            cplex.solve();
            printf("MaximumVertices: %.0lf\n", cplex.getObjValue());

            IloNumArray xSolution(env, n), ySolution(env, n);
            cplex.getValues(xSolution, x); cplex.getValues(ySolution, y);

            printf("x:"); for (int i = 0; i < n; i ++) if (xSolution[i]) printf(" %d", i + 1);
            printf("\ny:"); for (int i = 0; i < n; i ++) if (ySolution[i]) printf(" %d", i + 1);
            printf("\n");

            env.end();
            return(0);
}
