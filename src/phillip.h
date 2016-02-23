/* -*- coding: utf-8 -*- */

#pragma once

#include <string>
#include <map>
#include <chrono>

#include "./kb.h"
#include "./interface.h"
#include "./logical_function.h"
#include "./proof_graph.h"
#include "./ilp_problem.h"
#include "./optimization.h"


/** A namespace of Henry.  */
namespace phil
{

class lhs_enumerator_t;
class ilp_converter_t;
class ilp_solver_t;


namespace wf /// Flags for writing.
{
    extern const bits_t WR_FGEN; /// Write to path-lhs.
    extern const bits_t WR_FCNV; /// Write to path-ilp.
    extern const bits_t WR_FSOL; /// Write to path-sol.
    extern const bits_t WR_FOUT; /// Write to path-out and stdout.
    extern const bits_t WR_ALL;  /// Equals to (WR_FGEN | WR_FCNV | FSOL | FOUT).
    extern const bits_t TRUNK;   /// Overwrite.
}


/** Main class of Phillip.
 *  The procedure of inference is following:
 *   - Create an instance of phillip_main_t.
 *   - Create components.
 *   - Set the components to phillip_main_t instance.
 *   - Call infer() with input observation. */
class phillip_main_t
{
public:
    static inline void set_verbose(int v);
    static inline const int verbose();

    static const std::string VERSION;

    phillip_main_t();
    ~phillip_main_t();
    
    /** Infer a explanation to given observation.
     *  You can get the results via accesser functions.
     *  @param inputs A list of observations.
     *  @param idx    Index of an observation to infer. */
    void infer(const lf::input_t &input);

    /** Do learning on given observation. */
    void learn(const lf::input_t &input, opt::epoch_t epoch);

    inline const std::unique_ptr<lhs_enumerator_t>& generator() const { return m_lhs_enumerator; }
    inline std::unique_ptr<lhs_enumerator_t>& generator() { return m_lhs_enumerator; }
    inline const std::unique_ptr<ilp_converter_t>& converter() const { return m_ilp_convertor; }
    inline std::unique_ptr<ilp_converter_t>& converter() { return m_ilp_convertor; }
    inline const std::unique_ptr<ilp_solver_t>& solver() const { return m_ilp_solver; }
    inline std::unique_ptr<ilp_solver_t>& solver() { return m_ilp_solver; }

    inline void set_timeout_lhs(duration_time_t t) { m_timeout_lhs.set(t); }
    inline void set_timeout_ilp(duration_time_t t) { m_timeout_ilp.set(t); }
    inline void set_timeout_sol(duration_time_t t) { m_timeout_sol.set(t); }
    inline void set_timeout_all(duration_time_t t) { m_timeout_all.set(t); }

    inline void set_param(const string_t &key, const string_t &param);
    inline void erase_param(const std::string &key);

    inline void set_flag(const string_t &key);
    inline void erase_flag(const std::string &key);
    
    inline const lf::input_t* get_input() const;
    inline const lf::logical_function_t* get_observation() const;
    inline const lf::logical_function_t* get_requirement() const;
    inline const pg::proof_graph_t* get_latent_hypotheses_set() const;
    inline const ilp::ilp_problem_t* get_ilp_problem() const;
    inline const std::vector<ilp::ilp_solution_t>& get_solutions() const;
    inline const std::vector<ilp::ilp_solution_t>& get_positive_answer() const;
    inline const opt::training_result_t* get_training_result() const;

    inline const util::timeout_t& timeout_lhs() const { return m_timeout_lhs; }
    inline const util::timeout_t& timeout_ilp() const { return m_timeout_ilp; }
    inline const util::timeout_t& timeout_sol() const { return m_timeout_sol; }
    inline const util::timeout_t& timeout_all() const { return m_timeout_all; }

    inline const hash_map<string_t, string_t>& params() const;
    inline const string_t& param(const string_t &key) const;
    inline int param_int(const std::string &key, int def = -1) const;
    inline float param_float(const std::string &key, float def = -1.0f) const;

