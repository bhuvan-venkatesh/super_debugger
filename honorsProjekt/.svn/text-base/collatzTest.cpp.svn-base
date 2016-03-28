#include <bits/stdc++.h>
using namespace std;

// Run with: g++ -std=c++0x -g collatzTest.cpp -o collatzTest

int dist[1000005] = {0};


int findLen(int n) {
	if (n == 1)
		return 1;
		
	if (n & 1) {
		if (dist[3 * n + 1] != 0) 
			return(1 + dist[3 * n + 1]);
		return 1 + findLen(3 * n + 1);
	}
	else {
		if (dist[n >> 1] != 0) 
			return 1 + dist[n >> 1];
		return 1 + findLen(n >> 1);
	}
}

int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);
	
	int a, b, m = 0;
	dist[1] = 1;
	
	for (int i = 3; i < 1000000; i += 2) {
		dist[i - 1] = dist[(i - 1) >> 1] + 1;
		dist[i] = findLen(i);
	}
	
	cin >> a >> b;
	while (!cin.eof()) {
		for (int i = a; i <= b; ++i) 
			m = max(dist[i], m);
		
		cout << a << " " << b << " " << m << "\n";
		cin >> a >> b;
		m = 0;
	}
	
	return 0;
}
