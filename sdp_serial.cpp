#include <iostream>
#include <cmath>
#include <limits>
#include <time.h>

using namespace std;

void transpose(float**** matrix, float**** matrix_transposed, int batch_size, int num_heads, int rows, int cols) {
    // cout << "Transposing matrices with rows: " << rows << ", cols: " << cols << endl;
    for (int b = 0; b < batch_size; ++b) {
        for (int h = 0; h < num_heads; ++h) {
            for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < cols; ++j) {
                    // Validate indices and assignment
                    // cout << "matrix[" << b << "][" << h << "][" << i << "][" << j << "] = " << matrix[b][h][i][j] << endl;
                    // cout << "matrix_transposed[" << b << "][" << h << "][" << j << "][" << i << "] = " << matrix_transposed[b][h][j][i] << endl;
                    matrix_transposed[b][h][j][i] = matrix[b][h][i][j];
                }
            }
        }
    }
    // cout << "Transpose completed successfully." << endl;
}

void matrix_multiply(float**** A, float**** B, float**** C, int batch_size, int num_heads, int rows, int cols, int inner_dim) {
    // cout << rows << " " << cols << " " << inner_dim << endl;
    for (int b = 0; b < batch_size; ++b) {
        // cout << "Batch " << b << endl;
        for (int h = 0; h < num_heads; ++h) {
            // cout << "Head " << h << endl;
            for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < cols; ++j) {
                    C[b][h][i][j] = 0.0;
                    // cout << "C[" << b << "][" << h << "][" << i << "][" << j << "] = " << C[b][h][i][j] << endl;
                    for (int k = 0; k < inner_dim; ++k) {
                        // cout << "k = " << k << endl;
                        // cout << "A[" << b << "][" << h << "][" << i << "][" << k << "] = " << A[b][h][i][k] << endl;
                        // cout << "B[" << b << "][" << h << "][" << k << "][" << j << "] = " << B[b][h][k][j] << endl;
                        C[b][h][i][j] += A[b][h][i][k] * B[b][h][k][j];
                    }
                    // cout << "C[" << b << "][" << h << "][" << i << "][" << j << "] = " << C[b][h][i][j] << endl;
                }
            }
        }
    }
}

void scaled_dot_product_attention(float**** query, float**** key, float**** value, float**** output, int batch_size, int num_heads, int L, int S, int D, float**** attn_mask = nullptr, float dropout_p = 0.1, float scale = -1.0, bool enable_gqa = false) {
    float scale_factor = (scale == -1.0) ? 1.0 / sqrt(D) : scale;
    // cout << "Scale factor: " << scale_factor << endl;

    float**** attn_bias = new float***[batch_size];
    for (int b = 0; b < batch_size; ++b) {
        attn_bias[b] = new float**[num_heads];
        for (int h = 0; h < num_heads; ++h) {
            attn_bias[b][h] = new float*[L];
            for (int i = 0; i < L; ++i) {
                attn_bias[b][h][i] = new float[S];
                for (int j = 0; j < S; ++j) {
                    attn_bias[b][h][i][j] = 0.0;
                }
            }
        }
    }

    // cout << "attn_bias created" << endl;

    if (attn_mask != nullptr) {
        for (int b = 0; b < batch_size; ++b) {
            for (int h = 0; h < num_heads; ++h) {
                for (int i = 0; i < L; ++i) {
                    for (int j = 0; j < S; ++j) {
                        if (attn_mask[b][h][i][j] == 0) {
                            attn_bias[b][h][i][j] = -numeric_limits<float>::infinity();
                        }
                    }
                }
            }
        }
    }

    float**** attn_weight = new float***[batch_size];
    for (int b = 0; b < batch_size; ++b) {
        attn_weight[b] = new float**[num_heads];
        for (int h = 0; h < num_heads; ++h) {
            attn_weight[b][h] = new float*[L];
            for (int i = 0; i < L; ++i) {
                attn_weight[b][h][i] = new float[S];
            }
        }
    }

    // cout << "start mm" << endl;

    float**** key_transposed = new float***[batch_size];
    for (int b = 0; b < batch_size; ++b) {
        key_transposed[b] = new float**[num_heads];
        for (int h = 0; h < num_heads; ++h) {
            key_transposed[b][h] = new float*[D];
            for (int i = 0; i < D; ++i) {
                key_transposed[b][h][i] = new float[L];
                for (int j = 0; j < L; ++j) {
                    key_transposed[b][h][i][j] = 0.0;
                }
            }
        }
    }

    transpose(key, key_transposed, batch_size, num_heads, L, D);

    matrix_multiply(query, key_transposed, attn_weight, batch_size, num_heads, L, S, D);

    // cout << "end mm" << endl;

    for (int b = 0; b < batch_size; ++b) {
        for (int h = 0; h < num_heads; ++h) {
            for (int i = 0; i < L; ++i) {
                for (int j = 0; j < S; ++j) {
                    attn_weight[b][h][i][j] *= scale_factor;
                    attn_weight[b][h][i][j] += attn_bias[b][h][i][j];
                    // cout << "attn_weight[" << b << "][" << h << "][" << i << "][" << j << "] = " << attn_weight[b][h][i][j] << endl;
                }
            }
        }
    }

    for (int b = 0; b < batch_size; ++b) {
        for (int h = 0; h < num_heads; ++h) {
            for (int i = 0; i < L; ++i) {
                float max_val = -numeric_limits<float>::infinity();
                for (int j = 0; j < S; ++j) {
                    if (attn_weight[b][h][i][j] > max_val) {
                        max_val = attn_weight[b][h][i][j];
                    }
                }
                float sum = 0.0;
                for (int j = 0; j < S; ++j) {
                    attn_weight[b][h][i][j] = exp(attn_weight[b][h][i][j] - max_val);
                    sum += attn_weight[b][h][i][j];
                }
                for (int j = 0; j < S; ++j) {
                    attn_weight[b][h][i][j] /= sum;
                }
            }
        }
    }

    // cout << "Attention weights mm value start" << endl;

    matrix_multiply(attn_weight, value, output, batch_size, num_heads, L, D, S);

    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            cout << output[0][0][i][j] << " ";
        }
        cout << "\n";
    }

    // cout << "Attention weights mm value end" << endl;
    // Free allocated memory
    for (int b = 0; b < batch_size; ++b) {
        for (int h = 0; h < num_heads; ++h) {
            for (int i = 0; i < L; ++i) {
                delete[] attn_bias[b][h][i];
                delete[] attn_weight[b][h][i];
            }
            delete[] attn_bias[b][h];
            delete[] attn_weight[b][h];
        }
        delete[] attn_bias[b];
        delete[] attn_weight[b];
    }
    delete[] attn_bias;
    delete[] attn_weight;

    for (int b = 0; b < batch_size; ++b) {
        for (int h = 0; h < num_heads; ++h) {
            for (int i = 0; i < S; ++i) {
                delete[] key_transposed[b][h][i];
            }
            delete[] key_transposed[b][h];
        }
        delete[] key_transposed[b];
    }
    delete[] key_transposed;
}

