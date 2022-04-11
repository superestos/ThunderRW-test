//
// Created by Shixuan Sun on 11/3/20.
//

#include "creeper_main.h"

HyperParameter g_para = {ExecutionMode::Static, SamplingMethod::AliasSampling, 0, {nullptr, 0}, false, false};

struct DeepWalk {
    int l_;
    Graph* g_;

    DeepWalk() {}

    DeepWalk(int l, Graph* g) : l_(l), g_(g) {}

    DeepWalk(const DeepWalk& dw) : DeepWalk(dw.l_, dw.g_) {}

    inline double weight(WalkerMeta& w, intT begin, intT end, int64_t eid) {
        return g_->edge_weight_[eid];
    }

    inline bool update(WalkerMeta& w, intT begin, intT end, int64_t eid) {
        return w.length_ == l_;
    }

    inline double max_weight(WalkerMeta& w) {
        return 5.0;
    }
    inline bool is_stop(WalkerMeta& w, intT begin, intT end, int64_t eid){
        return false;
    }
};

void execute(Graph &graph, InputParser &cmd_parser) {
    /**
     * Extract the parameters.
     *  1. -l: the length.
     *  2. -nw: if nw<100: walker num=nw*|V|; else walker num=nw
     */
    int l = 80;
    if (!cmd_parser.get_cmd_option("-l").empty()) {
        l = std::stoi(cmd_parser.get_cmd_option("-l"));
    }
    g_para.length_ = l;

    uint nw = graph.num_vertices();
    if (!cmd_parser.get_cmd_option("-nw").empty()) {
        nw = std::stoi(cmd_parser.get_cmd_option("-nw"));
    }
    //log_info("input nw: %d", nw);

    if (nw<100){
        nw = nw*graph.num_vertices();
    }
    //log_info("input nw*5: %d", nw);

    int em = static_cast<int>(ExecutionMode::Static);
    if (!cmd_parser.get_cmd_option("-em").empty()) {
        em = std::stoi(cmd_parser.get_cmd_option("-em"));
    }

    g_para.execution_ = static_cast<ExecutionMode>(em);

    int sm = static_cast<int>(SamplingMethod::AliasSampling);
    if (!cmd_parser.get_cmd_option("-sm").empty()) {
        sm = std::stoi(cmd_parser.get_cmd_option("-sm"));
    }

    g_para.sample_ = static_cast<SamplingMethod>(sm);

    /**
     * Initialize the walkers.
     */
    std::vector<WalkerMeta> walkers;
    //generate_all_source(graph, walkers);
    generate_random_source(graph, walkers, nw, 0);
    //log_info("total vertex num: %d", graph.num_vertices());
    //log_info("total walker num: %d", walkers.size());
    /**
     * Execute the walkers.
     */
    compute(graph, walkers, DeepWalk(l, &graph));

    /**
     * Finalize the walkers as necessary.
     */
}