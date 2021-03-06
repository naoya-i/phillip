/* -*- coding: utf-8 -*- */

#ifndef INCLUDE_HENRY_PROOF_GRAPH_INLINE_H
#define INCLUDE_HENRY_PROOF_GRAPH_INLINE_H


#include <algorithm>


namespace phil
{

namespace pg
{



inline const hash_set<pg::node_idx_t>& node_t::parents() const
{
    return m_parents;
}


inline const hash_set<pg::node_idx_t>& node_t::ancestors() const
{
    return m_ancestors;
}


inline const hash_set<pg::node_idx_t>& node_t::relatives() const
{
    return m_relatives;
}


inline void node_t::set_master_hypernode(hypernode_idx_t idx)
{
    m_master_hypernode_idx = idx;
}


inline hypernode_idx_t node_t::master_hypernode() const
{
    return m_master_hypernode_idx;
}


inline bool node_t::is_equality_node() const
{
    return (m_literal.predicate == "=" and m_literal.truth);
}


inline bool node_t::is_non_equality_node() const
{
    return (m_literal.predicate == "=" and not m_literal.truth);
}


inline bool node_t::is_transitive_equality_node() const
{
    return is_equality_node() and m_master_hypernode_idx == -1;
}


inline std::string node_t::to_string() const
{
    return m_literal.to_string() + util::format(":%d", m_index);
}


inline edge_t::edge_t()
    : m_type(EDGE_UNDERSPECIFIED),
      m_index_tail(-1), m_index_head(-1), m_axiom_id(-1)
{}


inline edge_t::edge_t(
    edge_type_e type, hypernode_idx_t tail, hypernode_idx_t head, axiom_id_t id )
    : m_type(type), m_index_tail(tail), m_index_head(head), m_axiom_id(id)
{}


inline bool edge_t::is_chain_edge() const
{
    return type() == EDGE_HYPOTHESIZE or type() == EDGE_IMPLICATION;
}


inline bool edge_t::is_unify_edge() const
{
    return type() == EDGE_UNIFICATION;
}


inline unifier_t::unifier_t( const term_t &x, const term_t &y )
{ add(x, y); }


inline const term_t* unifier_t::find_substitution_term(const term_t &x) const
{
    auto found = m_mapping.find(x);
    return (found != m_mapping.end()) ? &(found->second) : NULL;
}


inline const std::set<literal_t>& unifier_t::substitutions() const
{
    return m_substitutions;
}


inline const hash_map<term_t, term_t>& unifier_t::mapping() const
{
    return m_mapping;
}


inline void unifier_t::add(term_t x, term_t y)
{
    if (x == y) return;
    //if (x < y) std::swap(x, y);

    literal_t added("=", x, y);
    m_substitutions.insert(added);
    m_mapping[x] = y;
}


inline void unifier_t::clear()
{
    m_substitutions.clear();
    m_mapping.clear();
}


inline bool unifier_t::empty() const
{
    return m_substitutions.empty();
}


inline const hash_map<index_t, hash_set<term_t> >&
proof_graph_t::unifiable_variable_clusters_set_t::clusters() const
{ return m_clusters; }


inline const hash_set<term_t>* proof_graph_t
    ::unifiable_variable_clusters_set_t::find_cluster(term_t t) const
{
    auto it = m_map_v2c.find(t);
    return (it != m_map_v2c.end()) ? &m_clusters.at(it->second) : NULL;
}


inline bool
proof_graph_t::unifiable_variable_clusters_set_t::is_in_same_cluster(
    term_t t1, term_t t2 ) const
{
    auto i_v1 = m_map_v2c.find(t1);
    if( i_v1 == m_map_v2c.end() ) return false;
    auto i_v2 = m_map_v2c.find(t2);
    if( i_v2 == m_map_v2c.end() ) return false;
    return ( i_v1->second == i_v2->second );
}


inline node_idx_t proof_graph_t::
    add_observation(const literal_t &lit, int depth)
{
    int idx = add_node(lit, NODE_OBSERVABLE, depth, hash_set<node_idx_t>());
    std::list<std::tuple<node_idx_t, unifier_t> > muex;

    get_mutual_exclusions(lit, &muex);
    _generate_mutual_exclusions(idx, muex);
    _generate_unification_assumptions(idx);
    m_observations.insert(idx);

    return idx;
}


inline hypernode_idx_t proof_graph_t::backward_chain(
    const std::vector<node_idx_t> &target, const lf::axiom_t &axiom)
{
    return chain(target, axiom, true);
}


inline hypernode_idx_t proof_graph_t::forward_chain(
    const std::vector<node_idx_t> &target, const lf::axiom_t &axiom)
{
    return chain(target, axiom, false);
}


inline const std::vector<node_t>& proof_graph_t::nodes() const
{ return m_nodes; }


inline const node_t& proof_graph_t::node( node_idx_t i ) const
{ return m_nodes.at(i); }


inline const std::vector<edge_t>& proof_graph_t::edges() const
{
    return m_edges;
}


inline const edge_t& proof_graph_t::edge( edge_idx_t i ) const
{
    return m_edges.at(i);
}


inline const std::vector< std::vector<node_idx_t> >&
    proof_graph_t::hypernodes() const
{
        return m_hypernodes;
}


inline const std::vector<node_idx_t>&
proof_graph_t::hypernode(hypernode_idx_t i) const
{
    static const std::vector<node_idx_t> empty;
    return (i >= 0) ? m_hypernodes.at(i) : empty;
}


inline const hash_set<node_idx_t>& proof_graph_t::observation_indices() const
{
    return m_observations;
}


inline const std::vector<requirement_t>& proof_graph_t::requirements() const
{
    return m_requirements;
}


inline const unifier_t* proof_graph_t::search_mutual_exclusion_of_node(
    node_idx_t n1, node_idx_t n2) const
{
    return m_mutual_exclusive_nodes.find(n1, n2);
}


inline const hash_set<node_idx_t>*
proof_graph_t::search_nodes_with_term( term_t term ) const
{
    auto iter_tm = m_maps.term_to_nodes.find( term );
    return ( iter_tm != m_maps.term_to_nodes.end() ) ? &iter_tm->second : NULL;
}


inline const hash_set<node_idx_t>* proof_graph_t::search_nodes_with_predicate(
    predicate_t predicate, int arity ) const
{
    auto iter_nm = m_maps.predicate_to_nodes.find( predicate );
    if( iter_nm == m_maps.predicate_to_nodes.end() ) return NULL;

    auto iter_an = iter_nm->second.find( arity );
    if( iter_an == iter_nm->second.end() ) return NULL;

    return &iter_an->second;
}


inline const hash_set<node_idx_t>*
proof_graph_t::search_nodes_with_arity(const arity_t &arity) const
{
    int idx(arity.rfind('/')), num;
    assert( idx > 0 );
    _sscanf( arity.substr(idx+1).c_str(), "%d", &num );

    return search_nodes_with_predicate(arity.substr(0, idx), num);
}


inline const hash_set<node_idx_t>*
proof_graph_t::search_nodes_with_arity(const kb::arity_id_t arity) const
{
    auto found = m_maps.arity_to_nodes.find(arity);
    return (found != m_maps.arity_to_nodes.end()) ? &found->second : NULL;
}


inline const hash_set<node_idx_t>*
proof_graph_t::search_nodes_with_depth(depth_t depth) const
{
    auto it = m_maps.depth_to_nodes.find( depth );
    return (it == m_maps.depth_to_nodes.end()) ? NULL : &it->second;
}


inline const hash_set<edge_idx_t>*
    proof_graph_t::search_edges_with_hypernode( hypernode_idx_t idx ) const
{
    auto it = m_maps.hypernode_to_edge.find(idx);
    return (it == m_maps.hypernode_to_edge.end()) ? NULL : &it->second;
}


inline const hash_set<edge_idx_t>*
proof_graph_t::search_edges_with_node_in_head(node_idx_t idx) const
{
    auto found = m_maps.head_node_to_edges.find(idx);
    return (found == m_maps.head_node_to_edges.end()) ? NULL : &found->second;
}


inline const hash_set<edge_idx_t>*
proof_graph_t::search_edges_with_node_in_tail(node_idx_t idx) const
{
    auto found = m_maps.tail_node_to_edges.find(idx);
    return (found == m_maps.tail_node_to_edges.end()) ? NULL : &found->second;
}


inline const hash_set<hypernode_idx_t>*
proof_graph_t::search_hypernodes_with_node( node_idx_t node_idx ) const
{
    auto it = m_maps.node_to_hypernode.find( node_idx );
    return (it == m_maps.node_to_hypernode.end()) ? NULL : &it->second;
}


template<class It> const hash_set<hypernode_idx_t>*
proof_graph_t::find_hypernode_with_unordered_nodes(It begin, It end) const
{
    size_t hash = get_hash_of_nodes(std::list<node_idx_t>(begin, end));
    auto find = m_maps.unordered_nodes_to_hypernode.find(hash);
    return (find != m_maps.unordered_nodes_to_hypernode.end()) ?
        &(find->second) : NULL;
}


inline hypernode_idx_t
    proof_graph_t::find_parental_hypernode( hypernode_idx_t idx ) const
{
    edge_idx_t e = find_parental_edge(idx);
    return (e >= 0) ? edge(e).tail() : -1;
}


inline const hash_set<term_t>*
proof_graph_t::find_variable_cluster( term_t t ) const
{
    return m_vc_unifiable.find_cluster(t);
}


template <class IterNodesArray>
bool proof_graph_t::check_nodes_coexistability(IterNodesArray begin, IterNodesArray end) const
{
#ifdef DISABLE_CANCELING
    return true;
#else
    for (auto n1 = begin; n1 != end; ++n1)
    for (auto n2 = begin; n2 != n1; ++n2)
    if (not _check_nodes_coexistability(*n1, *n2))
        return false;
    return true;
#endif
}


inline bool proof_graph_t::
is_hypernode_for_unification(hypernode_idx_t idx) const
{ return m_indices_of_unification_hypernodes.count(idx) > 0; }


inline void proof_graph_t::add_attribute(const std::string &name, const std::string &value)
{
    m_attributes[name] = value;
}


inline float proof_graph_t::threshold_distance_for_soft_unifying() const
{
    return m_threshold_distance_for_soft_unify;
}


inline bool proof_graph_t::_is_considered_unification(
    node_idx_t i, node_idx_t j ) const
{
    return m_temporal.considered_unifications.count(i, j) > 0;
}


inline int proof_graph_t::get_depth_of_deepest_node(hypernode_idx_t idx) const
{
    return get_depth_of_deepest_node(hypernode(idx));
}


}

}


#endif
