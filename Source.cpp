#include <vector>
#include <iostream>
#include <random>
#include <tuple>
#include <cmath>
#include <assert.h>
#include <algorithm>
using std::cout;
using std::vector;
int get_square_rt(int N) {
	static vector<int> cache(1, 0);
	if (cache.size() <= N) {
		for (int i = cache.size(); i <= 2 * N + 1; ++i) {
			int tmp = (int)sqrt(i + 0.5);
			assert((tmp + 1) * (tmp + 1) > i && i >= tmp * tmp);
			cache.push_back(tmp);
		}
	}
	return cache[N];
}
vector<int> search(int N) {
	vector<int> prefix;
	vector<char> slot(N + 3, false);
	int remained_max = N;
	prefix.push_back(N);
	remained_max = N - 1;
	slot[N] = true;
	slot[N + 1] = slot[N + 2] = slot[0] = true;
	int hint = N - 1;
	while (prefix.size() < N) {
		int record1 = prefix.back();
		int count = N - prefix.size();
		int s_beg = get_square_rt(hint + record1);
		for (int sum_it = s_beg; sum_it > 0; --sum_it) {
			int sum = sum_it * sum_it;
			int record2 = sum - record1;
			if (record2 < 1) {
				break;
			}
			if (!slot[record2]) {
				slot[record2] = true;
				if (record2 == remained_max) {
					for (int i = record2; i > 0; --i) {
						if (!slot[i]) {
							remained_max = i;
							break;
						}
					}
				}
				hint = remained_max;
				prefix.push_back(record2);
				goto while_end;
			}
		}
		prefix.pop_back();
		slot[record1] = false;
		remained_max = std::max(record1, remained_max);
		if (prefix.size()) {
			hint = record1 - 1;
		}
		else {
			if (record1 == 1) {
				return vector<int>();
			}
			prefix.push_back(record1 - 1);
		}
	while_end:
		;
	}
	return prefix;
}

bool is_square(int n) {
	int rt = get_square_rt(n);
	return rt * rt == n;
}


std::default_random_engine e;


class SelectEngine {
public:
	SelectEngine() : total_(0) {}
	template<typename T>
	void random_update(T& raw, const T& candidate, int freq = 1) {
		total_ = freq + total_;
		if (e() % total_ < freq) {
			// update		
			raw = candidate;
		}
	}
private:
	int total_;
};

bool check_arr(vector<int>& wtf) {
	for (int i = 0; i < wtf.size() - 1; ++i) {
		bool hh = is_square(wtf[i] + wtf[i + 1]);
		if (!hh) {
			return false;
		}
	}
	return true;
}


enum class Status {
	PerfectMatch,
	GoodMatch,
	TabuMatch,
	NoMatch
};


constexpr int tabu_N = 3000;
bool is_tabu(int step, vector<int>& tabu_table, int ep1, int ep2) {
	--ep1;
	--ep2;
	return step <= tabu_table[ep1 * tabu_N + ep2];
}

void update_tabu(int step, vector<int>& tabu_table, int ep1, int ep2) {
	tabu_table[ep1* tabu_N + ep2] = step;
	tabu_table[ep2* tabu_N + ep1] = step;
}

std::tuple<Status, int> find_best(int step, vector<int>& tabu_table, vector<int>& left, int ep1, int ep2) {
	// the only permitted operation: 
	// spilt record1 into [record1_beg x] [y record1_end] || [ep1, ep2]
	// get [record1_beg x, ep1 ep2] 
	//     [y record1_end]
	using std::make_tuple;
	std::pair<int, size_t> tabu_best, normal_best;
	tabu_best.first = normal_best.first = tabu_N;
	tabu_best.second = normal_best.second = -1;
	SelectEngine tabuSelect;
	SelectEngine normSelect;


	for (size_t index = 0; index < left.size() - 1; ++index) {
		auto x = left[index];
		auto y = left[index + 1];
		if (is_square(x + ep1)) {
			if (is_square(y + ep2)) {
				return make_tuple(Status::PerfectMatch, index);
			}
			auto new_pair = std::make_pair(y, index);
			tabuSelect.random_update(tabu_best, new_pair);
			if (!is_tabu(step, tabu_table, x, ep1)) {
				normSelect.random_update(normal_best, new_pair);
			}
		}
	}

	if (normal_best.first == tabu_N) {
		if (tabu_best.first == tabu_N) {
			return make_tuple(Status::NoMatch, -1);
		}
		else return make_tuple(Status::TabuMatch, tabu_best.second);
	}
	else return make_tuple(Status::GoodMatch, normal_best.second);
}


