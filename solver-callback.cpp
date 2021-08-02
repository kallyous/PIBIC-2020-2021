#include <stdio.h>
#include <vector>
#include <set>
#include <ilcplex/ilocplex.h>
using namespace std;

// Implementação do callback
ILOLAZYCONSTRAINTCALLBACK2(MaxBalBiProLazyCallback, IloBoolVarArray &, x, IloBoolVarArray &, y){

    // Referência do ambiente.
    IloEnv masterEnv = getEnv();

    // Misc.
    IloInt num_x = x.getSize();
    IloInt num_y = y.getSize();
    IloInt i, j;
    IloInt n = num_x;

	// Arrays onde por valores da solução atual para análise.
	IloBoolArray x_val(masterEnv, num_x);
	IloBoolArray y_val(masterEnv, num_y);

	for (i = 0; i < n; i++) {
	    for (j = 0; j < n; j++) {

	        // Extração dos valores dos vértices.
	        x_val[i] = getValue(x[i]);
	        y_val[j] = getValue(y[j]);

	        // (2)  xᵢ + yᵢ <=1  ∀ vᵢ ∈ V  :  Vértice só pode pertencer a uma parte da biclique.
            if (i == j) {
                if (x_val[i] + y_val[j] > 1) {
                    add(x[i] + y[j] <= 1).end();
                    return;
                }
            }

            // (3), (4)
            else {

                // (3)  xᵢ + xⱼ <= 1  e  yᵢ + yⱼ <= 1  ∀ vᵢ,vⱼ ∈ E  :  Vertices adjacentes não podem pertencer ao mesmo grupo.
                if (x_val[i] + x_val[j] > 1) {
                    add(x[i] + x[j] <= 1).end();
                    return;}
                if (y_val[i] + y_val[j] > 1) {
                    add(y[i] + y[j] <= 1).end();
                    return;}

                // (4)  xᵢ + yⱼ <= 1  e  yᵢ + xⱼ <= 1  ∀ vᵢ,vⱼ ∉ E  :  vértices não-adjacentes pertencem ao mesmo grupo.
                if (x_val[i] + y_val[j] > 1) {
                    add(x[i] + y[j] <= 1).end();
                    return;}
                if (y_val[i] + x_val[j] > 1) {
                    add(y[i] + x[j] <= 1).end();
                    return;}
            }

	    }

		// TODO: Ver como CPLEX usa tolerância e personalizar isso se necessário.
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

    // Limite de RAM a utilizar, em megabytes. ~10GB.
    cplex.setParam(IloCplex::Param::MIP::Limits::TreeMemory, 4000);

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
    set<pair<int, int>> edges;
    int u, v;
    for (int i = 0; i < e; i ++)
    {
        scanf("%d %d", &u, &v);
        if(v < u) swap(u,v);
        edges.insert({u-1, v-1}); // Vertices number start at 1
    }

    // TODO: questionar prof Rian sobre esses no_edges.
    set<pair<int, int>> no_edges;
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
    cplex.use( MaxBalBiProLazyCallback(env, x, y) );

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
