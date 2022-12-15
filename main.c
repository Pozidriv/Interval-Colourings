#include <bits/stdc++.h>
using namespace std;

int debug = 0;
// Class of bipartite, biregular graphs
class bp_graph {
  public:
    int a,b; // Sizes of parts A and B, respectively
    int d_a, d_b; // degrees
    vector<vector<int>> A; // edges (indexed first by A then B)

    void print_graph() { // Prints size and edges of graph
      cout << "a: " << a << ", b: " << b << "\n";
      cout << "Edges (A to B)" << "\n";
      int count=0;
      for (auto i=A.begin(); i<A.end(); i++,count++) {
        cout << count << ": ";
        for (auto j=i->begin(); j<i->end(); j++) {
          cout << *j;
          if (j+1 != i->end()) cout << ", ";
        }
        cout << "\n";
      }
    }

    bp_graph() { // Basic constructor
    }

    void set_param(int x, int y, vector<vector<int>> B) {
      a = x;
      b = y;
      A = B;
    }

    // Generate a (semi) random biregular graph of indicated parameters (|A|=n)
    bp_graph(int n, int m, int deg_a, int deg_b) { 
      cout << "[bp_graph] Creating random biregular graph (" << n << "," << m << ")" << endl;
      if (n*deg_a != m*deg_b) {
        cerr << "[bireg_rand] Invalid parameters. Exiting...\n";
        exit(-1);
      }
      a=n;
      b=m;
      d_a=deg_a;
      d_b=deg_b;
      int count = 0;
      bool valid = false;
      while (!valid) { // Generating edges (~ configuration model)
        vector<vector<int>> temp_A;
        vector<int> edges;
        bool fail=false;
        for (int i=0; i<deg_a*n; i++) {
          edges.push_back(i);
        }
        random_shuffle(edges.begin(), edges.end()); // Shuffling edge stubs
        for (int i=0; i<n; i++) {
          vector<int> incidences;
          for (int j=0; j<deg_a; j++) {
            int index = edges[i*deg_a+j]/deg_b;
            //cout << edges[i*deg_a+j]<< "," <<edges[i*deg_a+j]/deg_b << "|";
            incidences.push_back(index);
          }
          sort(incidences.begin(), incidences.end());
          //for (int iii=0; iii<incidences.size(); iii++) cout << incidences[iii] << "-";
          if (adjacent_find(incidences.begin(), incidences.end()) != incidences.end()) {
            fail=true;
            //cout << "FAIL" << endl;
            break; // Process failed if there are multi-edges, exit loop
          }
          //cout << ".";
          temp_A.push_back(incidences); // Process successful so far, continue loop
        }
        if (fail) {
          //cout << "[bp_graph] Attempt failed, trying again" << endl;
          //cout << endl;
          count++;
          continue;
        } else {
          valid = true;
          //cout << "Writing edges" << endl;
          for (auto i=temp_A.begin(); i<temp_A.end(); i++) {
            A.push_back(*i);
            //cout << endl;
          }
        }
      }
    cout << "[bp_graph] Total attempts: " << count << endl;
    }

    // checks if the graph is interval colourable with k colours
    bool check_int_col(int k) {

      vector<vector<int>> colouring, B_cols(b);
      vector<pair<int,int>> ranges(b);
      fill(ranges.begin(), ranges.end(), make_pair(0, k));
      return this->run_through_vertex(0, k, colouring, ranges, B_cols);
      // initialize ranges to 0, k
    }

