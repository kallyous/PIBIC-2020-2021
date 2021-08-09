#include <stdio.h>
#include <vector>
#include <set>
#include <ilcplex/ilocplex.h>

// Segundo a documentação do CPLEX, o valor default é 1e-6
#define epsilon 1e-3


using namespace std;


typedef pair<int, int> edge;


// Lazy Constraint Callback
ILOBRANCHCALLBACK3(
        BranchByVertexWeightCallback,
        IloBoolVarArray &, x,
        IloBoolVarArray &, y,
        vector<int> &, weights) {

    if ( getBranchType() != BranchOnVariable )
        return;

    // Prepara
    IloNumArray x_val, y_val;
    IntegerFeasibilityArray x_feas, y_feas;

    // Carrega valores
    getValues(x_val, x);
    getFeasibilities(x_feas, x);
    getValues(y_val, y);
    getFeasibilities(y_feas, y);

    // Variáveis para o loop do algoritmo.
    IloInt best_i = -1;  // Se não forem atualizados, todos os x,y são viáveis e a solução atual não deve
    IloInt best_j = -1;  // ramificar, e sim retornar para a função objetiva. São processados separadamente.
    IloInt x_max_rel = 0.0;
    IloInt y_max_rel = 0.0;
    IloInt cols = x.getSize();

    for (IloInt i = 0; i < cols; i++) {

        // Prepara pra ramificar em x[i] caso x[i] seja inviável.
        if (x_feas[i] == Infeasible)
        {
            // Relevância do x[i] atual.
            IloNum xi_rel = weights[i] / abs(0.5 - x_val[i]);

            // Aplica critério de escolha e atualiza os índices.
            if (xi_rel > x_max_rel)
            {
                best_i = i;
                x_max_rel = xi_rel;
            }
        }

        // Prepara pra ramificar em y[i] caso y[i] seja inviável.
        if (y_feas[i] == Infeasible)
        {
            // Relevância do y[i] atual.
            IloNum yi_rel = weights[i] / abs(0.5 - y_val[i]);

            // Aplica critério de escolha e atualiza os índices.
            if (yi_rel > y_max_rel)
            {
                best_j = i;
                y_max_rel = yi_rel;
            }
        }
    }

    // Ramifica x
    if ( best_i >= 0 ) {
        makeBranch(x[best_i], x_val[best_i], IloCplex::BranchUp,   getObjValue());
        makeBranch(x[best_i], x_val[best_i], IloCplex::BranchDown, getObjValue());
    }

    // Ramifica y
    if ( best_j >= 0 ) {
        makeBranch(y[best_j], y_val[best_j], IloCplex::BranchUp,   getObjValue());
        makeBranch(y[best_j], y_val[best_j], IloCplex::BranchDown, getObjValue());
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
    cplex.setParam(IloCplex::Param::MIP::Limits::TreeMemory, 8000);

    // Força Cplex a utilizar todos os meus núcleos.
    cplex.setParam(IloCplex::Param::Threads, 8);

    // Tolerância de integralidade
    //cplex.setParam(IloCplex::Param::MIP::Tolerances::Integrality, 1e-4);

    // Feasibility cut tolerance
    //cplex.setParam(IloCplex::Param::Benders::Tolerances::feasibilitycut, 1e-4);

    // Optimality cut tolerance
    //cplex.setParam(IloCplex::Param::Benders::Tolerances::optimalitycut, 1e-4);

    // Enfatiza solução viável em detrimento de prova
    //cplex.setParam(IloCplex::Param::Emphasis::MIP, CPX_MIPEMPHASIS_FEASIBILITY);

    // Seleciona próximo nó durante backtracing na árvore usando estimativa alternativa
    //cplex.setParam(IloCplex::Param::MIP::Strategy::NodeSelect, CPX_NODESEL_BESTEST_ALT);

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

    // Lê as informações das 'e' arestas
    set<edge> edges;
    int u, v;
    for (int i = 0; i < e; i ++)
    {
        scanf("%d %d", &u, &v);
        if(v < u) swap(u,v);
        edges.insert({u-1, v-1}); // Vertices number start at 1
    }

    // Prof Rian: o conjunto no_edges são as não arestas. Ou seja,
    //            nesse caso i e j não são vizinho no grafo G.
    // Identifica as não-arestas.
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

    // (2)  xᵢ + yᵢ <=1  ∀ vᵢ ∈ V
    //      Vértice só pode pertencer a uma parte da biclique.
    for (int i = 0; i < n; i ++) {
        maximumBalancedBicliqueProblem.add(x[i] + y[i] <= 1); // (5)
    }

    // (3)  xᵢ + xⱼ <= 1  e  yᵢ + yⱼ <= 1  ∀ vᵢ,vⱼ ∈ E
    //      Vertices adjacentes não podem pertencer ao mesmo grupo.
    for (auto edge: edges) {
        maximumBalancedBicliqueProblem.add(x[edge.first] + x[edge.second] <= 1); // (6)
        maximumBalancedBicliqueProblem.add(y[edge.first] + y[edge.second] <= 1); // (7)
    }

    // (4)  xᵢ + yⱼ <= 1  e  yᵢ + xⱼ <= 1  ∀ vᵢ,vⱼ ∉ E
    //      Vértices não-adjacentes pertencem ao mesmo grupo (i.e. não podem estar em grupos diferentes).
    for (auto edge: no_edges) {
        maximumBalancedBicliqueProblem.add(x[edge.first] + y[edge.second] <= 1); // (8)
        maximumBalancedBicliqueProblem.add(x[edge.second] + y[edge.first] <= 1); // (9)
    }

    // Uso do bound, se definido.
    if(hasBound)
        maximumBalancedBicliqueProblem.add(weigxSum + weigySum >= bound);

    // Aplica callback
    cplex.use( BranchByVertexWeightCallback(env, x, y, weights) );

    // Resolve.
    cplex.solve();

    // Pega e exibe solução.
    printf("MaximumVertices: %.0lf\n", cplex.getObjValue());
    IloNumArray xSolution(env, n), ySolution(env, n);
    cplex.getValues(xSolution, x); cplex.getValues(ySolution, y);
    printf("x:"); for (int i = 0; i < n; i ++) if (xSolution[i]) printf(" %d", i + 1);
    printf("\ny:"); for (int i = 0; i < n; i ++) if (ySolution[i]) printf(" %d", i + 1);
    printf("\n");

    // DEBUG
    cout << "\nFeasibility Cut:\n";
    printf("%e\n\n", IloCplex::Param::Benders::Tolerances::feasibilitycut);

    env.end();
    return(0);
}