vector<int> tabu_insert(vector<int> raw, int N, int max) {
	assert(N == max);
	auto record1 = raw;
	auto record2 = vector<int>(1, N);
	constexpr int tabu_step = 100;
	vector<int> tabu_table(max * tabu_N, -tabu_step * 2);
	int step = 0;
	constexpr int max_step = 100000;
	while (step < max_step) {

		step += 1;
		auto record1_rev = record1;
		auto record2_rev = record2;
		std::reverse(record1_rev.begin(), record1_rev.end());
		std::reverse(record2_rev.begin(), record2_rev.end());
		auto extract = [&](int exec_id) {
			assert(exec_id < 8 && exec_id >= 0);
			switch (exec_id) {
			case 0: return std::make_pair(&record1, &record2);
			case 1: return std::make_pair(&record1_rev, &record2);
			case 2: return std::make_pair(&record1, &record2_rev);
			case 3: return std::make_pair(&record1_rev, &record2_rev);
			case 4: return std::make_pair(&record2, &record1);
			case 5: return std::make_pair(&record2_rev, &record1);
			case 6: return std::make_pair(&record2, &record1_rev);
			case 7: return std::make_pair(&record2_rev, &record1_rev);
			default: return std::make_pair((vector<int>*)nullptr, (vector<int>*)nullptr);
			}
		};
		for (int id = 0; id < 4; ++id) {
			auto[left_, right_] = extract(id);
			auto& left = *left_;
			auto& right = *right_;
			if (is_square(left.back() + right.front())) {
				// perfect match
				auto res = left;
				res.insert(res.end(), right.begin(), right.end());
				return res;
			}
		}
		int exec_id = -1;
		auto exec_status = Status::NoMatch;
		int exec_penalty = 0;
		for (int id = 0; id < 8; ++id) {
			auto[left_, right_] = extract(id);
			auto& left = *left_;
			auto& right = *right_;
			auto[status, iter] = find_best(step, tabu_table, left, right.front(), right.back());
			if (status == Status::NoMatch || status > exec_status) {
				continue;
			}
			assert(iter >= 0 && iter < left.size() - 1);
			auto penalty = left[iter + 1] - right.front();
			if (status < exec_status) {
				exec_status = status;
				exec_id = id;
				exec_penalty = penalty;
			}
			else {
				assert(status == exec_status);
				if (penalty < exec_penalty) {
					exec_status = status;
					exec_id = id;
					exec_penalty = penalty;
				}
			}
		}
		assert(exec_id != -1);
		{
			auto[left_, right_] = extract(exec_id);
			auto left = *left_;
			auto right = *right_;
			auto[status, iter] = find_best(step, tabu_table, left, right.front(), right.back());
			assert(is_square(left[iter] + right.front()));
			assert(iter >= 0 && iter < left.size() - 1);
			record1 = vector<int>(left.begin() + iter + 1, left.end());
			record2 = vector<int>(left.begin(), left.begin() + iter + 1);
			record2.insert(record2.end(), right.begin(), right.end());
		}
	}
	return vector<int>();
}
int main() {
	// generate seed using brute force
	auto vec = search(40);

	cout << 40 << ":";
	for (auto x : vec) {
		cout << x << " ";
	}
	assert(check_arr(vec));

	for (int i = 41; i < 2001; ++i) {
		vec = tabu_insert(vec, i, i);
		if (vec.size() != i) {
			cout << "failed" << std::endl;
			break;
		}
		assert(check_arr(vec));
		if (i % 100 == 0) {
			cout << std::endl;
			cout << i << ":";
			for (auto x : vec) {
				cout << x << " ";
			}
			cout << std::endl;
		}

	}
	system("pause");
	return 0;
}