    // runs through vertex number no (in A), using the partial colouring for vertices smaller than no
    // no: vertex in A being considered
    // colouring: partial colouring for edges incident to vertices smaller than no
    // range: admissible ranges for vertices in B (min,max)
    // B_cols: vector of colours for B
    bool run_through_vertex(int no, int k, vector<vector<int>> colouring, vector<pair<int,int>> ranges, vector<vector<int>> B_cols) {
      //cout<<"[run_through_vertex] no " << no << endl;
      if (debug) cout << "(" << no << ")" << endl;
      if (no == a) {
        cout << "Found interval colouring" << endl;
        cout << "   ";
        for (int i=0; i<b; i++) {
          cout << i <<" ";
        }
        cout << endl;
        int count = 0;
        for (auto i=colouring.begin(); i<colouring.end(); i++) { // iterating over vertices in A
          cout << count << ": ";
          int iii=0, jjj=0;
          for (auto j=(*i).begin(); j<(*i).end(); j++) { // iterating over colours
            while (iii<A[count][jjj]) {
              if (iii<10) cout << "- ";
              else if (iii<100) cout << "-- ";
              else if (iii<1000) cout << "--- ";
              iii++;
            }
            cout << *j;
            if (iii<10) cout << " ";
            else if (iii<100) cout << "  ";
            else if (iii<1000) cout << "   ";
            iii++;
            jjj++;
          }
          cout << endl;
          count++;
        }
        return true;
      }
      for (int int_start=0; int_start<k-d_a+1; int_start++) { // choose the start point of the interval
        if (debug) cout << "." << endl;
        vector<int> colour_perm; 
        bool last_perm = false; // true when we did the last permutation of this colour
        for (int j=int_start; j<int_start+d_a; j++) { // populate permutation vector
          colour_perm.push_back(j);
        }
        //cout<<"[run_through_vertex] length of colour_perm: " << colour_perm.size() << endl;
        while (!last_perm) {
          if (debug) cout << "+";
          vector<pair<int,int>> old_ranges;
          for (int i=0; i<d_a; i++) { // check edge colour ranges
            //cout << "Checking ranges " << i << endl;
            int b_vertex = A[no][i]; // vertex in b that we are checking ranges for
            //cout << "b_vertex: " << b_vertex << endl;
            if (colour_perm[i] < ranges[b_vertex].first || colour_perm[i] > ranges[b_vertex].second) {
              if (debug) cout << "_";
              goto next_perm;
            }
            // check that colour is not used already
            //cout << "." << endl;
            if (find(B_cols[b_vertex].begin(), B_cols[b_vertex].end(), colour_perm[i]) != B_cols[b_vertex].end()) {
              if (debug) cout << "-";
              goto next_perm;
            }
            //cout << "." << endl;
          }

          // Update ranges, colouring and B_cols, and pass down
          colouring.push_back(colour_perm);
          for (int i=0; i<d_a; i++) {
            int b_vertex = A[no][i];
            //cout << "[" << b_vertex << "|" << max(ranges[b_vertex].first, colour_perm[i]-d_b+1) << ","
            //                            << min(ranges[b_vertex].second, colour_perm[i]+d_b-1) << ","
            //                            << colour_perm[i] << "]" << endl;
            old_ranges.push_back(ranges[b_vertex]);
            ranges[b_vertex].first = max(ranges[b_vertex].first, colour_perm[i]-d_b+1);
            ranges[b_vertex].second = min(ranges[b_vertex].second, colour_perm[i]+d_b-1);
            B_cols[b_vertex].push_back(colour_perm[i]);
          }
          // Pass to next vertex
          if (this->run_through_vertex(no+1, k, colouring, ranges, B_cols)) {
            return true;
          }
          // Restitute colouring, ranges and B colouring
          colouring.pop_back();
          for (int i=0; i<d_a; i++) {
            int b_vertex = A[no][i];
            ranges[b_vertex] = old_ranges[i];
            B_cols[b_vertex].pop_back();
          }
          next_perm: 
          last_perm = !next_permutation(colour_perm.begin(), colour_perm.end());
        }
      }
      return false;
    }
};


int main() {
  srand(unsigned(time(0)));
  cout << "Hello!\n";
  for (int i=0; i<10000; i++) {
    bp_graph graph(18,30,5,3);
    graph.print_graph();
    if (graph.check_int_col(7)) {
      cout << "Interval colourable"<< endl;
    } else {
      cout << "Not interval colourable"<< endl;
      break;
    }
  }

  return 0;
}