int main() {
    int batch_size = 64;
    int num_heads = 12;
    int L = 64;
    int S = 64;
    int D = 1024;

    float**** query = new float***[batch_size];
    float**** key = new float***[batch_size];
    float**** value = new float***[batch_size];
    float**** output = new float***[batch_size];

    for (int b = 0; b < batch_size; ++b) {
        query[b] = new float**[num_heads];
        key[b] = new float**[num_heads];
        value[b] = new float**[num_heads];
        output[b] = new float**[num_heads];
        for (int h = 0; h < num_heads; ++h) {
            query[b][h] = new float*[L];
            key[b][h] = new float*[L];
            value[b][h] = new float*[L];
            output[b][h] = new float*[L];
            for (int i = 0; i < L; ++i) {
                query[b][h][i] = new float[D];
                key[b][h][i] = new float[D];
                value[b][h][i] = new float[D];
                output[b][h][i] = new float[D];
                for (int j = 0; j < D; ++j) {
                    query[b][h][i][j] = 1.0;
                    key[b][h][i][j] = 1.0;
                    value[b][h][i][j] = 1.0;
                }
            }
        }
    }

    // cout << "query[0][0][0][63]" << query[0][0][0][63] << endl;

    // cout << "start" << endl;
    struct timespec start, stop; 
    double time;

	if( clock_gettime(CLOCK_REALTIME, &start) == -1) { perror("clock gettime");}
    scaled_dot_product_attention(query, key, value, output, batch_size, num_heads, L, S, D);
    if( clock_gettime(CLOCK_REALTIME, &stop) == -1 ) { perror("clock gettime");}		
	time = (stop.tv_sec - start.tv_sec)+ (double)(stop.tv_nsec - start.tv_nsec)/1e9;

    cout << "Time taken for scaled_dot_product_attention: " << time << " seconds" << endl;


    // for (int b = 0; b < batch_size; ++b) {
    //     for (int h = 0; h < num_heads; ++h) {
    //         for (int i = 0; i < L; ++i) {
    //             for (int j = 0; j < D; ++j) {
    //                 cout << "output[" << b << "][" << h << "][" << i << "][" << j << "] = " << output[b][h][i][j] << endl;
    //             }
    //         }
    //     }
    // }

    // Free allocated memory
    for (int b = 0; b < batch_size; ++b) {
        for (int h = 0; h < num_heads; ++h) {
            for (int i = 0; i < L; ++i) {
                delete[] query[b][h][i];
                delete[] key[b][h][i];
                delete[] value[b][h][i];
                delete[] output[b][h][i];
            }
            delete[] query[b][h];
            delete[] key[b][h];
            delete[] value[b][h];
            delete[] output[b][h];
        }
        delete[] query[b];
        delete[] key[b];
        delete[] value[b];
        delete[] output[b];
    }
    delete[] query;
    delete[] key;
    delete[] value;
    delete[] output;

    return 0;
}