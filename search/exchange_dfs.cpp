// Exact increasing-index DFS, following the traversal contributed by fabius-cunctator.
// Input: n, blocker-word count, radius; then hex blockers and compatibility rows.
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using Bits = std::vector<uint64_t>;
using Blockers = std::array<uint64_t, 16>;
struct BlockerHash {
    size_t operator()(const Blockers& b) const {
        size_t h = 0;
        for (auto x : b) h ^= std::hash<uint64_t>{}(x) + size_t(0x9e3779b97f4a7c15ULL) + (h << 6) + (h >> 2);
        return h;
    }
};
struct Search {
    size_t n, bw, cw;
    unsigned radius;
    std::vector<Blockers> blockers;
    std::vector<Bits> compatible;
    std::vector<unsigned> chosen;
    uint64_t started = 0, completed = 0;
    bool subset_index = false;
    uint64_t subset_probes = 0, budget_prunes = 0;
    bool budget_bitsets = false;
    std::vector<Bits> blocker_incidence;
    static constexpr unsigned budget_max_degree = 9;
    std::array<Bits, budget_max_degree + 1> degree_masks;
    unsigned max_degree = 0;
    uint64_t bitset_filters = 0;
    bool noncore_bound = false;
    unsigned core_prefix = 0, noncore_limit = 0;
    Blockers noncore_mask{};
    uint64_t noncore_prunes = 0;
    bool enumerate_neutral = false, output_limited = false;
    unsigned target_size;
    int root_prefix = -1;
    unsigned root_start = 0;
    int last_completed_root = -1;
    bool anchor_bounds = false;
    unsigned anchor_prefix = 0;
    std::vector<unsigned> anchor_requirements;
    uint64_t anchor_prunes = 0;
    bool pair_bounds = false;
    std::vector<std::vector<std::pair<unsigned, unsigned>>> pair_requirements;
    std::vector<std::vector<unsigned>> neutral, improving;
    std::unordered_map<Blockers, std::vector<unsigned>, BlockerHash> groups;
    bool timed_out = false;
    std::chrono::steady_clock::time_point deadline;

