#ifndef INCLUDE_HENRY_HENRY_INLINE_H
#define INCLUDE_HENRY_HENRY_INLINE_H


namespace phil
{


inline void phillip_main_t::set_verbose(int v)
{
    ms_verboseness = v;
}


inline const int phillip_main_t::verbose()
{
    return ms_verboseness;
}


inline const lhs_enumerator_t* phillip_main_t::lhs_enumerator() const
{
    return m_lhs_enumerator;
}


inline lhs_enumerator_t* phillip_main_t::lhs_enumerator()
{
    return m_lhs_enumerator;
}


inline const ilp_converter_t* phillip_main_t::ilp_convertor() const
{
    return m_ilp_convertor;
}


inline ilp_converter_t* phillip_main_t::ilp_convertor()
{
    return m_ilp_convertor;
}


inline const ilp_solver_t* phillip_main_t::ilp_solver() const
{
    return m_ilp_solver;
}


inline ilp_solver_t* phillip_main_t::ilp_solver()
{
    return m_ilp_solver;
}


inline void phillip_main_t::set_input(const lf::input_t &ipt)
{
    if (m_input != NULL)
        delete m_input;
    m_input = new lf::input_t(ipt);
}


inline void phillip_main_t::set_lhs_enumerator(lhs_enumerator_t *ptr)
{
    if (m_lhs_enumerator != NULL)
        delete m_lhs_enumerator;
    m_lhs_enumerator = ptr;
}


inline void phillip_main_t::set_ilp_convertor(ilp_converter_t *ptr)
{
    if (m_ilp_convertor != NULL)
        delete m_ilp_convertor;
    m_ilp_convertor = ptr;
}


inline void phillip_main_t::set_ilp_solver(ilp_solver_t *ptr)
{
    if (m_ilp_solver != NULL)
        delete m_ilp_solver;
    m_ilp_solver = ptr;
}


inline void phillip_main_t::set_param(
    const std::string &key, const std::string &param )
{
    m_params[key] = param;
}


inline void phillip_main_t::erase_param(const std::string &key)
{
    m_params.erase(key);
}


inline void phillip_main_t::set_flag(const std::string &key)
{
    m_flags.insert(key);
}


inline void phillip_main_t::erase_flag(const std::string &key)
{
    m_flags.erase(key);
}


inline const lf::input_t* phillip_main_t::get_input() const
{
    return m_input;
}


inline const lf::logical_function_t* phillip_main_t::get_observation() const
{
    return (m_input != NULL) ? &m_input->obs : NULL;
}


inline const lf::logical_function_t* phillip_main_t::get_requirement() const
{
    return (m_input != NULL) ? &m_input->req : NULL;
}


inline const pg::proof_graph_t* phillip_main_t::get_latent_hypotheses_set() const
{
    return m_lhs;
}


inline const ilp::ilp_problem_t* phillip_main_t::get_ilp_problem() const
{
    return m_ilp;
}


inline const std::vector<ilp::ilp_solution_t>& phillip_main_t::get_solutions() const
{
    return m_sol;
}


inline const hash_map<std::string, std::string>& phillip_main_t::params() const
{ return m_params; }


inline const std::string& phillip_main_t::param(const std::string &key) const
{
    static const std::string empty_str("");
    auto found = m_params.find(key);
    return (found != m_params.end()) ? found->second : empty_str;
}


inline int phillip_main_t::param_int(const std::string &key, int def) const
{
    int out(def);
    _sscanf(param(key).c_str(), "%d", &out);
    return out;
}


inline float phillip_main_t::param_float(const std::string &key, float def) const
{
    float out(def);
    _sscanf(param(key).c_str(), "%f", &out);
    return out;
}


inline const hash_set<std::string>& phillip_main_t::flags() const
{
    return m_flags;
}


inline bool phillip_main_t::flag(const std::string &key) const
{
    return m_flags.find(key) != m_flags.end();
}


inline bool phillip_main_t::do_infer_pseudo_positive() const
{
    return flag("get_pseudo_positive");
}


inline float phillip_main_t::get_time_for_lhs()  const
{
    return m_time_for_enumerate;
}


inline float phillip_main_t::get_time_for_ilp()  const
{
    return m_time_for_convert;
}


inline float phillip_main_t::get_time_for_sol()  const
{
    return m_time_for_solve;
}


inline float phillip_main_t::get_time_for_infer() const
{
    return m_time_for_infer;
}


inline void phillip_main_t::add_target(const std::string &name)
{
    m_target_obs_names.insert(name);
}


inline void phillip_main_t::clear_targets()
{
    m_target_obs_names.clear();
}


inline bool phillip_main_t::is_target(const std::string &name) const
{
    return m_target_obs_names.empty() ? true : (m_target_obs_names.count(name) > 0);
}


inline void phillip_main_t::add_exclusion(const std::string &name)
{
    m_excluded_obs_names.insert(name);
}


inline void phillip_main_t::clear_exclusions()
{
    m_excluded_obs_names.clear();
}


inline bool phillip_main_t::is_excluded(const std::string &name) const
{
    return m_excluded_obs_names.empty() ? false : (m_excluded_obs_names.count(name) > 0);
}


inline bool phillip_main_t::check_validity() const
{
    bool can_infer =
        (m_lhs_enumerator != NULL) and
        (m_ilp_convertor != NULL) and
        (m_ilp_solver != NULL);

    if (not can_infer)
    {
        if (lhs_enumerator() == NULL)
            throw phillip_exception_t(
            "Cannot infer, because of lack of lhs_enumerator.");
        if (ilp_convertor() == NULL)
            throw phillip_exception_t(
            "Cannot infer, because of lack of ilp_convertor.");
        if (ilp_solver() == NULL)
            throw phillip_exception_t(
            "Cannot infer, because of lack of ilp_solver.");
    }

    return can_infer;
}


inline void phillip_main_t::reset_for_inference()
{
    if (m_input != NULL) delete m_input;
    if (m_lhs != NULL)   delete m_lhs;
    if (m_ilp != NULL)   delete m_ilp;

    m_input = NULL;
    m_lhs = NULL;
    m_ilp = NULL;

    m_time_for_enumerate = 0.0f;
    m_time_for_convert = 0.0f;
    m_time_for_convert_gold = 0.0f;
    m_time_for_solve = 0.0f;
    m_time_for_solve_gold = 0.0f;
    m_time_for_learn = 0.0f;
    m_time_for_infer = 0.0f;

    m_sol.clear();
}


inline void phillip_main_t::execute_enumerator()
{
    execute_enumerator(
        &m_lhs, &m_time_for_enumerate, param("path_lhs_out"));
}


inline void phillip_main_t::execute_convertor()
{
    execute_convertor(
        &m_ilp, &m_time_for_convert, param("path_ilp_out"));
}


inline void phillip_main_t::execute_solver()
{
    execute_solver(
        &m_sol, &m_time_for_solve, param("path_sol_out"));
}


}

#endif
