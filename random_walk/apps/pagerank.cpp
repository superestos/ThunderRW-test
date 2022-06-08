#include "creeper_main.h"

// By default, we only count the steps without logging the
HyperParameter g_para = {ExecutionMode::Uniform, SamplingMethod::UniformSampling, 0, {nullptr, 0}, true, false};

struct PPR {
    int l_;
    double tp_;
    sfmt_t sfmt_;

    PPR() {}

    PPR(int l, double tp) : l_(l), tp_(tp) {
        auto seed = static_cast<uint32_t>(time(nullptr));
        sfmt_init_gen_rand(&sfmt_, seed);
    }

    PPR(const PPR& ppr) : PPR(ppr.l_, ppr.tp_) {}

    inline double weight(WalkerMeta& w, intT begin, intT end, int64_t eid) {
        return 1.0;
    }

    inline bool update(WalkerMeta& w, intT begin, intT end, int64_t eid) {
        return w.length_ == l_;
    }

    inline bool is_stop(WalkerMeta& w, intT begin, intT end, int64_t eid) {
        return sfmt_genrand_real1(&sfmt_) <= tp_;
    }
    // inline bool update(WalkerMeta& w, intT begin, intT end, int64_t eid) {
    //     return sfmt_genrand_real1(&sfmt_) <= tp_;
    // }

    inline double max_weight(WalkerMeta& w) {
        return 1.0;
    }
};


void execute(Graph &graph, InputParser &cmd_parser) {
    /**
     * Extract the parameters.
     *  1. -sp: the stop probability.
     *  2. -nw: the number of walkers. 
     *          if nw<100: walker num=nw*|V|; 
     *          else walker num=nw.
     *  3. -em: the execution mode of the engine.
     *  4. -sm: the sampling method of the engine.
     *  5.  -l: the target length.
     */
    double sp = 0.15;
    if (!cmd_parser.get_cmd_option("-sp").empty()) {
        sp = std::stod(cmd_parser.get_cmd_option("-sp"));
    }

    int l = 80;
    if (!cmd_parser.get_cmd_option("-l").empty()) {
        l = std::stoi(cmd_parser.get_cmd_option("-l"));
    }
    g_para.length_ = l;

    uint nw = graph.num_vertices();
    if (!cmd_parser.get_cmd_option("-nw").empty()) {
        nw = std::stoi(cmd_parser.get_cmd_option("-nw"));
    }
    if (nw<100){
        nw = nw*graph.num_vertices();
    }


    int em = static_cast<int>(ExecutionMode::Uniform);
    if (!cmd_parser.get_cmd_option("-em").empty()) {
        em = std::stoi(cmd_parser.get_cmd_option("-em"));
    }

    g_para.execution_ = static_cast<ExecutionMode>(em);

    int sm = static_cast<int>(SamplingMethod::UniformSampling);
    if (!cmd_parser.get_cmd_option("-sm").empty()) {
        sm = std::stoi(cmd_parser.get_cmd_option("-sm"));
    }

    g_para.sample_ = static_cast<SamplingMethod>(sm);

    /**
     * Initialize the walkers.
     */
    std::vector<WalkerMeta> walkers;
    //generate_single_source(graph, walkers, nw, 0);
    generate_random_source(graph, walkers, nw, 0);
    //printf("%d\n", walkers[0].source_);

    /**
     * Execute the walkers.
     */
    compute(graph, walkers, PPR(l, sp));

    /**
     * Finalize the walkers as necessary.
     */
}