    bool dfs(size_t first, const Blockers& used, const Bits& allowed, unsigned used_count,
             unsigned anchors = 0, unsigned required = 0) {
        if (anchor_bounds && required > anchors + target_size - chosen.size()) {
            ++anchor_prunes;
            return false;
        }
        if (chosen.size() >= target_size) {
            if (!enumerate_neutral) return true;
            (used_count == radius ? neutral : improving).push_back(chosen);
            if (neutral.size() + improving.size() >= 100000) output_limited = true;
            return false;  // Keep traversing after each result.
        }
        Bits filtered;
        const Bits* active = &allowed;
        if (subset_index && !budget_bitsets && used_count == radius) {
            // A full budget cannot acquire another blocker. Enumerate every subset,
            // including the empty one; then preserve compatibility and index order.
            std::vector<unsigned> positions;
            for (size_t w = 0; w < bw; ++w) {
                uint64_t bits = used[w];
                while (bits) {
                    positions.push_back(unsigned(w * 64 + __builtin_ctzll(bits)));
                    bits &= bits - 1;
                }
            }
            filtered.assign(cw, 0);
            Blockers sub{};
            unsigned previous_gray = 0, remaining = 0;
            for (unsigned j = 0; j < (1u << positions.size()); ++j) {
                unsigned gray = j ^ (j >> 1);
                if (j) {
                    unsigned p = positions[__builtin_ctz(gray ^ previous_gray)];
                    sub[p / 64] ^= uint64_t(1) << (p % 64);
                }
                previous_gray = gray;
                ++subset_probes;
                auto group = groups.find(sub);
                if (group == groups.end()) continue;
                for (unsigned i : group->second) {
                    uint64_t bit = uint64_t(1) << (i % 64);
                    if (i >= first && (allowed[i / 64] & bit)) {
                        filtered[i / 64] |= bit;
                        ++remaining;
                    }
                }
            }
            if (chosen.size() + remaining < target_size) {
                ++budget_prunes;
                return false;
            }
            active = &filtered;
        } else if (budget_bitsets && radius - used_count < max_degree) {
            // Count, in parallel, how many blockers of each candidate are
            // already paid for. A degree-d candidate needs d-slack shared
            // blockers; the remaining blockers fit the removal budget.
            std::vector<unsigned> paid;
            for (size_t w = 0; w < bw; ++w) {
                uint64_t bits = used[w];
                while (bits) {
                    paid.push_back(unsigned(w * 64 + __builtin_ctzll(bits)));
                    bits &= bits - 1;
                }
            }
            filtered.assign(cw, 0);
            unsigned slack = radius - used_count, remaining = 0;
            for (size_t w = first / 64; w < cw; ++w) {
                if (!allowed[w]) continue;
                uint64_t hits[budget_max_degree + 1]{};
                for (unsigned old : paid) {
                    uint64_t present = blocker_incidence[old][w];
                    for (unsigned k = max_degree; k > 1; --k)
                        hits[k] |= hits[k - 1] & present;
                    hits[1] |= present;
                }
                uint64_t eligible = 0;
                for (unsigned d = 0; d <= max_degree; ++d)
                    eligible |= degree_masks[d][w] & (d <= slack ? ~uint64_t(0) : hits[d - slack]);
                filtered[w] = allowed[w] & eligible;
                if (w == first / 64) filtered[w] &= ~uint64_t(0) << (first % 64);
                remaining += __builtin_popcountll(filtered[w]);
            }
            ++bitset_filters;
            if (chosen.size() + remaining < target_size) {
                ++budget_prunes;
                return false;
            }
            active = &filtered;
        }
        if (anchor_bounds && required > anchors) {
            unsigned available = 0, need = required - anchors;
            for (size_t w = first / 64; w < (anchor_prefix + 63) / 64; ++w) {
                uint64_t bits = (*active)[w];
                if (w == first / 64) bits &= (~uint64_t(0) << (first % 64));
                if (w == anchor_prefix / 64 && anchor_prefix % 64)
                    bits &= (uint64_t(1) << (anchor_prefix % 64)) - 1;
                available += __builtin_popcountll(bits);
                if (available >= need) break;
            }
            if (available < need) {
                ++anchor_prunes;
                return false;
            }
        }
        Bits next(cw);
        for (size_t w = first / 64; w < cw; ++w) {
            uint64_t bits = (*active)[w];
            if (w == first / 64) bits &= (~uint64_t(0) << (first % 64));
            while (bits) {
                unsigned bit = __builtin_ctzll(bits);
                bits &= bits - 1;
                size_t i = w * 64 + bit;
                // Increasing order: requiring any prefix vertex is equivalent
                // to requiring the first selected vertex to be in that prefix.
                if (chosen.empty() && root_prefix >= 0 && i >= unsigned(root_prefix)) return false;
                ++started;
                if ((started & 1023) == 1 && std::chrono::steady_clock::now() >= deadline) {
                    timed_out = true;
                    return false;
                }
                Blockers merged{};
                unsigned count = 0;
                for (size_t j = 0; j < bw; ++j) {
                    merged[j] = used[j] | blockers[i][j];
                    count += __builtin_popcountll(merged[j]);
                    if (count > radius) break;
                }
                if (count <= radius) {
                    // Conditional restriction supplied by a separately checked core bound.
                    unsigned noncore_count = 0;
                    if (noncore_bound) {
                        for (size_t j = core_prefix / 64; j < bw; ++j)
                            noncore_count += __builtin_popcountll(merged[j] & noncore_mask[j]);
                    }
                    if (noncore_bound && noncore_count > noncore_limit) {
                        ++noncore_prunes;
                        ++completed;
                        if (chosen.empty()) last_completed_root = int(i);
                        continue;
                    }
                    for (size_t j = 0; j < cw; ++j) next[j] = (*active)[j] & compatible[i][j];
                    unsigned next_anchors = anchors, next_required = required;
                    if (anchor_bounds) {
                        next_anchors += i < anchor_prefix;
                        if (anchor_requirements[i] > next_required) next_required = anchor_requirements[i];
                        if (pair_bounds) {
                            // Store rules at their later endpoint; a selected earlier
                            // endpoint is already present in chosen.
                            for (const auto& rule : pair_requirements[i])
                                for (unsigned earlier : chosen)
                                    if (earlier == rule.first && rule.second > next_required)
                                        next_required = rule.second;
                        }
                    }
                    chosen.push_back(unsigned(i));
                    if (dfs(i + 1, merged, next, count, next_anchors, next_required)) return true;
                    chosen.pop_back();
                    if (timed_out || output_limited) return false;
                }
                ++completed;
                if (chosen.empty()) last_completed_root = int(i);
            }
        }
        return false;
    }
};

