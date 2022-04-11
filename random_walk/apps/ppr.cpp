//
// Created by Shixuan Sun on 11/5/20.
//

#include "creeper_main.h"

// By default, we only count the steps without logging the
HyperParameter g_para = {ExecutionMode::Uniform, SamplingMethod::UniformSampling, 0, {nullptr, 0}, false, true};

struct PPR {
    double tp_;
    sfmt_t sfmt_;

    PPR() {}

    PPR(double tp) : tp_(tp) {
        auto seed = static_cast<uint32_t>(time(nullptr));
        sfmt_init_gen_rand(&sfmt_, seed);
    }

    PPR(const PPR& ppr) : PPR(ppr.tp_) {}

    inline double weight(WalkerMeta& w, intT begin, intT end, int64_t eid) {
        return 1.0;
    }

    inline bool is_stop(WalkerMeta& w, intT begin, intT end, int64_t eid) {
        return sfmt_genrand_real1(&sfmt_) <= tp_;
    }

    inline bool update(WalkerMeta& w, intT begin, intT end, int64_t eid) {
        return sfmt_genrand_real1(&sfmt_) <= tp_;
    }

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

    g_para.length_ = 1; //keep length>0

    uint nw = graph.num_vertices();
    if (!cmd_parser.get_cmd_option("-nw").empty()) {
        nw = std::stoi(cmd_parser.get_cmd_option("-nw"));
    }

    //log_info("input nw: %llu", nw);

    if (nw<100){
        nw = nw*graph.num_vertices();
    }

    //log_info("input nw*5: %llu", nw);

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
    //log_info("expected walker num: %llu", nw);
    generate_single_source(graph, walkers, nw, 0);
    //log_info("total vertex num: %lld", graph.num_vertices());
    //log_info("total walker num: %llu", walkers.size());
    printf("%d\n", walkers[0].source_);

    /**
     * Execute the walkers.
     */
    compute(graph, walkers, PPR(sp));

    /**
     * Finalize the walkers as necessary.
     */
}