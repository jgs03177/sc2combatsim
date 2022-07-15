#pragma once
#include <vector>
#include <random>

class RandomSample {
private:

	std::random_device _device;
	std::mt19937_64 _generator;

public:

	RandomSample() : _generator(_device()) {}

	// dirichlet with alpha=(1,1,...,1), dimension k
	template <typename T = float> std::vector<T> dirichlet1(int k) {
		std::vector<T> ratios(k, 0);
		std::gamma_distribution<T> distribution(1.0, 1.0);
		T sum = 0;
		for (T& ratio : ratios) {
			T sample = distribution(_generator);
			sum += sample;
			ratio = sample;
		}
		for (T& ratio : ratios) {
			ratio /= sum;
		}
		return ratios;
	}

	// Beta(1, k)
	// see (https://en.wikipedia.org/wiki/Dirichlet_distribution#Marginal_beta_distributions)
	template <typename T = float> T beta1k(int k) {
		std::uniform_real_distribution<T> distribution(0.0f, 1.0f);
		float u = distribution(_generator);
		return 1 - pow(1 - u, 1.0f / k);
	}

	// uniform discrete dist 0~n-1, dimension k (k samples)
	template <typename T = int> std::vector<T> randints(T n, int k) {
		std::vector<T> samples(k, 0);
		std::uniform_int_distribution<T> distribution(0, n - 1);
		for (T& sample : samples) {
			sample = distribution(_generator);
		}
		return samples;
	}

	// uniform discrete dist 0~n-1, 1 sample
	template <typename T = int> T randint(T n) {
		std::uniform_int_distribution<T> distribution(0, n - 1);
		return distribution(_generator);
	}

	// uniform continuous dist 0~1, 1 sample
	template <typename T = float> T random() {
		std::uniform_real_distribution<T> distribution(0, 1);
		return distribution(_generator);
	}

	template <typename RandomIt> void shuffle(RandomIt first, RandomIt last) {
		std::shuffle(first, last, _generator);
	}
};