int main(int argc, char** argv) {
    try {
        if (argc < 2) throw std::runtime_error("Expected cutoff seconds [--subset] [--budget-bitsets] [--neutral] [--root-prefix N] [--noncore-limit CORE_PREFIX LIMIT] [--anchor-bounds PREFIX] [--pair-bounds]");
        double seconds = std::stod(argv[1]);
        if (!(seconds > 0 && seconds <= 600)) throw std::runtime_error("Invalid cutoff");
        Search s;
        for (int a = 2; a < argc; ++a) {
            std::string option = argv[a];
            if (option == "--subset") s.subset_index = true;
            else if (option == "--budget-bitsets") s.budget_bitsets = true;
            else if (option == "--neutral") s.enumerate_neutral = true;
            else if (option == "--pair-bounds") s.pair_bounds = true;
            else if (option == "--root-start" && a + 1 < argc) {
                int first = std::stoi(argv[++a]);
                if (first < 0 || first > 20000) throw std::runtime_error("Invalid root start");
                s.root_start = unsigned(first);
            }
            else if (option == "--anchor-bounds" && a + 1 < argc) {
                int prefix = std::stoi(argv[++a]);
                if (prefix < 0 || prefix > 20000) throw std::runtime_error("Invalid anchor prefix");
                s.anchor_bounds = true;
                s.anchor_prefix = unsigned(prefix);
            }
            else if (option == "--root-prefix" && a + 1 < argc) {
                s.root_prefix = std::stoi(argv[++a]);
                if (s.root_prefix < 0 || s.root_prefix > 20000)
                    throw std::runtime_error("Invalid root prefix");
            }
            else if (option == "--noncore-limit" && a + 2 < argc) {
                int prefix = std::stoi(argv[++a]), limit = std::stoi(argv[++a]);
                if (prefix < 0 || prefix > 1024 || limit < 0 || limit > 8)
                    throw std::runtime_error("Invalid noncore restriction");
                s.noncore_bound = true;
                s.core_prefix = unsigned(prefix);
                s.noncore_limit = unsigned(limit);
            } else throw std::runtime_error("Unknown or incomplete option");
        }
        if (s.pair_bounds && !s.anchor_bounds)
            throw std::runtime_error("Pair bounds require anchor bounds");
        if (!(std::cin >> s.n >> s.bw >> s.radius) || !s.n || s.n > 20000 ||
            !s.bw || s.bw > 16 || s.radius > 9) throw std::runtime_error("Invalid dimensions");
        s.cw = (s.n + 63) / 64;
        if (s.root_prefix > int(s.n)) throw std::runtime_error("Root prefix exceeds dimensions");
        unsigned root_end = s.root_prefix >= 0 ? unsigned(s.root_prefix) : unsigned(s.n);
        if (s.root_start > root_end) throw std::runtime_error("Root start exceeds range");
        s.last_completed_root = int(s.root_start) - 1;
        if (s.anchor_prefix > s.n) throw std::runtime_error("Anchor prefix exceeds dimensions");
        if (s.enumerate_neutral && (!s.radius || s.noncore_bound))
            throw std::runtime_error("Neutral mode requires positive radius and no noncore restriction");
        s.target_size = s.enumerate_neutral ? s.radius : s.radius + 1;
        if (s.noncore_bound) {
            if (s.core_prefix > s.bw * 64 || s.noncore_limit > s.radius)
                throw std::runtime_error("Noncore restriction exceeds dimensions");
            for (size_t j = 0; j < s.bw * 64; ++j)
                if (j >= s.core_prefix) s.noncore_mask[j / 64] |= uint64_t(1) << (j % 64);
        }
        s.blockers.resize(s.n);
        s.compatible.assign(s.n, Bits(s.cw));
        std::cin >> std::hex;
        for (size_t i = 0; i < s.n; ++i) {
            for (size_t j = 0; j < s.bw; ++j)
                if (!(std::cin >> s.blockers[i][j])) throw std::runtime_error("Missing blocker word");
            for (auto& word : s.compatible[i])
                if (!(std::cin >> word)) throw std::runtime_error("Missing compatibility word");
            if (s.n % 64 && (s.compatible[i].back() >> (s.n % 64)))
                throw std::runtime_error("Out of range compatibility bit");
        }
        Bits full(s.cw, ~uint64_t(0));
        if (s.anchor_bounds) {
            std::cin >> std::dec;
            s.anchor_requirements.resize(s.n);
            for (auto& requirement : s.anchor_requirements)
                if (!(std::cin >> requirement) || requirement > 20000)
                    throw std::runtime_error("Invalid or missing anchor requirement");
        }
        if (s.pair_bounds) {
            unsigned count;
            if (!(std::cin >> count) || count > 200000)
                throw std::runtime_error("Invalid pair count");
            s.pair_requirements.resize(s.n);
            for (unsigned j = 0; j < count; ++j) {
                unsigned a, b, requirement;
                if (!(std::cin >> a >> b >> requirement) || a >= b || b >= s.n || requirement > 20000)
                    throw std::runtime_error("Invalid pair requirement");
                s.pair_requirements[b].emplace_back(a, requirement);
            }
        }
        if (s.n % 64) full.back() = (uint64_t(1) << (s.n % 64)) - 1;
        if (s.subset_index && !s.budget_bitsets)
            for (size_t i = 0; i < s.n; ++i) s.groups[s.blockers[i]].push_back(unsigned(i));
        if (s.budget_bitsets) {
            s.blocker_incidence.assign(s.bw * 64, Bits(s.cw));
            for (auto& mask : s.degree_masks) mask.assign(s.cw, 0);
            for (size_t i = 0; i < s.n; ++i) {
                unsigned degree = 0;
                for (size_t w = 0; w < s.bw; ++w) {
                    uint64_t bits = s.blockers[i][w];
                    degree += __builtin_popcountll(bits);
                    while (bits) {
                        unsigned old = unsigned(w * 64 + __builtin_ctzll(bits));
                        s.blocker_incidence[old][i / 64] |= uint64_t(1) << (i % 64);
                        bits &= bits - 1;
                    }
                }
                if (degree > s.budget_max_degree) throw std::runtime_error("Budget bitsets require at most nine blockers per candidate");
                s.degree_masks[degree][i / 64] |= uint64_t(1) << (i % 64);
                if (degree > s.max_degree) s.max_degree = degree;
            }
        }
        auto began = std::chrono::steady_clock::now();
        s.deadline = began + std::chrono::duration_cast<std::chrono::steady_clock::duration>(
            std::chrono::duration<double>(seconds));
        bool found = s.root_start < root_end && s.dfs(s.root_start, Blockers{}, full, 0);
        double elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - began).count();
        std::cout << "{\"status\":\"" << (found ? "witness" : s.timed_out ? "timeout" : s.output_limited ? "output_limit" : "exhausted")
                  << "\",\"nodes_started\":" << s.started << ",\"nodes_completed\":" << s.completed
                  << ",\"seconds\":" << elapsed << ",\"chosen\":[";
        for (size_t i = 0; i < s.chosen.size(); ++i) std::cout << (i ? "," : "") << s.chosen[i];
        std::cout << "],\"subset_index\":" << (s.subset_index ? "true" : "false")
                  << ",\"subset_probes\":" << s.subset_probes
                  << ",\"budget_prunes\":" << s.budget_prunes
                  << ",\"budget_bitsets\":" << (s.budget_bitsets ? "true" : "false")
                  << ",\"bitset_filters\":" << s.bitset_filters
                  << ",\"noncore_bound\":" << (s.noncore_bound ? "true" : "false")
                  << ",\"core_prefix\":" << s.core_prefix << ",\"noncore_limit\":" << s.noncore_limit
                  << ",\"noncore_prunes\":" << s.noncore_prunes
                  << ",\"enumerate_neutral\":" << (s.enumerate_neutral ? "true" : "false");
        std::cout << ",\"root_prefix\":" << s.root_prefix;
        // A timeout may interrupt one root subtree. All earlier completed
        // root subtrees are safe to omit on a subsequent identical query.
        unsigned next_root = !found && !s.timed_out && !s.output_limited
            ? root_end : unsigned(s.last_completed_root + 1);
        std::cout << ",\"root_start\":" << s.root_start << ",\"root_end\":" << root_end
                  << ",\"next_root\":" << next_root;
        std::cout << ",\"anchor_bounds\":" << (s.anchor_bounds ? "true" : "false")
                  << ",\"anchor_prefix\":" << s.anchor_prefix << ",\"anchor_prunes\":" << s.anchor_prunes;
        std::cout << ",\"pair_bounds\":" << (s.pair_bounds ? "true" : "false");
        auto print_results = [](const char* name, const std::vector<std::vector<unsigned>>& rows) {
            std::cout << ",\"" << name << "\":[";
            for (size_t i = 0; i < rows.size(); ++i) {
                std::cout << (i ? ",[" : "[");
                for (size_t j = 0; j < rows[i].size(); ++j)
                    std::cout << (j ? "," : "") << rows[i][j];
                std::cout << ']';
            }
            std::cout << ']';
        };
        print_results("neutral", s.neutral);
        print_results("improving", s.improving);
        std::cout << "}\n";
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
