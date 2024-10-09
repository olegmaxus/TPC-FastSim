#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <algorithm>

#include "model.h"


int main(int argc, char **argv) {
  model_init(1);

  int batch_size = get_batch_size();

  std::vector<float> input (batch_size * 6);
  for (int ib = 0; ib < batch_size; ++ib) {
      input[6 * ib + 0] = 15.f; // crossing angle
      input[6 * ib + 1] = 45.f; // dip angle
      input[6 * ib + 2] = 150.f; // drift length
      input[6 * ib + 3] = 42.f; // pad coordinate
      input[6 * ib + 4] = 40.f; // padrow
      input[6 * ib + 5] = 1.25f; // pT
  }
  std::vector<float> output(batch_size * 8 * 16, 0.f);

  std::vector<float> duration_values;
  int num_operations = std::max(10000 / batch_size, 1);
  for (int i_exp = 0; i_exp < 5; ++i_exp) {
    auto t1 = std::chrono::high_resolution_clock::now();
    for (int i_op = 0; i_op < num_operations; ++i_op) {
      model_run(input.data(), output.data(), batch_size * 6, batch_size * 8 * 16);
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    duration_values.push_back(((float)duration) / (num_operations * batch_size));
  }

  float sum = 0, sum2 = 0;
  for (auto duration : duration_values) {
    sum += duration;
    sum2 += duration * duration;
  }

  float mean = sum / duration_values.size();
  float mean2 = sum2 / duration_values.size();
  float std = sqrt((mean2 - mean * mean) / (duration_values.size() - 1));

  std::cout << "With batch_size = " << batch_size
            << ", duration per 1 generation is: "
            << mean << " +\\- " << std << "ms" << std::endl;

  std::cout << "Example output:" << std::endl;
  for (int ix = 0; ix < 8; ix++) {
    for (int iy = 0; iy < 16; iy++) {
      std::cout << output[ix * 16 + iy] << " ";
    }
    std::cout << std::endl;
  }

  model_free();
  return 0;
}