    inline const hash_set<string_t>& flags() const;
    inline bool flag(const std::string &key) const;
    inline bool do_infer_pseudo_positive() const;

    inline float get_time_for_lhs()  const;
    inline float get_time_for_ilp()  const;
    inline float get_time_for_sol()  const;
    inline float get_time_for_infer() const;

    inline void add_target(const std::string &name);
    inline void clear_targets();
    inline bool is_target(const std::string &name) const;
    inline void add_exclusion(const std::string &name);
    inline void clear_exclusions();
    inline bool is_excluded(const std::string &name) const;
    
    bool check_validity_for_infer() const;
    bool check_validity_for_train() const;

    void write(
        const std::function<void(std::ostream*)> &writer,
        bits_t flags = wf::WR_ALL) const;
    
    void write_header() const;
    void write_header(std::ostream *os) const;
    
    void write_footer() const;
    void write_footer(std::ostream *os) const;

protected:
    inline void reset_for_inference();
    inline void set_input(const lf::input_t&);

    inline void execute_enumerator();
    inline void execute_convertor();
    inline void execute_solver();
    
    void execute_enumerator(
        std::unique_ptr<pg::proof_graph_t> *out_lhs, duration_time_t *out_time,
        const std::string &path_out_xml);
    void execute_convertor(
        std::unique_ptr<ilp::ilp_problem_t> *out_ilp, duration_time_t *out_time,
        const std::string &path_out_xml);
    void execute_solver(
        const ilp::ilp_problem_t *prob,
        std::vector<ilp::ilp_solution_t> *out_sols,
        duration_time_t *out_clock,
        const std::string &path_out_xml);

private:
    phillip_main_t(const phillip_main_t &) {} /// COPY IS PROHIBITED.

    static int ms_verboseness;

    // ---- FUNCTION CLASS OF EACH PROCEDURE
    std::unique_ptr<lhs_enumerator_t> m_lhs_enumerator;
    std::unique_ptr<ilp_converter_t>  m_ilp_convertor;
    std::unique_ptr<ilp_solver_t>     m_ilp_solver;

    // ---- DATA, SETTING, ETC...
    hash_map<string_t, string_t> m_params;
    hash_set<string_t> m_flags;
    util::timeout_t m_timeout_lhs, m_timeout_ilp, m_timeout_sol, m_timeout_all;
    
    hash_set<std::string> m_target_obs_names;
    hash_set<std::string> m_excluded_obs_names;

    // ---- PRODUCTS OF INFERENCE
    std::unique_ptr<lf::input_t> m_input;
    std::unique_ptr<pg::proof_graph_t> m_lhs;
    std::unique_ptr<ilp::ilp_problem_t> m_ilp;
    std::unique_ptr<ilp::ilp_problem_t> m_ilp_gold;
    std::vector<ilp::ilp_solution_t> m_sol;
    std::vector<ilp::ilp_solution_t> m_sol_gold;
    std::unique_ptr<opt::training_result_t> m_train_result;

    // ---- FOR MEASURE TIME
    duration_time_t
        m_time_for_enumerate,
        m_time_for_convert, m_time_for_convert_gold,
        m_time_for_solve, m_time_for_solve_gold,
        m_time_for_learn, m_time_for_infer;
};


inline bool is_verbose(verboseness_e v) { return phillip_main_t::verbose() >= v; }


}


#define IF_VERBOSE(V, E) if(phillip_main_t::verbose() >= V) util::print_console(E);

#define IF_VERBOSE_1(E) IF_VERBOSE(phil::VERBOSE_1, E)
#define IF_VERBOSE_2(E) IF_VERBOSE(phil::VERBOSE_2, E)
#define IF_VERBOSE_3(E) IF_VERBOSE(phil::VERBOSE_3, E)
#define IF_VERBOSE_4(E) IF_VERBOSE(phil::VERBOSE_4, E)
#define IF_VERBOSE_FULL(E) IF_VERBOSE(phil::FULL_VERBOSE, E)


#include "./phillip.inline.h"

