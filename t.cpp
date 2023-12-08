#include <iostream>
#include <vector>

using namespace std;

vector<vector<double>> gaussElimination(const vector<vector<double>>& A, const vector<double>& b) {
    int n = A.size();
    vector<vector<double>> A_transpose(n, vector<double>(n));
    vector<double> b_transpose(n);

    // 交换矩阵A和其转置矩阵
    for (int i = 0; i < n; ++i) {
        swap(A[i], A_transpose[i]);
    }
    swap(b, b_transpose);

    vector<double> x(n);

    // 沿着主元列进行高斯消元
    for (int i = 0; i < n - 1; ++i) {
        // 找到最大值的位置
        int max_index = 0;
        for (int j = 1; j < n; ++j) {
            if (A_transpose[j][i] > A_transpose[max_index][i]) {
                max_index = j;
            }
        }

        // 交换行
        swap(A_transpose[i], A_transpose[max_index]);
        swap(b_transpose[i], b_transpose[max_index]);

        // 消元
        for (int j = i + 1; j < n; ++j) {
            double factor = A_transpose[j][i] / A_transpose[i][i];
            for (int k = 0; k < n; ++k) {
                A_transpose[j][k] -= factor * A_transpose[i][k];
            }
            b_transpose[j] -= factor * b_transpose[i];
        }
    }

    // 回代求解线性方程组
    for (int i = n - 1; i >= 0; --i) {
        if (A_transpose[i][i] == 0) {
            for (int j = i - 1; j >= 0; --j) {
                if (A_transpose[j][i] != 0) {
                    double factor = A_transpose[j][i] / A_transpose[i][i];
                    for (int k = 0; k < n; ++k) {
                        A_transpose[j][k] -= factor * A_transpose[i][k];
                    }
                    b_transpose[j] -= factor * b_transpose[i];
                    break;
                }
            }
        }
        x[i] = b_transpose[i];
    }

    return x;
}

int main() {
    vector<vector<double>> A = {{1, 2, 3},
                              {4, 5, 6},
                              {7, 8, 9}};
    vector<double> b = {10, 11, 12};

    vector<double> x = gaussElimination(A, b);

    cout << "解： ";
    for (double value : x) {
        cout << value << " ";
    }
    cout << endl;

    return 0;
}

