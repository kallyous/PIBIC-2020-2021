#include <stdio.h>
#include <vector>
#include <set>
#include <ilcplex/ilocplex.h>

using namespace std;


typedef pair<int, int> edge;


// Lazy Constraint Callback
ILOLAZYCONSTRAINTCALLBACK4(
        EdgesLazyConstraintCallback,
        IloBoolVarArray &, x,
        IloBoolVarArray &, y,
        set<edge> &, edges,
        set<edge> &, no_edges) {

    // Referência do ambiente.
    IloEnv masterEnv = getEnv();

    // Misc.
    IloInt num_x = x.getSize();
    IloInt num_y = y.getSize();
    IloInt i;
    IloInt n = num_x;

    // Arrays onde por valores da solução atual em análise.
    IloBoolArray x_val(masterEnv, num_x);
    IloBoolArray y_val(masterEnv, num_y);

    for (i = 0; i < n; i++) {

        // Extração dos valores dos vértices.
        x_val[i] = getValue(x[i]);
        y_val[i] = getValue(y[i]);

        // (2)  xᵢ + yᵢ <=1  ∀ vᵢ ∈ V
        //      Vértice só pode pertencer a uma parte da biclique.
        if (x_val[i] + y_val[i] > 1) {
            add(x[i] + y[i] <= 1).end();
        }

    }

    // (3)  xᵢ + xⱼ <= 1  e  yᵢ + yⱼ <= 1  ∀ vᵢ,vⱼ ∈ E
    //      Vertices adjacentes não podem pertencer ao mesmo grupo.
    for (auto e: edges) {
        // xᵢ + xⱼ <= 1
        if (x_val[e.first] + x_val[e.second] > 1) {
            add(x[e.first] + x[e.second] <= 1).end();}
        // yᵢ + yⱼ <= 1
        if (y_val[e.first] + y_val[e.second] > 1) {
            add(y[e.first] + y[e.second] <= 1).end();}
    }

    // (4)  xᵢ + yⱼ <= 1  e  yᵢ + xⱼ <= 1  ∀ vᵢ,vⱼ ∉ E
    //      Vértices não-adjacentes pertencem ao mesmo grupo.
    for (auto e: no_edges) {
        // xᵢ + yⱼ <= 1
        if (x_val[e.first] + y_val[e.second] > 1) {
            add(x[e.first] + y[e.second] <= 1).end();}
        // yᵢ + xⱼ <= 1
        if (x_val[e.second] + y_val[e.first] > 1) {
            add(x[e.second] + y[e.first] <= 1).end();}
    }

    return;
}


int main(int argc, char* argv[])
{
    // Definições iniciais.
    IloEnv env;
    IloModel maximumBalancedBicliqueProblem(env, "Maximum Balanced Biclique Problem");
    IloCplex cplex(maximumBalancedBicliqueProblem);

    // Limite de tempo de execução.
    cplex.setParam(IloCplex::Param::TimeLimit, 60);

    // Limite de RAM a utilizar, em megabytes.
    cplex.setParam(IloCplex::Param::MIP::Limits::TreeMemory, 4000);

    // Força Cplex a utilizar todos os meus núcleos.
//    cplex.setParam(IloCplex::Param::Threads, 8);

    // Verifica e ativa parâmetro bound.
    bool hasBound = false;
    int bound;
    if(argc > 1) {
        hasBound = true;
        bound = atoi(argv[1]);
        cout << "Using bound " << bound << "\n";
    }

    // Lê quantidade de vértices/nodes 'n' e quantidade de arestas/edges 'e'.
    int n, e;
    scanf("%d %d", &n, &e);
    cout << "Instance with " << n << " vertices and " << e << " edges\n";

    // Lê os valores dos pesos dos 'n' vértices.
    vector<int> weights(n);
    int w;
    for (int i = 0; i < n; i ++) {
        scanf("%d" , &w);
        weights[i] = w;
    }

    // Lê as informações das 'e' arestas.
    set<edge> edges;
    int u, v;
    for (int i = 0; i < e; i ++)
    {
        scanf("%d %d", &u, &v);
        if(v < u) swap(u,v);
        edges.insert({u-1, v-1}); // Vertices number start at 1
    }

    // TODO: questionar prof Rian sobre esses no_edges.
    set<edge> no_edges;
    for (int i = 0; i < n; i ++)
        for (int j = i+1; j <n; j ++)
            if (! edges.count({i, j}))
                no_edges.insert({i, j});
    cerr << "No edges = " << no_edges.size() << endl;

    // Variáveis de decisão.
    IloBoolVarArray x(env, n), y(env, n);

    // Definições de expressões de soma e pesos.
    IloExpr xSum(env), ySum(env);
    IloExpr weigxSum(env), weigySum(env);

    // Soma e pesos de x.
    for (int i = 0; i < n; i ++) {
        xSum += x[i];
        weigxSum += x[i] * weights[i];
    }

    // Soma e pesos de y.
    for (int i = 0; i < n; i ++) {
        ySum += y[i];
        weigySum += y[i] * weights[i];
    }

    // Função objetivo, MAX(⅀wx + ⅀wy)
    maximumBalancedBicliqueProblem.add(IloMaximize(env, weigxSum + weigySum));

    // Restrição da igualdade da quantidade de vértices nos dois grupos da biclique.
    maximumBalancedBicliqueProblem.add(xSum == ySum);

    // Uso do bound, se definido.
    if(hasBound)
        maximumBalancedBicliqueProblem.add(weigxSum + weigySum >= bound);

    // Aplica callback
    cplex.use( EdgesLazyConstraintCallback(env, x, y, edges, no_edges) );

    // Resolve.
    cplex.solve();

    // Pega e exibe solução.
    printf("MaximumVertices: %.0lf\n", cplex.getObjValue());
    IloNumArray xSolution(env, n), ySolution(env, n);
    cplex.getValues(xSolution, x); cplex.getValues(ySolution, y);
    printf("x:"); for (int i = 0; i < n; i ++) if (xSolution[i]) printf(" %d", i + 1);
    printf("\ny:"); for (int i = 0; i < n; i ++) if (ySolution[i]) printf(" %d", i + 1);
    printf("\n");

    env.end();
    return(0);
}
